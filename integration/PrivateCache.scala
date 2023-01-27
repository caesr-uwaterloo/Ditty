package vexriscv.demo.smp

import spinal.core.Component.push
import spinal.core._
import spinal.lib._

import java.nio.file.{CopyOption, Files, StandardCopyOption}
import reflect.io._
import Path._
import scala.tools.nsc.classpath.FileUtils


case class PrivateCacheConfig(
                               isDitty: Boolean = true,
                               designFolder: String = "/home/allen/mount2/CLionProjects/vitis_system/cmake-build-debug/private_cache/solution_1/syn/verilog/",
                               bb_name: String = "private_cache"
                             )
case class PrivateCache( ditty: DittyConfiguration, id: Int) extends Component {
  val conf = ditty.private_cache
  import conf._
  val isGP = !isDitty
  val genHopliteRTReq = RequestHopliteRT(ditty.intc_config)
  val genReq = HardType(Request())
  val genResp = HardType(Response())
  val io = new Bundle {
    val from_core = slave Stream(genReq)
    val from_forward = slave Stream(genHopliteRTReq)
    val from_response = slave Stream(genHopliteRTReq)

    val to_request = master Stream(genHopliteRTReq)
    // we don't have to forward lol
    val to_response = master Stream(genHopliteRTReq)
    val to_core = master Stream(genResp)
    val id = in UInt(32 bits)

    // val request_if = slave Stream(genReq)
    // val to_core = master Stream(genResp)
    // val to_intc = master Stream(genReq)
    // val id = in UInt(32 bits)
  }
  val busy = Reg(Bool()) init false
  when(io.to_core.fire) {
    busy := Bool(false)
  }
  when(io.from_core.fire) {
    busy := Bool(true)
  }

  // max 16 core
  val reqQ = StreamFifo(genReq(), 1)
  reqQ.io.push.payload := io.from_core.payload
  reqQ.io.push.valid := io.from_core.valid && !busy
  io.from_core.ready := !busy && reqQ.io.push.ready
  val from_core = reqQ.io.pop
  val wallclock = CounterFreeRun(BigInt(1) << 64)
  val latency = Counter(32 bits, busy)
  when(!busy) {
    latency.clear()
  }

  // assert(latency < (ditty.nCores * ditty.nCores * 100), s"Latency of ${id} should not be too large")

  // from_core_converted.valid := from_core.valid
  // from_core_converted.payload := from_core.payload
  // from_core.ready := from_core_converted.ready

  val from_forward = Stream(genReq).translateFrom(io.from_forward.queue(16))(_ := _)
  val from_response_p = Stream(genReq).translateFrom(io.from_response.queue(16))(_ := _)
  val from_response = Stream(genReq)
  val request_if = Stream(genReq)
  val to_intc = Stream(genReq)



  // to request or to response
  // 1 -> Request
  // 3 -> Response
  val isRequest = ((to_intc.payload.respond_to_src_network_id & 0x3) === 0x1).asUInt(1 bits)
  val demuxedIntc = StreamDemux(to_intc, isRequest, 2)
  // re-routing
  val to_req_strm = demuxedIntc(1)
  val wb_staller = isGP.generate {
    new Area {
      // -----
      //   | to_req_strm
      // ____
      // |  |
      val stalling = Reg(Bool()) init false

      val req_strm = StreamFifo(genReq(), 1)
      val wb_strm = StreamFifo(genReq(), 1)

      val push_wb = to_req_strm.payload.is_combined()
      val sync = req_strm.io.push.ready && wb_strm.io.push.ready

      to_req_strm.ready := sync

      // split wb from the normal request
      req_strm.io.push.payload := to_req_strm.payload
      req_strm.io.push.payload.allowOverride()
      req_strm.io.push.payload.wb_addr := 0
      req_strm.io.push.valid := sync && to_req_strm.valid

      wb_strm.io.push.payload := to_req_strm.payload
      wb_strm.io.push.valid := sync && to_req_strm.valid && push_wb

      when(wb_strm.io.push.fire) {
        stalling := Bool(true)
      }

    }
  }

  // priority arbitration
  isDitty.generate {
    // ditty directly accept the response
    from_response << from_response_p
    request_if << StreamArbiterFactory().lowerFirst.onArgs(
      from_response,
      StreamArbiterFactory().roundRobin.onArgs(
        from_core,
        from_forward
      )
    )
  }


  isGP.generate {
    val is_put_ack_resp = from_response_p.valid && from_response_p.payload.is_put_ack()
    val put_ack_strm = StreamDemux(from_response_p, is_put_ack_resp.asUInt, 2)
    from_response << put_ack_strm(0)
    // always receiving
    put_ack_strm(1).ready := Bool(true)
    when(put_ack_strm(1).fire) {
      // unblock the design
      assert(wb_staller.stalling, "PutAck should only be received when a request is stalled")
      wb_staller.stalling := Bool(false)
    }

    request_if << StreamArbiterFactory().lowerFirst.onArgs(
      from_response,
      from_forward,
      from_core
    )
  }

  // this is to split the PutAck etc...
  val to_resp_strm = demuxedIntc(0)
  val to_req_adapter = RequestAdapter(ditty)
  val to_resp_adapter = RequestAdapter(ditty)
  isDitty.generate {
    to_req_adapter.io.in << to_req_strm
  }
  isGP.generate {
    wb_staller.wb_strm.io.pop.ready := Bool(false)
    wb_staller.req_strm.io.pop.ready := Bool(false)
    when(wb_staller.stalling) {
      to_req_adapter.io.in << wb_staller.wb_strm.io.pop
    }.otherwise {
      to_req_adapter.io.in << wb_staller.req_strm.io.pop
    }
    // choose between wb_strm and req_strm
  }
  to_resp_adapter.io.in << to_resp_strm
  io.to_request << to_req_adapter.io.out.queue(2)
  io.to_response << to_resp_adapter.io.out.queue(32)




  val private_cache_inst = new BlackBox {
    setBlackBoxName(bb_name)
    val io = new Bundle {
      val ap_clk = in Bool()
      val ap_rst_n = in Bool()
      val request_if_TDATA = in UInt (256 bits)
      val request_if_TVALID = in Bool()
      val request_if_TREADY = out Bool()

      val to_core_TDATA = out UInt (256 bits)
      val to_core_TVALID = out Bool()
      val to_core_TREADY = in Bool()

      val to_intc_TDATA = out UInt (256 bits)
      val to_intc_TVALID = out Bool()
      val to_intc_TREADY = in Bool()
      val id = in UInt (32 bits)
    }

    noIoPrefix()
    designFolder.toDirectory.files.map(_.path).filter(_.endsWith(".v")).foreach(f => {
      // println(s"Elaborating private_cache design, adding files: ${f}")
      addRTLPath(f)
    })
    designFolder.toDirectory.files.map(_.path).filter(_.endsWith(".dat")).foreach( f => {
      val p = java.nio.file.Paths.get(f)
      // println(s"Copying: ${f} to ${System.getProperty("user.dir")}")
      Files.copy(
        java.nio.file.Paths.get(f),
        java.nio.file.Paths.get(System.getProperty("user.dir") + "/" + p.getFileName),
        StandardCopyOption.REPLACE_EXISTING
      )
    })
    mapCurrentClockDomain(io.ap_clk, io.ap_rst_n, resetActiveLevel = LOW)
  }
  private_cache_inst.io.id := io.id

  private_cache_inst.io.request_if_TDATA := request_if.payload.asBits.asUInt
  private_cache_inst.io.request_if_TVALID := request_if.valid
  request_if.ready := private_cache_inst.io.request_if_TREADY

  // we need response somehow
  io.to_core.payload := private_cache_inst.io.to_core_TDATA.as(genResp)
  io.to_core.payload.allowOverride()
  // recording latency
  io.to_core.payload.latency := latency
  io.to_core.payload.wallclock := wallclock
  io.to_core.valid := private_cache_inst.io.to_core_TVALID
  private_cache_inst.io.to_core_TREADY := io.to_core.ready

  to_intc.payload := private_cache_inst.io.to_intc_TDATA.as(genReq)
  to_intc.valid := private_cache_inst.io.to_intc_TVALID
  private_cache_inst.io.to_intc_TREADY := to_intc.ready

  // add buffer here for output (i.e. broadcast etc)
}