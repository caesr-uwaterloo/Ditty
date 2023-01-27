from argparse import ArgumentParser as AP
from pathlib import Path
import os

parser = AP()
parser.add_argument('-c', choices=["gp", "ditty"], default="ditty")
parser.add_argument('-x', type=int, default=2)
parser.add_argument('-y', type=int, default=1)
args = parser.parse_args()

cn = f"{args.c}_{args.x}_{args.y}"

n = args.x * args.y

res = f"""
kernel_frequency=100
[connectivity]
nk=sinker:{n}
nk=dram_module:1
nk=system_kernel:1
"""
for i in range(n):
    res += f"sc=sinker_{1+i}.req:system_kernel_1.from_core_{i}\n"
    res += f"sc=system_kernel_1.to_core_{i}:sinker_{1+i}.to_core\n"

res += f"sc=system_kernel_1.dram_req_0:dram_module_1.dram_req\n"
res += f"sc=dram_module_1.dram_resp:system_kernel_1.dram_resp_0\n"

Path(f"{cn}/connect.cfg").write_text(res)

