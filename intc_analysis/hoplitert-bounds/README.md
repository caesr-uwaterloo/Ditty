# HopliteRT Analytical Bounds Tool

We provide two executable python scripts to help assist system engineers and developers to certify their communicating applications working on HopliteRT NoC.

To read more about HopliteRT NoC, please refer to the following links:

1. [HopliteRT Paper in the International Conference on Field-Programmable Technology, 2017](http://ieeexplore.ieee.org/)

2. [Technical Report](http://hdl.handle.net/10012/12600)

If you want to cite this work please use the following reference:

-  [Bibtex file](ref.bib)
- *S. Wasly, R. Pellizzoni and N. Kapre. HopliteRT: An Efficient FPGA NoC for Real-time Applications. In Proceedings of The International Conference on Field-Programmable Technology, 2017.*


## How to use the tool:

#### Generating a Network:
To generate a network you can use the executable python script `gennoc.py` from the command line. To print the usage help, type: `./gennoc.py -h` as shown below.

```
./gennoc.py -h
usage: gennoc.py [-h] [-o output] [-p Rate] [-t TrafficType] [-f flows]
                 [-b bursts]
                 Width Hight

NoC Traffic Generator

positional arguments:
  Width           the hight of the NoC
  Hight           the width of the NoC

optional arguments:
  -h, --help      show this help message and exit
  -o output       the map file
  -p Rate         overide the rate of all the flows, default = random (1/m^2,
                  0.5)
  -t TrafficType  set the traffice type => {random, all2one, all2row,
                  all2column}: default = random
  -f flows        the maximum number of different flows per node
  -b bursts       the maximum number of burstiness per flow

```

#### Running The Analysis:
Now after you generate a network map file according to your specifications, you can run the analysis tool against the generated map file using the executable python script `runnoc.py`. To print the usage help, type: `./runnoc.py -h` as shown below.

```
./runnoc.py -h
usage: runnoc.py [-h] [-o output] [-s] MapFile

NoC Analysis Tool

positional arguments:
  MapFile     the input map file

optional arguments:
  -h, --help  show this help message and exit
  -o output   the results file
  -s          silent mode, not output on stdout except the final (0/1) result

```

The results will be printed to the `stdout` if you did not silent it using the option `-s`. In addition, the detailed analytical bounds will be output to output file specified in the command line. You can use the same map file as an output file, it will append the results as further columns.

#### The Format of the Map file:
Here is an arbitrary example of a 2X2 NoC Map file, where sx,sy  and dx,dy are the source and destination of a given flow respectively. As you can note, the node at (0,0) has three flows to (0,1), (1,0) and (1,1). Each flow is governed by the maximum Burst `Burst` and the regulated steady rate `Rate`.

```
sx, sy, dx, dy, Burst,  Rate,   
0,  0,  0,  1,  1,      0.250000 
0,  0,  1,  0,  1,      0.250000 
0,  0,  1,  1,  1,      0.250000 
1,  0,  1,  1,  1,      0.250000 
0,  1,  1,  1,  1,      0.250000 
1,  1,  0,  0,  1,      0.000000

```

#### The Format of the Result file:

In the Result output file, the first 6 columns are the same as in map file. After that, the `./runnoc.py` appends information about flows as in the following example:

```
sx, sy, dx, dy, Burst,   Rate,     =>, Tf, Tf.opt, TsS,  TsE, Stable,  TotalWaiting_S, TotalWaiting_E, Period, Waiting_No_Burst_S, Waiting_No_Burst_E
0,  0,  1,  0,  1,       0.250000, =>, 3,  3,      0,    4,   True,    0,              7,              Prd=4,  0.000,              2.000
1,  0,  0,  0,  1,       0.250000, =>, 3,  3,      0,    4,   True,    0,              7,              Prd=4,  0.000,              2.000
0,  1,  1,  1,  1,       0.250000, =>, 3,  3,      0,    0,   True,    0,              3,              Prd=4,  0.000,              0.000
1,  1,  1,  0,  1,       0.250000, =>, 5,  5,      1,    0,   True,    4,              0,              Prd=4,  1.000,              0.000

```

Where `Tf` is the bound on the maximum in-flight latency (routing latency) of packets, whereas `Tf.opt` is an optimized version that provides tighter bound especially in large networks. Fore more details, please refer to the paper. `TsS` and `TsE` are the source-queuing time at the source node for the South port and the East port of the node respectively. `Stable` indicates if the specific flow has met its own rate and is able to send without starvation. `TotalWaiting_S` and `TotalWaiting_E` are the total waiting time at the source node, for the South and the East ports respectively, before being able to inject to the network according to our Token-Bucket regulation. `Waiting_No_Burst_S` and `Waiting_No_Burst_E` are the maximum waiting at the source node, for the South and the East ports respectively, at steady state, e.g., when there is no burstiness. Finally, `Period` is a helper columns to help comparing the maximum waiting at steady state to the period, since the maximum waiting must not exceed the period at steady state.


#### Simulation Instructions

To run Verilog simulations, you have to run the following set of steps:

1. Generate test.map with list of flows `./gennoc.py -o test.map -t random -f 1 -p 0.08 -b 1 5 5`
Here I'm using a 5x5 network and a rate of 0.08

2. Check if things are fine.. i.e. network is sustainable `./runnoc.py test.map`
If this step returns stable, you're fine.. try to increase `-p` rate to the largest before it fails
Run 1 and 2 alternately until you settle on a stable value of `-p`

3. Generate dat files for Verilog sim `$ ./map2verilogmap.py results.dat tmp`
This will take output of runnoc which is result.dat and put the dat files in tmp folder.

Remember to set filenames for each step as appropriate. Output of step 1 here is test.map which is input to step 2. Output of step 2 (by default) is result.dat which is input to step 3. Output of step 3 is a list of dat files which are required by the Verilog simulation.

### License
This tool is distributed under MIT license.

Copyright (c) 2017 Saud Wasly

<div style="text-align: justify;"> 
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
<br><br>
</div>


<div style="text-align: justify;"> 
<b>The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.</b>
<br><br>
</div>


<div style="text-align: justify;"> 
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 </div>