#!/usr/bin/env pypy

import sys
import os
import commands as CMD
from math import ceil
import argparse
from Datatypes import *


def max_arrival(r, b, t, StabilityTest=False):
  if StabilityTest:
    # retV = min(t, (1 + int(r * (t - 1))) * (int(r > 0)))
    retV = min(t, (0 + int(r * (t))) * (int(r > 0)))
    # if b==0:
    #   return 0
  else:
    retV = min(t, (b + int(r * (t - 1))) * (int(b > 0 and r>0)))
  return retV


def writeBuffer(buff, filepath):
  f = open(filepath, 'wb')
  f.write(buff)
  f.close()

def readBuffer(filepath):
  f = open(filepath, 'rb')
  buff = f.read()
  f.close()
  return buff

def get_size(matfileLines):
  lines = matfileLines #type: list[str]
  max_x = max_y = 0
  for l in lines:
    x = int(l.split(',')[0])
    y = int(l.split(',')[1])
    max_x = x if x > max_x else max_x
    max_y = y if y > max_y else max_y
  return max_x+1, max_y+1

class TI:
  stbl = 0
  pktsNO = 1
  pktsNOBursty = 2
  burstNO = 3
  totRate = 4

class NoCT(object):
  def __init__(self, w, h):
    self.W = w
    self.H = h
    self.max_convergentLimit = 2**32
    self.Nodes = [[NodeT(x, y, []) for x in range(w)] for y in range(h)]  #type: list[list[NodeT]]
    self.All_stable = False
    self.Worst_WaitingTime_E = 0
    self.Worst_WaitingTime_S = 0
    self.Total_Worst_WaitingTime_E = 0
    self.Total_Worst_WaitingTime_S = 0
    
    self.largest_period_S = 0
    self.largest_period_E = 0
    self.Node_largest_period_S = None #type: NodeT
    self.Node_largest_period_E = None #type: NodeT
    

  def DX(self, source, dest):
    m = self.W
    dx = (dest - source + m)%m
    return dx

  def DY(self, source, dest):
    m = self.H
    dy = (dest - source + m)%m
    return dy

  def N2S(self, x, y, t):
    w = self.W
    h = self.H
    total_packets = 0
    total_packets_Bursty = 0
    total_bursts = 0
    total_rate = 0.0
    for sy in range(h):
      if sy == y: continue  # skip the same row, only consider other rows
      for sx in range(w):
        NodeV = self.Nodes[sy][sx]
        assert NodeV.sx == sx and NodeV.sy == sy
        for FlowV in NodeV.Flows:
          if FlowV.dx == x and self.DY(sy, FlowV.dy) >= self.DY(sy, y):
            b = FlowV.burst if FlowV.rate > 0.0 else 0
            r = FlowV.rate if b > 0 else 0.0
            total_rate += r
            pktsNO = max_arrival(r, b, t, StabilityTest=True)
            pktsNO_bursty = max_arrival(r, b, t, StabilityTest=False)
            total_packets += pktsNO
            total_packets_Bursty += pktsNO_bursty
            total_bursts += FlowV.burst

    return total_packets, total_packets_Bursty, total_bursts, total_rate

  def W2E(self, x, y, t):
    w = self.W
    h = self.H
    total_packets = 0
    total_packets_Bursty = 0
    total_bursts = 0
    total_rate = 0.0
    for sy in range(h):
      if sy != y: continue  # only consider the same row
      for sx in range(w):
        NodeV = self.Nodes[sy][sx]
        assert NodeV.sx == sx and NodeV.sy == sy
        for FlowV in NodeV.Flows:
          if sx != x and self.DX(sx, FlowV.dx) > self.DX(sx, x):
            b = FlowV.burst if FlowV.rate > 0.0 else 0
            r = FlowV.rate if b > 0 else 0.0
            total_rate += r
            pktsNO = max_arrival(r, b, t, StabilityTest=True)
            pktsNO_bursty = max_arrival(r, b, t, StabilityTest=False)
            total_packets += pktsNO
            total_packets_Bursty += pktsNO_bursty
            total_bursts += FlowV.burst

    return total_packets, total_packets_Bursty, total_bursts, total_rate

  def W2S(self, x, y, t):
    w = self.W
    h = self.H
    total_packets = 0
    total_packets_Bursty = 0
    total_bursts = 0
    total_rate = 0.0
    for sy in range(h):
      if sy != y: continue  # only consider the same row
      for sx in range(w):
        NodeV = self.Nodes[sy][sx]
        assert NodeV.sx == sx and NodeV.sy == sy
        for FlowV in NodeV.Flows:
          if sx != x and FlowV.dx == x:
            b = FlowV.burst if FlowV.rate > 0.0 else 0
            r = FlowV.rate if b > 0 else 0.0
            total_rate += r
            pktsNO = max_arrival(r, b, t, StabilityTest=True)
            pktsNO_bursty = max_arrival(r, b, t, StabilityTest=False)
            total_packets += pktsNO
            total_packets_Bursty += pktsNO_bursty
            total_bursts += FlowV.burst

    return total_packets, total_packets_Bursty, total_bursts, total_rate

  def dtot(self, y, t):
    w = self.W
    h = self.H
    total_packets = 0
    total_packets_bursty = 0
    total_bursts = 0
    total_rate = 0.0
    for sy in range(h):
      if sy != y: continue  # only consider the same row
      for sx in range(w):
        pktsNO_W2S, pktsNO_Bursty_W2S, burstsNO_W2S, total_rate_W2S = self.W2S(sx, sy, 0) #just to test if there is a conflicting flow
        if burstsNO_W2S > 0:
          pktsNO, pktsNO_Bursty, burstsNO, rate = self.N2S(sx, y, t)
          total_packets += pktsNO
          total_packets_bursty += pktsNO_Bursty
          total_bursts += burstsNO
          total_rate += rate

    return total_packets, total_packets_bursty, total_bursts, total_rate

  def W_tot(self, x, y, t):
    pktsW2E, pktsW2E_Bursty, burstW2E, total_rate_W2E = self.W2E(x, y, t)
    pktsW2S, pktsW2S_Bursty, burstW2S, total_rateW2S = self.W2S(x, y, t)
    pktsDefTot, pktsDefTot_Bursty, burstDefTot, total_rateDefTot = self.dtot(y, t)
    
    pktsNO = pktsW2E + pktsW2S + pktsDefTot
    pktsNO_Bursty = pktsW2E_Bursty + pktsW2S_Bursty + pktsDefTot_Bursty
    totalburst = burstW2E + burstW2S + burstDefTot
    total_rate = total_rate_W2E + total_rateW2S + total_rateDefTot
    
    return pktsNO, pktsNO_Bursty, totalburst, total_rate

  def max_burst_E(self, NodeP):
    #type:(NodeT)->int
    max_b = 0
    for fl in NodeP.Flows:
      if self.DX(NodeP.sx, fl.dx) > 0:
        if max_b < fl.burst:
          max_b = fl.burst
    return max_b

  def max_burst_S(self, NodeP):
    #type:(NodeT)->int
    max_b = 0
    for fl in NodeP.Flows:
      if self.DY(NodeP.sy, fl.dy) >0 and fl.dx == NodeP.sx:
        if max_b < fl.burst:
          max_b = fl.burst
    return max_b


  def max_rate_E(self, NodeP):
    #type:(NodeT)->float
    max_r = 0.0
    for fl in NodeP.Flows:
      if self.DX(NodeP.sx, fl.dx) > 0:
        if max_r < fl.rate:
          max_r = fl.rate
    return max_r

  def max_period_E(self, NodeP):
    #type:(NodeT)->int
    maxV = 0
    for fl in NodeP.Flows:
      if self.DX(NodeP.sx, fl.dx) > 0:
        if maxV < fl.period:
          maxV = fl.period
    return maxV

  def get_larget_periods_stat(self):
    largest_period_S = 0
    largest_period_E = 0
    Node_largest_period_S = None #type: NodeT
    Node_largest_period_E = None #type: NodeT

    for y in range(self.H):
      for x in range(self.W):
        NodeV = self.Nodes[y][x]
        PL_E = self.max_period_E(NodeV)
        PL_S = self.max_period_S(NodeV)
        if PL_E > largest_period_E:
          largest_period_E = PL_E
          Node_largest_period_E = NodeV

        if PL_S > largest_period_S:
          largest_period_S = PL_S
          Node_largest_period_S = NodeV
          
    return largest_period_E, Node_largest_period_E, largest_period_S, Node_largest_period_S


  def max_rate_S(self, NodeP):
    #type:(NodeT)->float
    max_r = 0
    for fl in NodeP.Flows:
      if self.DY(NodeP.sy, fl.dy) > 0 and fl.dx == NodeP.sx:
        if max_r < fl.rate:
          max_r = fl.rate
    return max_r

  def max_period_S(self, NodeP):
    #type:(NodeT)->int
    maxV = 0
    for fl in NodeP.Flows:
      if self.DY(NodeP.sy, fl.dy) > 0 and fl.dx == NodeP.sx:
        if maxV < fl.period:
          maxV = fl.period
    return maxV

  def max_waitingTime_E(self):
    
    max_Ts = 0
    max_Ts_tot = 0
    for y in range(self.H):
      for x in range(self.W):
        nd = self.Nodes[y][x]
        if max_Ts < nd.Max_Waiting_E:
          max_Ts = nd.Max_Waiting_E
          max_Ts_tot = nd.Max_TotalWaiting_E
    return max_Ts, max_Ts_tot

  def max_inflight_Latency(self):
    max_Tf = 0
    max_Tf_opt = 0
    for y in range(self.H):
      for x in range(self.W):
        nd = self.Nodes[y][x]
        for fl in nd.Flows:
          if max_Tf < fl.InFlight_latency:
            max_Tf = fl.InFlight_latency
          
          if max_Tf_opt < fl.InFlight_latency_opt:
            max_Tf_opt = fl.InFlight_latency_opt
    
    
    return max_Tf, max_Tf_opt

  def avg_inflight_Latency(self):
    sum_Tf = 0
    sum_Tf_opt = 0
    N = self.H * self.W
    for y in range(self.H):
      for x in range(self.W):
        nd = self.Nodes[y][x]
        for fl in nd.Flows:
          sum_Tf     += fl.InFlight_latency
          sum_Tf_opt += fl.InFlight_latency_opt
    avg_Tf = int(float(sum_Tf)/N)
    avg_Tf_opt = int(float(sum_Tf_opt)/N)
    
    return avg_Tf, avg_Tf_opt

  def max_waitingTime_S(self):
    max_Ts = 0
    max_Ts_tot = 0
    for y in range(self.H):
      for x in range(self.W):
        nd = self.Nodes[y][x]
        if max_Ts < nd.Max_Waiting_S:
          max_Ts = nd.Max_Waiting_S
          max_Ts_tot = nd.Max_TotalWaiting_S
    return max_Ts, max_Ts_tot

  def sending_E(self, NodeP):
    #type:(NodeT)->bool
    for fl in NodeP.Flows:
      if self.DX(NodeP.sx, fl.dx) > 0:
        return True
    return False

  def sending_S(self, NodeP):
    #type:(NodeT)->bool
    for fl in NodeP.Flows:
      if self.DY(NodeP.sy, fl.dy) > 0 and fl.dx == NodeP.sx:
        return True
    return False

  def is_stable_E(self, x, y, t):
    pktNo, pktNo_Bursty, burstsNO, total_rate = self.W_tot(x, y, t)
    NodeV = self.Nodes[y][x]
    totPkts_fixed1 = max_arrival(total_rate, 0, t, StabilityTest=True)
    # totPkts_fixed = round(total_rate * t, 3)
    totPkts_fixed = ceil(total_rate * t)
#    assert totPkts_fixed1 == int(totPkts_fixed)
    
    # stable = True if  self.max_burst_E(NodeV) < 1 or self.max_rate_E(NodeV) <= 0  else False  # check if you are not sending at all
    # if not stable:  # check if you are not sending East
    #   stable = True if not self.sending_E(NodeV)  else False
    # if not stable:
    #   stable = bool(totPkts_fixed < t)
    #   if t > 0:
    #     stable2 = total_rate + self.max_rate_E(NodeV) <= 1.0
    #     #FIXME: make sure which formula is more correct
    #     assert stable == stable2

    stable = total_rate + self.max_rate_E(NodeV) <= 1.0

    return stable, totPkts_fixed, pktNo_Bursty, burstsNO, total_rate
  
  def is_stable_S(self, x, y, t):
    pktNO_W2S, pktNO_W2S_Bursty, burstsNO_W2S, total_rateW2S = self.W2S(x, y, t)
    pktNO_N2S, pktNO_N2S_Bursty, burstsNO_N2S, total_rateN2S = self.N2S(x, y, t)

    pktNO_tot = pktNO_W2S + pktNO_N2S
    pktNO_tot_Bursty = pktNO_W2S_Bursty + pktNO_N2S_Bursty
    burst_tot = burstsNO_W2S + burstsNO_N2S
    total_rate = total_rateW2S + total_rateN2S

    # totPkts_fixed = max_arrival(total_rate, 1, t, StabilityTest=True)
    # totPkts_fixed = round(total_rate * t, 3)
    totPkts_fixed = ceil(total_rate * t)


    NodeV = self.Nodes[y][x]
    # stable = True if self.max_burst_S(NodeV) < 1 or self.max_rate_S(NodeV) <= 0  else False  # check if you are not sending at all
    # if not stable:  # check if you are not sending South
    #   stable = True if not self.sending_S(NodeV) else False
    # if not stable:
    #   # stable = bool(pktNO_tot < t)
    #   stable = bool(totPkts_fixed < t)
    #   if t > 0:
    #     stable2 = total_rate + self.max_rate_S(NodeV) <= 1.0
    #     assert stable == stable2
    stable = total_rate + self.max_rate_S(NodeV) <= 1.0
      
    return stable, totPkts_fixed, pktNO_tot_Bursty, burst_tot, total_rate

  def inflight_latency(self, sx, sy, dx, dy):
    lat = self.DX(sx, dx) + self.DY(sy, dy) + (self.DY(sy, dy)*self.W) + 2
    return lat

  def inflight_latency_opt(self, sx, sy, dx, dy):
    h = self.H
    j = (sy + 1)%h
    NORows = 0
    orig_NORows = self.DY(sy, dy)
    for i in range(orig_NORows):
      ConflictFlag = False
      for NodeV in self.Nodes[j]:
        for fl in NodeV.Flows:
          if fl.dx == dx and NodeV.sx != dx:
            ConflictFlag = True
            break
        if ConflictFlag:
          break

      j = (j+1)%h
      if ConflictFlag:
        NORows += 1


    lat = self.DX(sx, dx) + self.DY(sy, dy) + (NORows*self.W) + 2
    return lat
  
  def WaitingTime_form1_sum(self, x, y):
    NodeV = self.Nodes[y][x]
    max_rate_E = self.max_rate_E(NodeV)
    stable_E, pktsNO_E, pktsNO_Bursty_E, burstsNO_E, total_rate_E = self.is_stable_E(x, y, 0)
    Ts_E = 0
    Ts_E_pre = -1
    while Ts_E != Ts_E_pre:
      Ts_E_pre = Ts_E
      Ts_E = burstsNO_E + int(max_rate_E * Ts_E_pre)
      if max_rate_E >= 1 and burstsNO_E > 0:
        Ts_E = -1
        break
    
    max_rate_S = self.max_rate_S(NodeV)
    stable_S, pktsNO_S, pktsNO_Bursty_S, burstsNO_S, total_rate_S = self.is_stable_S(x, y, 1)
    Ts_S = 0
    Ts_S_pre = -1
    while Ts_S != Ts_S_pre:
      Ts_S_pre = Ts_S
      Ts_S = burstsNO_E + int(max_rate_S * Ts_S_pre)
      if max_rate_S >= 1 and burstsNO_S:
        Ts_S = -1
        break

    return Ts_E, Ts_S

  def WaitingTime_form2_TotalTraffic(self, x, y, stable_E, stable_S):
    NodeV = self.Nodes[y][x]    
    burstsNO_E = self.is_stable_E(x, y, 0)[TI.burstNO]
    if stable_E:
      Ts_E = burstsNO_E
      Ts_E_pre = 0
      total_rate_E = 0.0
      while Ts_E != Ts_E_pre:
        if Ts_E >= self.max_convergentLimit or total_rate_E >= 1.0:
          Ts_E = -1
          break
        Ts_E_pre = Ts_E
        stable_E, pktsNO_E, pktsNO_Bursty_E, burstsNO_E, total_rate_E = self.is_stable_E(x, y, 0)
        Ts_E = burstsNO_E + int(total_rate_E*Ts_E)
        # Ts_E = self.is_stable_E(x, y, Ts_E_pre)[TI.pktsNOBursty]
    else:
      Ts_E = -1
    

    burstsNO_S = self.is_stable_S(x, y, 0)[TI.burstNO]
    if stable_S:
      Ts_S = burstsNO_S
      Ts_S_pre = 0
      total_rate_S = 0.0
      while Ts_S != Ts_S_pre:
        if Ts_S >= self.max_convergentLimit or total_rate_S >= 1.0:
          Ts_S = -1
          break

        Ts_S_pre = Ts_S
        stable_S, pktsNO_S, pktsNO_Bursty_S, burstsNO_S, total_rate_S = self.is_stable_S(x, y, 0)
        Ts_S = burstsNO_S + int(total_rate_S * Ts_S)
        # Ts_S = self.is_stable_S(x, y, Ts_S_pre)[TI.pktsNOBursty]
    else:
      Ts_S = -1

    return Ts_E, Ts_S
  
  def WaitingTime_form3_totalrates(self, x, y, stable_E, stable_S):
    NodeV = self.Nodes[y][x]
    Ts_E = -1
    Ts_S = -1
    Ts_Total_E = -1
    Ts_Total_S = -1

    _, pktsNO_E, pktsNO_Bursty_E, burstsNO_E, total_rate_E = self.is_stable_E(x, y, 0)
    # if y == 9 and x == 2:
    #   _, pktsNO_E, pktsNO_Bursty_E, burstsNO_E, total_rate_E = self.is_stable_E(x, y, burstsNO_E)
    if total_rate_E < 1 and stable_E:
      Ts_E = burstsNO_E/(1-total_rate_E)
      Ts_E = int((Ts_E))
      Ts_Total_E = Ts_E + (self.max_period_E(NodeV) -1)
    

    _, pktsNO_S, pktsNO_Bursty_S, burstsNO_S, total_rate_S = self.is_stable_S(x, y, 0)
    if total_rate_S < 1 and stable_S:
      Ts_S = burstsNO_E/(1-total_rate_S)
      Ts_S = int((Ts_S))
      Ts_Total_S = Ts_S + (self.max_period_S(NodeV) - 1)


    return Ts_E, Ts_S, Ts_Total_E, Ts_Total_S


    
    
  
  def calc_bounds(self, silent_mode):
    retV = True

    for y in range(self.H):
      for x in range(self.W):
        NodeV = self.Nodes[y][x]
        max_rate_E = self.max_rate_E(NodeV)
        max_rate_S = self.max_rate_S(NodeV)
        max_burst_E = self.max_burst_E(NodeV)
        max_burst_S = self.max_burst_S(NodeV)
        min_period_E = int(round(1.0/max_rate_E)) if max_rate_E > 0.0 else 0
        min_period_S = int(round(1.0/max_rate_S)) if max_rate_S > 0.0 else 0
        stable_E, pktsNO_E, pktsNO_Bursty_E, burstsNO_E, total_rate_E = self.is_stable_E(x, y, min_period_E)
        stable_S, pktsNO_S, pktsNO_Bursty_S, burstsNO_S, total_rate_S = self.is_stable_S(x, y, min_period_S)
        
        # TsE, TsS = self.WaitingTime_form1_sum(x, y)
        # TsE, TsS = self.WaitingTime_form2_TotalTraffic(x, y, stable_E, stable_S)
        TsE, TsS, Ts_Total_E, Ts_Total_S = self.WaitingTime_form3_totalrates(x, y, stable_E, stable_S)
        
        NodeV.Max_Waiting_E = TsE if max_burst_E > 0 else 0
        NodeV.Max_Waiting_S = TsS if max_burst_S > 0 else 0
        NodeV.Max_TotalWaiting_E = Ts_Total_E if max_burst_E > 0 else 0
        NodeV.Max_TotalWaiting_S = Ts_Total_S if max_burst_S > 0 else 0
        
        NodeV.Sustained_Waiting_E = pktsNO_E  if max_burst_E > 0 else 0
        NodeV.Sustained_Waiting_S = pktsNO_S  if max_burst_S > 0 else 0

        for fl in NodeV.Flows:
          fl.InFlight_latency = self.inflight_latency(x,y, fl.dx, fl.dy)
          fl.InFlight_latency_opt = self.inflight_latency_opt(x,y, fl.dx, fl.dy)
          flow_stable_E, flow_pktsNO_E, flow_pktsNO_Bursty_E, flow_burstsNO_E, flow_total_rate_E = self.is_stable_E(x, y, fl.period)
          flow_stable_S, flow_pktsNO_S, flow_pktsNO_Bursty_S, flow_burstsNO_S, flow_total_rate_S = self.is_stable_S(x, y, fl.period)
          fl.stable = flow_stable_E and flow_stable_S

        if not stable_E:
          if not silent_mode:
            print 'Node(%d,%d) E-Port is not sustainable'%(x,y)
            # print '\tTotal incomming pkts on W-port is %d in %d cycles' % (pktsNO_E, min_period_E)
            print '\tTotal rate going East from the W-port is %f' % total_rate_E
          retV = False

        if not stable_S:
          if not silent_mode:
            print 'Node(%d,%d) S-Port is not sustainable'%(x,y)
            # print '\tTotal incomming pkts on W-port and N-port is %d in %d cycles' % (pktsNO_S, min_period_S)
            print '\tTotal rate going South from the W-port + N-Port is %f' % total_rate_S
          retV = False

    self.All_stable = retV
    if retV:
      self.Worst_WaitingTime_E, self.Total_Worst_WaitingTime_E = self.max_waitingTime_E()
      self.Worst_WaitingTime_S, self.Total_Worst_WaitingTime_S = self.max_waitingTime_S()
      self.largest_period_E, self.Node_largest_period_E, self.largest_period_S, self.Node_largest_period_S = self.get_larget_periods_stat()
    return retV

  
  def dump_results(self, filepath):
    rows = ''
    for y in range(self.H):
      for x in range(self.W):
        NodeV = self.Nodes[y][x]
        for fl in NodeV.Flows:
          rows += '%d, %d, %d, %d, %d, %.6f, =>, ' % (x, y, fl.dx, fl.dy, fl.burst_orig, fl.rate)
          rows += '%d, %d, %d, %d, %s, %d, %d, '%(fl.InFlight_latency, fl.InFlight_latency_opt, NodeV.Max_Waiting_S, NodeV.Max_Waiting_E, fl.stable, NodeV.Max_TotalWaiting_S, NodeV.Max_TotalWaiting_E)
          rows += 'Prd=%d, %.3f, %.3f\n'%(fl.period, NodeV.Sustained_Waiting_S, NodeV.Sustained_Waiting_E)
          # rate_percent = int(fl.rate * 100.0)
          # row = '%x, %x, %x, %x, %x, %x\n'%(x,y, fl.dx,fl.dy, fl.burst, rate_percent)
    writeBuffer(rows, filepath)


def parse_mapfile(filepath):
  buf = readBuffer(filepath)
  lines = buf.splitlines()
  w,h = get_size(lines)
  NoCV = NoCT(w, h)

  for l in lines:
    values = l.split(',')
    sx = int(values[0])
    sy = int(values[1])
    dx = int(values[2])
    dy = int(values[3])
    burst = int(values[4])
    rate = float(values[5])
    # burst = burst if rate > 0.0 else 0
    FlowV = FlowT(dx, dy, rate, burst)
    NoCV.Nodes[sy][sx].Flows.append(FlowV)

  return NoCV


def main(mapFile, resultsFile, silent_mode):
    NoC1 = parse_mapfile(mapFile)
    stable = NoC1.calc_bounds(silent_mode)
    LP_ND_E = NoC1.Node_largest_period_E #type: NodeT
    LP_ND_S = NoC1.Node_largest_period_S #type: NodeT
    LP_prd_E = NoC1.largest_period_E
    LP_prd_S = NoC1.largest_period_S
    LP_totW_E = LP_prd_E - 1 if LP_prd_E >0 else 0
    LP_totW_S = LP_prd_S - 1 if LP_prd_S >0 else 0

    absWorst_E =  NoC1.Worst_WaitingTime_E + LP_prd_E - 1 if LP_prd_E >0 else 0
    absWorst_S =  NoC1.Worst_WaitingTime_S + LP_prd_S - 1 if LP_prd_S >0 else 0
    if not silent_mode:
      if stable:
        print('network is Stable (True)\nWorst Waiting Times')
        print('\tNetwork Waiting Times only:\n\t\tEast:%d\n\t\tSouth:%d' % (NoC1.Worst_WaitingTime_E, NoC1.Worst_WaitingTime_S))
        print('\tTotal (network + bucket) (of the above flows):\n\t\tEast:%d\n\t\tSouth:%d' % (NoC1.Total_Worst_WaitingTime_E, NoC1.Total_Worst_WaitingTime_S))
        print('\tTotal worst Client waitings for the bucket only :\n\t\tEast:%d\n\t\tSouth:%d' % (LP_totW_E, LP_totW_S))
        print('\tThe Total abslute worst Waiting:\n\t\tEast:%d\n\t\tSouth:%d' % (absWorst_E, absWorst_S))
      else:
        print('network is Not Stable (False)')
    NoC1.dump_results(resultsFile)
    print int(stable)





if __name__ == '__main__':
  parser = argparse.ArgumentParser(description='NoC Analysis Tool')
  parser.add_argument('m', metavar='MapFile', type=str, help='the input map file')
  parser.add_argument('-o', metavar='output', type=str, default='./results.dat', help='the results file')
  parser.add_argument('-s',  action='store_true', help='silent mode, not output on stdout except the final (0/1) result')
  args = parser.parse_args()
  # print args.s
  main(args.m, args.o, args.s)
