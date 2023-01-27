package vexriscv.demo.smp

import spinal.core._
import spinal.lib._

case class Response() extends Bundle {
  // the whole thing is 256-bit
  val cacheline = UInt(16 * 8 bits)
  val addr = UInt(32 bits)
  val latency = UInt(32 bits)
  val wallclock = UInt(64 bits)
}
