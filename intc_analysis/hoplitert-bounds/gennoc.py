#!/usr/bin/env pypy

import sys
import os
import commands as CMD
from random import randint, uniform


import argparse
from Datatypes import *

class NoC_GenT(object):
  def __init__(self, w, h, max_rate, max_burst, max_flows, trType=TTypes.Random):
    self.W = w
    self.H = h
    self.max_rate = max_rate
    self.Defult_min_rate = 1.0 / (h * w)
    self.Defult_max_rate = self.Defult_min_rate * 2
    self.max_burst = max_burst
    self.max_flows = max_flows

    self.Nodes = [[None for x in range(w)] for y in range(h)]  #type: list[list[NodeT]]
    if trType == TTypes.Random:
      self._Set_Random_Traffic()
    elif trType == TTypes.All2One:
      self._Set_All2One_Traffic()

  def _random_rate(self):
    retV =   round(uniform(self.Defult_min_rate, self.Defult_max_rate), 3)
    return retV
  def _Set_Random_Traffic(self):
    w = self.W
    h = self.H
    for y in range(h):
      for x in range(w):
        NodeV = NodeT(x,y, [])
        self.Nodes[y][x] = NodeV
        flowsNO = randint(1, self.max_flows)
        for i in range(flowsNO):
          if self.max_rate > 0:
            rateV = self.max_rate
          else:
            rateV = self._random_rate()

          burstV = randint(1, self.max_burst)
          xpos = x
          ypos = y
          while xpos == x and ypos == y:
            dx = randint(0, w)
            dy = randint(0, h)
            xpos = (x + dx) % w
            ypos = (y + dy) % h
            
          FlowV = FlowT(xpos, ypos, rateV, burstV)
          NodeV.Flows.append(FlowV)
          pass


    return self

  def _Set_All2One_Traffic(self):
    w = self.W
    h = self.H
    for y in range(h):
      for x in range(w):
        NodeV = NodeT(x,y, [])
        self.Nodes[y][x] = NodeV
        flowsNO = randint(1, self.max_flows)
        for i in range(flowsNO):
          if self.max_rate > 0:
            rateV = self.max_rate
          else:
            rateV = self._random_rate()

          burstV = randint(1, self.max_burst)

          xpos = w-1
          ypos = h-1
          if (x,y) == (w-1,h-1):
            (xpos,ypos) = (0,0)
            burstV = 1
            rateV = 0.0

          FlowV = FlowT(xpos, ypos, rateV, burstV)
          NodeV.Flows.append(FlowV)
          pass


    return self
  
  def _Set_All2Row_Traffic(self):
    w = self.W
    h = self.H
    for y in range(h):
      for x in range(w):
        NodeV = NodeT(x,y, [])
        self.Nodes[y][x] = NodeV
        flowsNO = randint(1, self.max_flows)
        for i in range(flowsNO):
          if self.max_rate > 0:
            rateV = self.max_rate
          else:
            rateV = self._random_rate()

          burstV = randint(1, self.max_burst)

          xpos = w-1
          ypos = h-1
          if (x,y) == (w-1,h-1):
            (xpos,ypos) = (0,0)
            burstV = 0

          FlowV = FlowT(xpos, ypos, rateV, burstV)
          NodeV.Flows.append(FlowV)
          pass


    return self





def writeBuffer(buff, filepath):
  f = open(filepath, 'wb')
  f.write(buff)
  f.close()

def readBuffer(filepath):
  f = open(filepath, 'rb')
  buff = f.read()
  f.close()
  return buff

def py2verilog(NoCP, dump_path):
  #type:(NoCT)->str
  h = NoCP.H
  w = NoCP.W
  X_str = ''
  Y_str = ''
  r_str = ''
  p_str = ''
  b_str = ''
  for y in range(h):
    for x in range(w):
      RecV = NoCP.DestData[y][x] #type:DestRecT
      X_str += '%x\n'%RecV.x
      Y_str += '%x\n'%RecV.y
      r = (RecV.rate*100)
      r_str += '%x\n'%r
      p_str += '%x\n'%RecV.period
      b_str += '%x\n'%RecV.burst
  
  if not os.path.exists(dump_path):
    CMD.getoutput('mkdir -p %s'%dump_path)
  writeBuffer(X_str, dump_path + 'x.dat')
  writeBuffer(Y_str, dump_path + 'y.dat')
  writeBuffer(r_str, dump_path + 'rates.dat')
  writeBuffer(p_str, dump_path + 'Periods.dat')
  writeBuffer(b_str, dump_path + 'bursts.dat')

def NoC_to_Flows_mapFile(NoCP, filepath):
  #type:(NoC_GenT, str)->None
  w = NoCP.W
  h = NoCP.H
  rows = ''
  for y in range(h):
    for x in range(w):
      NodeV = NoCP.Nodes[y][x]
      for fl in NodeV.Flows:
        if fl.burst == 0:
          fl.rate = 0.0
          fl.burst = 1
        rows += '%d, %d, %d, %d, %d, %.6f\n'%(x,y, fl.dx,fl.dy, fl.burst, fl.rate)
        # rate_percent = int(fl.rate * 100.0)
        # row = '%x, %x, %x, %x, %x, %x\n'%(x,y, fl.dx,fl.dy, fl.burst, rate_percent)
  writeBuffer(rows, filepath)


def main(w, h ,outfile, presetRate, max_burst, max_flows, trfType):
  print w, h, presetRate, trfType, '-o' + outfile
  NoC1 = NoC_GenT(w,h, presetRate, max_burst=max_burst, max_flows=max_flows, trType=trfType)
  NoC_to_Flows_mapFile(NoC1, outfile)
  pass


  
if __name__ == '__main__':
  parser = argparse.ArgumentParser(description='NoC Traffic Generator')
  parser.add_argument('W', metavar='Width', type=int, help='the hight of the NoC')
  parser.add_argument('H', metavar='Hight', type=int, help='the width of the NoC')
  parser.add_argument('-o', metavar='output', type=str, default='./map.dat', help='the map file')
  parser.add_argument('-p', metavar='Rate', type=float, default=-1, help='overide the rate of all the flows, default = random (1/m^2, 0.5)')
  parser.add_argument('-t', metavar='TrafficType',  help='set the traffice type => {random, all2one, all2row, all2column}: default = random', default='random')
  parser.add_argument('-f', metavar='flows', type=int, default=1, help='the maximum number of different flows per node')
  parser.add_argument('-b', metavar='bursts', type=int, default=1, help='the maximum number of burstiness per flow')
  args = parser.parse_args()
  # print args
  # sys.stderr = Sink()
  if args.p:
    pass

  main(args.W, args.H, args.o, args.p, args.b, args.f, args.t)
    