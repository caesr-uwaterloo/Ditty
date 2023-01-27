import yaml
from glob import  glob
from argparse import ArgumentParser as AP
import os

parser = AP()
parser.add_argument('-c', choices=["gp", "ditty"], default="ditty")
parser.add_argument('-x', type=int, default=2)
parser.add_argument('-y', type=int, default=1)
args = parser.parse_args()

def get_files(folder):
    return list(map(os.path.basename, glob(folder + "/*.v") + glob(folder + "/*.sv")))

"""
module system_kernel (
  input               reset,
  input               clk,
  input               io_from_core_0_valid,
  output              io_from_core_0_ready,
  input      [255:0]  io_from_core_0_payload,
  input               io_from_core_1_valid,
  output              io_from_core_1_ready,
  input      [255:0]  io_from_core_1_payload,
  output              io_to_core_0_valid,
  input               io_to_core_0_ready,
  output     [255:0]  io_to_core_0_payload,
  output              io_to_core_1_valid,
  input               io_to_core_1_ready,
  output     [255:0]  io_to_core_1_payload,
  output              io_dram_req_0_valid,
  input               io_dram_req_0_ready,
  output     [255:0]  io_dram_req_0_payload,
  input               io_dram_resp_0_valid,
  output              io_dram_resp_0_ready,
  input      [255:0]  io_dram_resp_0_payload
);
"""

def get_stream_port(name, direction, clk="clk"):
    return {
            "clock": clk,
            "bus_type": "stream",
            "interface_mode": direction,
            "port_map": {
                "TDATA": f"{name}_payload",
                "TVALID": f"{name}_valid",
                "TREADY": f"{name}_ready",
                }
            }



def get_interfaces():
    res = {}
    for i in range(args.x * args.y):
        name = f"from_core_{i}"
        res[name] = get_stream_port(name, direction="slave")
        name = f"to_core_{i}"
        res[name] = get_stream_port(name, direction="master")

    # assuming only 1 LLC
    name = f"dram_req_0"
    res[name] = get_stream_port(name, direction="master")

    name = f"dram_resp_0"
    res[name] = get_stream_port(name, direction="slave")

    return res

cn = f"{args.c}_{args.x}_{args.y}"
result = {
        "kernel_name": "system_kernel", 
        "clock": "clk",
        "reset": {
            "clock": "clk",
            "name": "reset",
            "polarity": "ACTIVE_HIGH"
            },
        "files": get_files(cn),
        "bus_interfaces": get_interfaces()
        }


with open(f'{cn}/desc.yml', 'w') as outfile:
    yaml.dump(result, outfile, default_flow_style=None)
