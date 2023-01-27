package vexriscv.demo.smp

import spinal.core.Component.push
import spinal.core._
import spinal.lib._

import java.nio.file.{Files, StandardCopyOption}
import reflect.io._
import Path._

case class LLCConfig(
                      isDitty: Boolean = true,
                      designFolder: String = "/home/allen/mount2/CLionProjects/vitis_system/cmake-build-debug/llc/solution_1/syn/verilog",
                      bb_name: String = "llc"
                    )
case class LLC(ditty: DittyConfiguration) extends Component {
  import ditty.llc._
  val genHopliteRTReq = RequestHopliteRT(ditty.intc_config)
  val genReq = Request()
  val genResp = Response()
  val genDRAMReq = DRAMRequest()
  val genDRAMResp = DRAMResponse()
  val isGP = !isDitty

  val io = new Bundle {
    val from_request = slave Stream(genHopliteRTReq)

    val to_forward = master Stream(genHopliteRTReq)

    val from_response = slave Stream(genHopliteRTReq)
    val to_response = master Stream(genHopliteRTReq)

    val dram_req = master Stream(genDRAMReq)
    val dram_resp = slave Stream(genDRAMResp)
  }

  val rsp_in = Stream(genHopliteRTReq)
  val dram_req = Stream(genDRAMReq)
  val dram_resp = Stream(genDRAMResp)
  val to_intc = Stream(genReq)
  rsp_in << io.from_response.queue(16)
  dram_resp << io.dram_resp
  dram_req >> io.dram_req


  /**
   * (req_in_s, wb_in_s) <- from_req_q <- io.from_req
   */
  val from_req_q = io.from_request.queue(1)
  val (req_in_s, wb_in_s) = if(isDitty) {
    val ditty_fork = new Area {
      val r = StreamFork2(from_req_q, synchronous = true)
    }
    ditty_fork.r
  } else {
    val gp_demux = new Area {
      val is_combined = from_req_q.valid && from_req_q.payload.is_combined()
      val r = StreamDemux(from_req_q, is_combined.asUInt, 2)
    }
    (gp_demux.r(0), gp_demux.r(1))
  }

  val req_in = req_in_s.queue(16)
  val pre_wb_in_s = wb_in_s.queue(16)
  val wb_in = StreamFifo(genReq, 16)

  wb_in.io.push.valid := pre_wb_in_s.valid && pre_wb_in_s.payload.is_combined()
  wb_in.io.push.payload := pre_wb_in_s.payload
  // over-write write back types and address
  wb_in.io.push.payload.allowOverride()
  wb_in.io.push.payload.addr := pre_wb_in_s.wb_addr
  pre_wb_in_s.ready := wb_in.io.push.ready

  val putAckQueue = isGP.generate {
    // sending response to putAck only when it is GP protocol
    val put_ack_queue = StreamFifo(genReq, 16)
    // set response type into PutAck
    // TODO: replace the response generation with parametrized design
    require(RequestDitty.PutAck.id == 10)
    // drive data
    put_ack_queue.io.push.payload.addr := (wb_in.io.push.payload.addr & 0xfffffff0L) | RequestDitty.PutAck.id
    put_ack_queue.io.push.payload.wb_addr := 0
    put_ack_queue.io.push.payload.data := 0
    // the directory starts at 16
    // sending to respond network (3)
    put_ack_queue.io.push.payload.respond_to_src_network_id := 0
    put_ack_queue.io.push.payload.respond_to_src_network_id.allowOverride()
    put_ack_queue.io.push.payload.set_src(16)
    put_ack_queue.io.push.payload.set_network_id(3)
    put_ack_queue.io.push.payload.dst := (U(1, 32 bits) << wb_in.io.push.payload.get_src()).resize(32 bits)
    put_ack_queue.io.push.valid := Bool(false)

    // responding with PutAck
    when(wb_in.io.push.fire) { // send back PutAck to unblock things..
      assert(put_ack_queue.io.push.ready, "PutAck enqueue must not block")
      put_ack_queue.io.push.valid := Bool(true)
    }
    put_ack_queue
  }


  val isForward = ((to_intc.payload.respond_to_src_network_id & 0x3) === 0x2).asUInt
  val demuxedIntc = StreamDemux(to_intc, isForward, 2)
  val to_forward_strm = demuxedIntc(1)
  val to_response_strm = demuxedIntc(0)
  val respAdapter = RequestAdapter(ditty)
  val forwardAdapter = RequestAdapter(ditty)
  isDitty.generate {
    respAdapter.io.in << to_response_strm
  }
  isGP.generate {
    respAdapter.io.in << StreamArbiterFactory().lowerFirst.onArgs(putAckQueue.io.pop, to_response_strm)
  }
  respAdapter.io.out >> io.to_response
  forwardAdapter.io.in << to_forward_strm
  forwardAdapter.io.out >> io.to_forward

  val llc_inst = new BlackBox {
    setBlackBoxName(bb_name)
    val io = new Bundle {
      val ap_clk = in Bool()
      val ap_rst_n = in Bool()

      val req_in_TDATA = in UInt (256 bits)
      val req_in_TVALID = in Bool()
      val req_in_TREADY = out Bool()

      val wb_in_TDATA = in UInt (256 bits)
      val wb_in_TVALID = in Bool()
      val wb_in_TREADY = out Bool()

      val rsp_in_TDATA = in UInt (256 bits)
      val rsp_in_TVALID = in Bool()
      val rsp_in_TREADY = out Bool()

      val dram_req_TDATA = out UInt (256 bits)
      val dram_req_TVALID = out Bool()
      val dram_req_TREADY = in Bool()

      val dram_resp_TDATA = in UInt (256 bits)
      val dram_resp_TVALID = in Bool()
      val dram_resp_TREADY = out Bool()

      val to_intc_TDATA = out UInt (256 bits)
      val to_intc_TVALID = out Bool()
      val to_intc_TREADY = in Bool()
    }


    noIoPrefix()
    designFolder.toDirectory.files.map(_.path).filter(_.endsWith(".v")).foreach(f => {
      // println(s"Elaborating llc design, adding files: ${f}")
      addRTLPath(f)
    })
    designFolder.toDirectory.files.map(_.path).filter(_.endsWith(".dat")).foreach(f => {
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

  // NOTE: the dst does not matter in this scenario
  llc_inst.io.req_in_TDATA := req_in.payload.asBits.asUInt.resize(256 bits)
  llc_inst.io.req_in_TVALID := req_in.valid
  req_in.ready := llc_inst.io.req_in_TREADY

  llc_inst.io.wb_in_TDATA := wb_in.io.pop.payload.asBits.asUInt
  llc_inst.io.wb_in_TVALID := wb_in.io.pop.valid
  wb_in.io.pop.ready := llc_inst.io.wb_in_TREADY

  llc_inst.io.rsp_in_TDATA := rsp_in.payload.asBits.asUInt.resize(256 bits)
  llc_inst.io.rsp_in_TVALID := rsp_in.valid
  rsp_in.ready := llc_inst.io.rsp_in_TREADY

  to_intc.payload := llc_inst.io.to_intc_TDATA.as(genReq)
  to_intc.valid := llc_inst.io.to_intc_TVALID
  llc_inst.io.to_intc_TREADY := to_intc.ready

  dram_req.payload := llc_inst.io.dram_req_TDATA.as(genDRAMReq)
  dram_req.valid := llc_inst.io.dram_req_TVALID
  llc_inst.io.dram_req_TREADY := dram_req.ready

  llc_inst.io.dram_resp_TDATA := dram_resp.payload.asBits.asUInt.resize(256 bits)
  llc_inst.io.dram_resp_TVALID := dram_resp.valid
  dram_resp.ready := llc_inst.io.dram_resp_TREADY
}
