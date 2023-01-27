#!/usr/bin/env python
# -*- coding: utf-8 -*-
__author__ = 'Saud Wasly'
import sys
import os
import commands as CMD
from math import ceil

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

class NodeT(object):
  def __init__(self, sx ,sy, flowsP=[]):
    self.sx = -1
    self.sy = -1
    self.Flows = flowsP #type: list[FlowT]
    self.Max_Waiting_E = 0
    self.Max_Waiting_S = 0
    self.Sustained_Waiting_E = 0
    self.Sustained_Waiting_S = 0
    self.ResponseTime = 0

class FlowT(object):
  def __init__(self,dx,dy, rate, burst):
    self.dx = dx
    self.dy = dy
    self.rate = rate
    self.burst = burst

    self.period = int((1.0 / rate)) if rate != 0 else 1
    # self.period = int(round(1.0/rate)) if rate!=0 else 1
    # self.period = int(ceil(1.0/rate)) if rate!=0 else 1

    self.stable = False
    self.fl_Waiting_E = 0
    self.fl_Waiting_S = 0

    self.InFlight_latency = 0
    self.InFlight_latency_opt = 0 #the optimized bound based on the flows
  def __str__(self):
    S = '\tdest X = %d\n' %self.dx
    S += '\tdest Y = %d\n' %self.dy
    S += '\trate = %.3f\n' %self.rate
    S += '\tperiod = %d\n' %self.rate
    S += '\tburst = %d\n' %self.burst
    return S


class NoCT(object):
  def __init__(self, w, h):
    self.W = w
    self.H = h
    self.max_convergentLimit = 2 ** 32
    self.Nodes = [[NodeT(x, y, []) for x in range(w)] for y in range(h)]  # type: list[list[NodeT]]
    
  def dump_results(self, filepath):
    rows = ''
    for y in range(self.H):
      for x in range(self.W):
        NodeV = self.Nodes[y][x]
        for fl in NodeV.Flows:
          rows += '%d, %d, %d, %d, %d, %.6f, =>, ' % (x, y, fl.dx, fl.dy, fl.burst, fl.rate)
          rows += '%d, %d, %d, %d, %s, ' % (
          fl.InFlight_latency, fl.InFlight_latency_opt, NodeV.Max_Waiting_S, NodeV.Max_Waiting_E, fl.stable)
          rows += 'Prd=%d, %.3f, %.3f\n' % (fl.period, NodeV.Sustained_Waiting_S, NodeV.Sustained_Waiting_E)
          # rate_percent = int(fl.rate * 100.0)
          # row = '%x, %x, %x, %x, %x, %x\n'%(x,y, fl.dx,fl.dy, fl.burst, rate_percent)
    writeBuffer(rows, filepath)

  def dump_NoC_verilog(self, filepath):
    rows = ''
    for y in range(self.H):
      for x in range(self.W):
        NodeV = self.Nodes[y][x]
        for fl in NodeV.Flows:
          rate_percent = int(fl.rate * 100.0)
          row = '%x, %x, %x, %x, %x, %x\n'%(x,y, fl.dx,fl.dy, fl.burst, rate_percent)
    writeBuffer(rows, filepath)

  def dump_NoC_verilog_multi(self, dump_path):
    path = './' + os.path.normpath(dump_path) + '/'
    # path = dump_path
    h = self.H
    w = self.W
    X_str = ''
    Y_str = ''
    r_str = ''
    p_str = ''
    b_str = ''
    for y in range(h):
      for x in range(w):
        NodeV = self.Nodes[y][x]
        for fl in NodeV.Flows:
            X_str += '%x\n' % fl.dx
            Y_str += '%x\n' % fl.dy
            r = (fl.rate * 100)
            r_str += '%x\n' % r
            p_str += '%x\n' % fl.period
            b_str += '%x\n' % fl.burst
  
    if not os.path.exists(path):
      print 'Creating new directory'
      retV = CMD.getoutput('mkdir -p %s' % path)
      print retV
      # exit(1)
    # print 'Path = %s'% path
    writeBuffer(X_str, path + 'destx.dat')
    writeBuffer(Y_str, path + 'desty.dat')
    writeBuffer(r_str, path + 'destrate.dat')
    writeBuffer(p_str, path + 'destperiod.dat')
    writeBuffer(b_str, path + 'destburst.dat')


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
    FlowV = FlowT(dx, dy, rate, burst)
    NoCV.Nodes[sy][sx].Flows.append(FlowV)

  return NoCV


if __name__ == '__main__':
    ifilename = sys.argv[1]
    odire = sys.argv[2]
    print sys.argv
    X1 = parse_mapfile(ifilename)
    X1.dump_NoC_verilog_multi(odire)
