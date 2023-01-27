#include "common.h"
#include "constants.h"
#include <hls_stream.h>

void dram_module(
    cacheline_t* dram,
    hls::stream<DRAMRequest>&  dram_req,
    hls::stream<DRAMResponse>& dram_resp
    ) {
#pragma HLS pipeline off
#pragma HLS interface ap_ctrl_hs port=return
  while(true) {
    auto req = dram_req.read();
    auto real_addr = req.addr >> log2c_CL_SIZE;
    if (req.addr & 1) {
      dram[real_addr] = req.data;
    } else {
      auto data = dram[real_addr];
      dram_resp.write(DRAMResponse{req.addr, data});
    }
  }
}