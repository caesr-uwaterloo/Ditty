#!/usr/bin/env python
# -*- coding: utf-8 -*-
__author__ = 'Saud Wasly'

from math import ceil
class NodeT(object):
  def __init__(self, sx ,sy, flowsP=[]):
    self.sx = sx
    self.sy = sy
    self.Flows = flowsP #type: list[FlowT]
    self.Max_Waiting_E = 0
    self.Max_Waiting_S = 0
    self.Max_TotalWaiting_E = 0
    self.Max_TotalWaiting_S = 0
    self.Sustained_Waiting_E = 0
    self.Sustained_Waiting_S = 0
    self.ResponseTime = 0

class FlowT(object):
  def __init__(self,dx,dy, rate, burst):
    self.dx = dx
    self.dy = dy
    self.rate = rate
    self.burst = burst
    self.burst_orig = burst
    # self.period = int(ceil(1.0/rate)) if rate!=0 else 1
    self.period = int((1.0/rate)) if rate!=0 else 1
    self.stable = False
    self.fl_Waiting_E = 0
    self.fl_Waiting_S = 0
    if not rate > 0.0:
      self.burst = 0

    self.InFlight_latency = 0
    self.InFlight_latency_opt = 0 #the optimized bound based on the flows
  def __str__(self):
    S = '\tdest X = %d\n' %self.dx
    S += '\tdest Y = %d\n' %self.dy
    S += '\trate = %.3f\n' %self.rate
    S += '\tperiod = %d\n' %self.rate
    S += '\tburst = %d\n' %self.burst
    return S


class TTypes:
  Random = 'random'
  All2One = 'all2one'
  All2Row = 'all2row'
  All2Column = 'all2column'
