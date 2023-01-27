package vexriscv.demo.smp

import spinal.lib._
import spinal.core._

// due to c++ padding, this struct will be 256 bit in HLS
case class DRAMRequest() extends Bundle {
  val cacheline = UInt(16 * 8 bits)
  val addr = UInt(32 bits)

  def is_read(): Bool = {
    (this.addr & 1) === 0
  }

  def is_write(): Bool = {
    (this.addr & 1) =/= 0
  }
}

case class DRAMResponse() extends Bundle {
  val cacheline = UInt(16 * 8 bits)
  val addr = UInt(32 bits)
}
