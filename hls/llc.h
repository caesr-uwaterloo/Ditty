#ifndef VITIS_SYSTEM_LLC_H
#define VITIS_SYSTEM_LLC_H

#include "common.h"

#include <hls_stream.h>

void llc(
    hls::stream<Request>& req_in,
    hls::stream<Request>& wb_in,
    hls::stream<Request>& rsp_in,

    hls::stream<DRAMRequest>&  dram_req,
    hls::stream<DRAMResponse>& dram_resp,

    hls::stream<Request>&     to_intc
    // hls::stream<log_entry_t>& to_log,
#ifndef __SYNTHESIS__
    ,
    bool reset
#endif
);

#endif //VITIS_SYSTEM_LLC_H
