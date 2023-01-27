package vexriscv.demo.smp

import spinal.core._
import spinal.lib._

import java.nio.file.{Files, StandardCopyOption}
object SystemKernel extends App {
  def getGPConfig(x: Int, y: Int): DittyConfiguration = {
    val gp = DittyConfiguration(
      intc_config = HopliteRTConfig(x, y, x * y, x * y),
      private_cache = PrivateCacheConfig(
        isDitty = false
      ),
      llc = LLCConfig(
        isDitty = false
      ),
      nCores = x * y,
      isDitty = false
    )
    gp
  }

  def getDittyConfig(x: Int, y: Int): DittyConfiguration = {
    val ditty = DittyConfiguration(
      intc_config = HopliteRTConfig(x, y, x * y, x * y),
      private_cache = PrivateCacheConfig(),
      llc = LLCConfig(),
      nCores = x * y
    )
    ditty
  }
  // generate Verilog, and Vitis kernel description
  println("Generating system kernel for ditty")
  val x = 2
  for {
    y <- 1 until 6
    is_ditty <- Seq(true, false)
  } {
    val name = s"${if(is_ditty) "ditty" else "gp"}_${x}_${y}"
    val dittyConfiguration = if(is_ditty) getDittyConfig(x, y) else getGPConfig(x, y)
    val conf = SpinalConfig(
      mode = SystemVerilog,
      targetDirectory = s"${name}"
    ).generate({
      val a = new Component {
        setDefinitionName("system_kernel")
        val io = new Bundle {
          val clk = in Bool()
          val reset = in Bool()
          val from_core = Vec(slave Stream (UInt(256 bits)), dittyConfiguration.nCores)
          val to_core = Vec(master Stream (UInt(256 bits)), dittyConfiguration.nCores)
          val dram_req = Vec(master Stream (UInt(256 bits)), dittyConfiguration.nLLC)
          val dram_resp = Vec(slave Stream (UInt(256 bits)), dittyConfiguration.nLLC)
        }
        val internalArea = new ClockingArea(
          ClockDomain(io.clk, io.reset, config=ClockDomainConfig(
            clockEdge = RISING,
            resetKind = SYNC, resetActiveLevel = LOW
          ))
        ) {

          val internal = new SystemKernel(dittyConfiguration)
          internal.io.to_core.zip(io.to_core).foreach(f => {
            f._2.translateFrom(f._1) { (g, x) =>
              g := x.asBits.asUInt
            }
          })
          io.from_core.zip(internal.io.from_core).foreach(f => {
            f._2.translateFrom(f._1) { (g, x) => g := x.as(g) }
          })
          internal.io.dram_req.zip(io.dram_req).foreach(f => {
            f._2.translateFrom(f._1) { (g, x) =>
              g := x.asBits.resize(256).asUInt
            }
          })
          io.dram_resp.zip(internal.io.dram_resp).foreach(f => {
            f._2.translateFrom(f._1) { (g, x) => g := x.as(g) }
          })
        }
        noIoPrefix()

      }
      for {n <- a.internalArea.internal.pcaches(0).private_cache_inst.listRTLPath ++ a.internalArea.internal.llcs(0).llc_inst.listRTLPath ++ a.internalArea.internal.req.hopliteRouter.listRTLPath} {
        val p = java.nio.file.Paths.get(n)
        // copy RTLs to the folder
        Files.copy(
          p,
          java.nio.file.Paths.get(s"${name}" + "/" + p.getFileName),
          StandardCopyOption.REPLACE_EXISTING
        )
      }
      a
    })
  }
}
case class SystemKernel(ditty: DittyConfiguration) extends Component {
  val genReq = HardType(Request())
  val genHopliteRT = HardType(RequestHopliteRT(ditty.intc_config))
  val genDRAMReq = HardType(DRAMRequest())
  val genDRAMResp = HardType(DRAMResponse())
  val genResp = HardType(Response())

  require(ditty.nLLC == 1, "Only 1 LLC is supported for the moment")
  require(ditty.nCores == ditty.intc_config.X_DIM * ditty.intc_config.Y_DIM)
  val io = new Bundle {
    // the io for the cores
    val from_core = Vec(slave(Stream(genReq)), ditty.nCores)
    val to_core = Vec(master(Stream(genResp)), ditty.nCores)

    val dram_req = Vec(master(Stream(genDRAMReq)), ditty.nLLC)
    val dram_resp = Vec(slave(Stream(genDRAMResp)), ditty.nLLC)
  }

  // 3 interconnects
  val req = HopliteRT(genHopliteRT(), ditty.intc_config)
  val fwd = HopliteRT(genHopliteRT(), ditty.intc_config)
  val rsp = HopliteRT(genHopliteRT(), ditty.intc_config)

  val reqFromExt = req.io.fromExt.map(_.toIndexedSeq).toIndexedSeq.flatten
  val reqToExt = req.io.toExt.map(_.toIndexedSeq).toIndexedSeq.flatten

  val fwdFromExt = fwd.io.fromExt.map(_.toIndexedSeq).toIndexedSeq.flatten
  val fwdToExt = fwd.io.toExt.map(_.toIndexedSeq).toIndexedSeq.flatten

  val respToExt = rsp.io.toExt.map(_.toIndexedSeq).toIndexedSeq.flatten
  val respFromExt = rsp.io.fromExt.map(_.toIndexedSeq).toIndexedSeq.flatten

  // tying off


  // core -> req -> llc
  // core <- fwd <- llc
  // core <> rsp <> llc
  val pcaches = (0 until ditty.nCores).map { i =>
    val private_cache = PrivateCache(ditty, i)
    private_cache.io.id := i
    private_cache.io.from_core << io.from_core(i)
    private_cache.io.to_core >> io.to_core(i)
    private_cache.io.to_request >> reqFromExt(i)
    private_cache.io.from_forward << fwdToExt(i)
    private_cache
  }
  val llcs = (0 until ditty.nLLC).map { i =>
    val llc = LLC(ditty)
    llc.io.dram_req >> io.dram_req(i)
    llc.io.dram_resp << io.dram_resp(i)
    llc.io.from_request << reqToExt(i)
    llc.io.to_forward >> fwdFromExt(i)
    llc
  }

  val resp_arbitration = for { i <- 0 until (ditty.nCores max ditty.nLLC) } yield new Area {
    if(i < ditty.nCores && i < ditty.nLLC) {
      val r = StreamArbiterFactory().roundRobin.onArgs(pcaches(i).io.to_response, llcs(i).io.to_response)
      r >> respFromExt(i)
      // how do we decide on which to respond...
      val sel = respToExt(i).payload.respond_to_src_network_id(31).asUInt(1 bits)
      val rsp = StreamDemux(respToExt(i), sel, 2)
      pcaches(i).io.from_response << rsp(0)
      llcs(i).io.from_response << rsp(1)
      // must be arbitrated
    } else {
      pcaches(i).io.from_response << respToExt(i)
      pcaches(i).io.to_response >> respFromExt(i)
    }
  }
  for { i <- (ditty.nCores min ditty.nLLC) until  (ditty.nCores max ditty.nLLC)} {
    fwdFromExt(i).valid := Bool(false)
    fwdFromExt(i).payload := U(0).resize(256 bits).as(genHopliteRT)
    reqToExt(i).ready := Bool(false)
  }

}
