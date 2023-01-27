- Guide to get interconnect latency.
- We use a 2 x N topology for 2 * N core setup, for example, for 6-core setup, we use a topology of 2 x N

- To get L1msg (the WCL to transfer 1 message), use `bash batch.sh`
```
$ bash batch.sh
N = 1
4

N = 2
10

N = 3
19

N = 4
34

N = 5
51
```

To get Lnmsg (the WCL to transfer N messages), use `python3 calc.py`. The calculation assumes no flow information and the flows are regulated with the a token-bucket of size 1 similar to the HopliteRT paper.
```
$ python3 calc.py
---- 1 ----
T_f:  2
L_Nmsg:  7
---- 2 ----
T_f:  5
L_Nmsg:  32
---- 3 ----
T_f:  8
L_Nmsg:  74
---- 4 ----
T_f:  11
L_Nmsg:  130
---- 5 ----
T_f:  14
L_Nmsg:  204
```
