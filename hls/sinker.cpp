#include "common.h"
#include "hls_stream.h"
#include <ap_int.h>

void sinker(
    Request* mem,
    Response* mem_out,
    int nreq,
    hls::stream<Request>& req,
    hls::stream<Response>& to_core
){
  static ap_uint<64> max_lat = 0;
  static ap_uint<64> wallclk = 0;
  for(int i = 0; i < nreq; i++) {
#pragma HLS pipeline off
    Request r = mem[i];
    req.write(r);
    Response resp;
    sinker_wait_loop:
    while(!to_core.read_nb(resp)) ;

    if(resp.latency > max_lat) {
      max_lat = resp.latency;
    }
    resp.wallclock = max_lat;
    resp.latency = max_lat;
    mem_out[0] = resp;
  }
  // if(!to_log.empty()) to_log.read();
  // if(!replay.empty()) replay.read();
}
