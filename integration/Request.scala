package vexriscv.demo.smp

import spinal.core._
import spinal.lib._
import spinal.lib.sim._
import spinal.core.sim._
import spinal.sim._

import scala.language.postfixOps

object RequestDitty extends Enumeration {
  val LD, ST, AMO, GetM, GetS, PutM, PutS, Data, Invalidate, Ack, PutAck, FwdAck, RespOk = Value
}

case class Request() extends Bundle {
  // we want to make sure this is the same as the original design
  val data = UInt(16 * 8 bits)
  val addr = UInt(32 bits)
  val wb_addr = UInt(32 bits)
  val respond_to_src_network_id = UInt(32 bits)
  // the following field will be stripped
  val dst = UInt(32 bits)

  def :=(that: RequestHopliteRT): Unit = {
    this.data := that.data
    this.addr := that.addr
    this.wb_addr := that.wb_addr
    this.respond_to_src_network_id := that.respond_to_src_network_id
    // the receiver will not care about the dst
    this.dst := 0
  }

  def is_combined(): Bool = (this.wb_addr & 0xf) =/= 0

  def is_put_ack(): Bool = (this.addr & 0xf) === 10

  def set_src(src: UInt): Unit = {
    this.respond_to_src_network_id(7 downto 2) := src
  }
  def get_src(): UInt= this.respond_to_src_network_id(7 downto 2)
  def set_network_id(network_id: UInt): Unit = {
    this.respond_to_src_network_id(7 downto 2) := network_id
  }
  // load request from core
  def simAsLoadRequest(addr: Int) = {
    this.addr #= addr
  }
}
case class RequestHopliteRT(intc_conf: HopliteRTConfig) extends Bundle with HasHopliteDestination {
  val data = UInt(16 * 8 bits)
  val addr = UInt(32 bits)
  val wb_addr = UInt(32 bits)
  val respond_to_src_network_id = UInt(32 bits)

  // destination section
  val y = UInt((log2Up(intc_conf.Y_DIM) max 1) bits)
  val x = UInt((log2Up(intc_conf.X_DIM) max 1) bits)

  def is_combined(): Bool = (this.wb_addr & 0xf) =/= 0
  // PutAck is 10 according to the original example
  def is_put_ack(): Bool = (this.addr & 0xf) === 10
  def set_src(src: UInt): Unit = {
    this.respond_to_src_network_id(7 downto 2) := src
  }
  def get_src(): UInt= this.respond_to_src_network_id(7 downto 2)
  def set_network_id(network_id: UInt): Unit = {
    this.respond_to_src_network_id(7 downto 2) := network_id
  }
  def :=(that: Request): Unit = {
    this.data := that.data
    this.addr := that.addr
    this.wb_addr := that.wb_addr
    this.respond_to_src_network_id := that.respond_to_src_network_id
  }
}
