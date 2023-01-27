package vexriscv.demo.smp

import spinal.lib._
import spinal.core._

trait HasHopliteDestination {
  val x: UInt
  val y: UInt
}

case class HopliteRTConfig(
                            X_DIM: Int,
                            Y_DIM: Int,
                            MAX_RATE: Int,
                            MAX_TOKEN: Int
                          )

// It will be hard to directly test the BlackBox with SpinalHDL due to constraints on clockDomain initialization
// Thus we create a wrapper Component class to bypass the constraints
// This also allows us to use Streaming interfaces
case class HopliteRT[T <: Data with HasHopliteDestination](
                                                            genT: HardType[T],
                                                            conf: HopliteRTConfig) extends Component {
  import conf._
  // we ignore toExt.ready input
  val io = new Bundle {
    val toExt = Vec(Vec(master Stream[T](genT), Y_DIM), X_DIM)
    val fromExt = Vec(Vec(slave Stream[T](genT), Y_DIM), X_DIM)
  }

  // actual payload width
  val P_W = genT.getBitsWidth

  val hopliteRouter = HopliteRTInternal(
    P_W, X_DIM, Y_DIM, MAX_RATE, MAX_TOKEN
  )

  val to_ext_data = hopliteRouter.io.to_ext_data_o.as(Vec(Vec(genT, Y_DIM), X_DIM))
  val to_ext_valid = hopliteRouter.io.to_ext_valid_o.as(Vec(Vec(Bool(), Y_DIM), X_DIM))
  val from_ext_data = hopliteRouter.io.from_ext_data_i.as(Vec(Vec(genT, Y_DIM), X_DIM))
  val from_ext_valid = hopliteRouter.io.from_ext_valid_i.as(Vec(Vec(Bool(), Y_DIM), X_DIM))
  val from_ext_ready = hopliteRouter.io.from_ext_ready_o.as(Vec(Vec(Bool(), Y_DIM), X_DIM))

  for {
    i <- 0 until X_DIM
    j <- 0 until Y_DIM
  } {
    val startingCtrl = i * Y_DIM + j
    val startingData = i * Y_DIM * P_W + j * P_W
    //io.toExt(i)(j).payload := to_ext_data(i)(j)
    io.toExt(i)(j).payload := hopliteRouter.io.to_ext_data_o(startingData + P_W - 1 downto startingData).as(genT)
    //io.toExt(i)(j).valid := to_ext_valid(i)(j)
    io.toExt(i)(j).valid := hopliteRouter.io.to_ext_valid_o(startingCtrl).as(Bool())

    //from_ext_data(i)(j) := io.fromExt(i)(j).payload
    //from_ext_valid(i)(j) := io.fromExt(i)(j).valid
    hopliteRouter.io.from_ext_data_i(startingData + P_W - 1 downto startingData) := io.fromExt(i)(j).payload.asBits
    hopliteRouter.io.from_ext_valid_i(startingCtrl) := io.fromExt(i)(j).valid
    io.fromExt(i)(j).ready := hopliteRouter.io.from_ext_ready_o(startingCtrl).as(Bool())
  }

}

case class HopliteRTInternal(
               P_W: Int,
               X_DIM: Int,
               Y_DIM: Int,
               MAX_RATE: Int,
               MAX_TOKEN: Int
               ) extends BlackBox {
  require(log2Up(X_DIM) + log2Up(Y_DIM) < P_W, "D_W must large enough to hold destination")
  require(MAX_RATE >= 1)
  require(MAX_TOKEN >= 1)
  setBlackBoxName("hoplite")
  addGeneric("P_W", P_W)
  addGeneric("X_DIM", X_DIM)
  addGeneric("Y_DIM", Y_DIM)
  addGeneric("MAX_RATE", MAX_RATE)
  addGeneric("MAX_TOKEN", MAX_TOKEN)



  val io = new Bundle {
    val clk = in Bool()
    val rst = in Bool()
    val to_ext_data_o = out Bits(P_W * X_DIM * Y_DIM bits)
    val to_ext_valid_o = out Bits(X_DIM * Y_DIM bits)

    val from_ext_data_i = in Bits (P_W * X_DIM * Y_DIM bits)
    val from_ext_valid_i = in  Bits (X_DIM * Y_DIM bits)
    val from_ext_ready_o = out Bits (X_DIM * Y_DIM bits)
  }

  mapCurrentClockDomain(clock=io.clk, reset=io.rst)

  noIoPrefix()

  // path to the RTL
  addRTLPath("/opt/hoplites/rtl/counter.v")
  addRTLPath("/opt/hoplites/rtl/hoplite.sv")
  addRTLPath("/opt/hoplites/rtl/mux.v")
  addRTLPath("/opt/hoplites/rtl/pe.v")
  addRTLPath("/opt/hoplites/rtl/pewrap.v")
  addRTLPath("/opt/hoplites/rtl/switch.v")
  addRTLPath("/opt/hoplites/rtl/torus.v")
}
