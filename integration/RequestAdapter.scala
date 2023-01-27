package vexriscv.demo.smp

import spinal.core._
import spinal.lib._

case class RequestAdapter(conf: DittyConfiguration) extends Component {

  val genReq = Request()
  val genHopliteReq = RequestHopliteRT(conf.intc_config)
  val io = new Bundle {
    val in = slave Stream(genReq)
    val out = master Stream(genHopliteReq)
  }
  val in_reg = VReg(genReq)
  val dest = Reg(HardType(io.in.payload.dst))
  // TODO: make this parametrized
  val counter = Counter(0 until 32, io.out.fire)
  val cur_value = Mux(
    counter.value >= 16, counter.value - 16, counter.value
  )

  val cur_x = cur_value / conf.intc_config.Y_DIM
  val cur_y = cur_value % conf.intc_config.Y_DIM
  val to_cur_counter = in_reg.value.dst(counter)

  io.in.ready := !in_reg.valid
  // updating payload
  io.out.payload := in_reg.value
  when(counter.value >= 16) {
    // This is used only in the private cache
    // to determine whether its going into the directories
    io.out.payload.respond_to_src_network_id(31) := Bool(true)
  }
  io.out.payload.x := cur_x.resize(io.out.payload.x.getBitsWidth)
  io.out.payload.y := cur_y.resize(io.out.payload.y.getBitsWidth)
  io.out.valid := in_reg.valid && to_cur_counter
  when(io.in.fire) {
    in_reg.write(io.in.payload)
    dest := io.in.payload.dst
  }
  when(to_cur_counter && io.out.fire || !to_cur_counter && in_reg.valid) {
    counter.increment()
    when(counter.willOverflow) {
      counter.clear()
      in_reg.invalidate()
    }
  }
  // < 16 -> core
  // >= 16 -> dir


}
