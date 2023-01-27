package vexriscv.demo.smp

import spinal.lib._
import spinal.core._

case class VReg[T <: Data](entry: HardType[T]) extends Area {
  val valid = Reg(Bool()) init false
  val value = Reg(entry())

  def write(v: T): Unit = {
    value := v
    valid := True
  }

  def invalidate(): Unit = {
    valid := False
  }
}
