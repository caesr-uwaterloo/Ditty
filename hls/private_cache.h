#pragma once
#ifndef VITIS_SYSTEM_MSI_PRIVATE_CACHE_H
#define VITIS_SYSTEM_MSI_PRIVATE_CACHE_H
#include "common.h"
#include "msi_protocol.h"
#include <hls_stream.h>

struct private_cache_data {
  // data path
  addr_t addr;

  tag_entry_t tag_entry;
  cacheline_t data;

  Request to_noc;
};


/**
 *
 * @param request_if
 * @param to_core
 * @param to_intc
 * @param to_log
 * @param id
 * @param reset the reset signal facilitating tests from software side
 * @param ___ext_tag the ___ext_tag is used for injecting internal states
 * @param ___ext_data the ___ext_data is used for injecting internal states
 */
void private_cache(hls::stream<Request>& request_if,
                   hls::stream<Response>& to_core,
                   hls::stream<Request>& to_intc,
                   // hls::stream<log_entry_t>& to_log,
                   // hls::stream<Request>& replay,
                   int id
#ifndef __SYNTHESIS__
    ,
                   bool reset,
                   tag_entry_t*& ___ext_tag,
                   cacheline_t*& ___ext_data
#endif
);

#endif