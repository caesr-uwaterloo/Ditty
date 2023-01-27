#include "constants.h"
#include "common.h"
#include "debug.h"
#include "addr_map.h"

#include <ap_int.h>
#include <hls_stream.h>

#ifndef GP
MSIProtocolDirControl llc_coherence_query(event_t event, coherence_state_t state, int id) {
  // Stage 3: Coherence checks
  // TODO: insert coherence table
  MSIProtocolDirControl ctrl;
#include "MSIProtocol_dir_actions.inc.h"
  else {
#ifndef __SYNTHESIS__
    DBG("[Dir] Unspecified event: " << event.to_string(10) << ", " << state.to_string(10) << std::endl);
    assert(false);
#endif
  }
  return ctrl;
}
#else
GPMSIProtocolDirControl llc_coherence_query(event_t event, coherence_state_t state, int id) {
  // Stage 3: Coherence checks
  // TODO: insert coherence table
  GPMSIProtocolDirControl ctrl;
#include "GPMSIProtocol_dir_actions.inc.h"
  else {
#ifndef __SYNTHESIS__
    DBG("[Dir] Unspecified event: " << event.to_string(10) << ", " << state.to_string(10) << std::endl);
    assert(false);
#endif
  }
  return ctrl;
}
#endif

// This module includes both LLC and Directory
// LLC is responsible for handling the requests from private caches
// Directory is responsible for tracking ownership/sharing of the cache lines
// The id is omited since we have a monolithic LLC
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
    ) {
#pragma HLS pipeline off
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
#pragma HLS INTERFACE mode=axis port=req_in register
#pragma HLS INTERFACE mode=axis port=wb_in register
#pragma HLS INTERFACE mode=axis port=rsp_in register
#pragma HLS INTERFACE mode=axis port=dram_req register
#pragma HLS INTERFACE mode=axis port=dram_resp register
#pragma HLS INTERFACE mode=axis port=to_intc register
// #pragma HLS INTERFACE mode=axis register_mode=both port=to_log register


  // Tag array / Directory
  INSTANCE_SPECIFIC_VAR(static llc_tag_t tag[DIR_NSET][DIR_NWAY]);
  INSTANCE_SPECIFIC_VAR(static bool tag_valid[DIR_NSET][DIR_NWAY]);
#pragma HLS BIND_STORAGE variable=tag type=ram_t2p impl=bram
#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=tag
#pragma HLS reset variable=tag_valid

  // Data array
  INSTANCE_SPECIFIC_VAR(static cacheline_t             data[LLC_NSET][LLC_NWAY]);
  INSTANCE_SPECIFIC_VAR(static addr_t                  data_tag[LLC_NSET][LLC_NWAY]);
  INSTANCE_SPECIFIC_VAR(static ap_uint<log2c_LLC_NWAY> data_age[LLC_NSET][LLC_NWAY]);
  INSTANCE_SPECIFIC_VAR(static bool                    data_valid[LLC_NSET][LLC_NWAY]);
#pragma HLS BIND_STORAGE variable=data type=ram_t2p impl=uram
#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=data

// #pragma HLS BIND_STORAGE variable=data_tag type=ram_t2p impl=bram
#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=data_tag

// #pragma HLS BIND_STORAGE variable=data_age type=ram_t2p impl=bram
#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=data_age

// #pragma HLS BIND_STORAGE variable=data_valid type=ram_t2p impl=bram
#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=data_valid

  // MSHR for request from core
  INSTANCE_SPECIFIC_VAR(static Request mshr[MSHR_ENTRIES]);
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=mshr
  INSTANCE_SPECIFIC_VAR(static bool	mshr_valid[MSHR_ENTRIES]);
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=mshr_valid
#pragma HLS reset variable=mshr_valid

  // Pending request: age is meaning less
  INSTANCE_SPECIFIC_VAR(static tag_entry_t pr[OUTSTANDING_REQUESTS]);
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=pr
  INSTANCE_SPECIFIC_VAR(static ap_uint<log2c_N> pr_acks[OUTSTANDING_REQUESTS]);
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=pr_acks
#pragma HLS reset variable=pr_acks
  INSTANCE_SPECIFIC_VAR(static ap_uint<log2c_N> pr_acks_total[OUTSTANDING_REQUESTS]);
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=pr_acks_total
#pragma HLS reset variable=pr_acks_total
  INSTANCE_SPECIFIC_VAR(static bool		 pr_valid[OUTSTANDING_REQUESTS]);
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=pr_valid
#pragma HLS reset variable=pr_valid


  INSTANCE_SPECIFIC_VAR(static hls::stream<Request> pending_request);
  INSTANCE_SPECIFIC_VAR(static int fwd_acks);
  INSTANCE_SPECIFIC_VAR(static bool blocking);
#pragma HLS stream variable=pending_request depth=12

  int id = 0;

#ifndef  __SYNTHESIS__

  if(reset) {
    // resetting static logic for software simulation
    DBG("resetting internal structures\n");
    while(!pending_request.empty()) {
      pending_request.read();
    }
    blocking = false;
    fwd_acks = 0;
    for (int i = 0; i < NSET; i++) {
      for (int j = 0; j < NWAY; j++) {
        _acc(tag_valid[i][j]) = false;
      }
    }
    for (int i = 0; i < MSHR_ENTRIES; i++) {
      _acc(mshr_valid[i]) = false;
    }

    for (int i = 0; i < OUTSTANDING_REQUESTS; i++) {
      _acc(pr_valid[i]) = false;
      _acc(pr_acks[i]) = 0;
      _acc(pr_acks_total[i]) = 0;
      // _acc(pr_data_valid[i]) = 0;
    }
    return;
  }

#endif

  Request req;
  DRAMResponse dresp {0xDEADBEEF, 0xffffffff};
  /*
  if(!dram_resp.empty()) {
#ifndef __SYNTHESIS__
    DBG("Reading from dram resp" << std::endl);
#endif
    dresp = dram_resp.read();
  } else */ if(!rsp_in.empty()) {
    req = rsp_in.read();
#ifndef __SYNTHESIS__
    DBG("Reading from rsp_in" << req << std::endl);
#endif
  } else if(!wb_in.empty()) {
    req = wb_in.read();
#ifndef __SYNTHESIS__
    DBG("Reading from wb_in " << req << std::endl);
#endif
  } else if(!blocking && !pending_request.empty()) {
    req = pending_request.read();
#ifndef __SYNTHESIS__
    DBG("Reading from pending_req" << req << std::endl);
#endif
  } else if(!blocking && !req_in.empty()) {
    req = req_in.read();
#ifndef __SYNTHESIS__
    DBG("Reading from req_in" << req << std::endl);
#endif
  } else {
    return;
  }
  DBG("=== === === NEW REQUEST === === ===" << std::endl);
  // handle request

  event_t event = NONE;
  coherence_state_t state = I, next_state = I;

  // Stage 1: check tags & victim buffer
  tag_t req_tag = (req.addr >> log2c_CL_SIZE);
  ap_uint<log2c_DIR_NSET> req_idx = req_tag & (DIR_NSET - 1);
  ap_uint<log2c_DIR_NWAY> hit_way, vacant_idx, way_idx;

  ap_uint<log2c_LLC_NSET> llc_req_idx = req_tag & (LLC_NSET - 1);
  ap_uint<log2c_LLC_NWAY> llc_hit_way, llc_vacant_idx, llc_victim_idx, llc_max_age = 0, llc_way_idx;
  addr_t llc_victim_tag = 0;

  llc_tag_t hit_tag, tag_entry;

  cacheline_t hit_data, victim_data;

  bool hit = false;
  bool hit_llc = false;
  bool has_vacant = false;
  bool llc_has_vacant = false;
  bool hit_mshr = false;

  tag_check_dir:
  for(int i = 0; i < DIR_NWAY; i++) {
#pragma HLS UNROLL
    llc_tag_t t = _acc(tag[req_idx][i]);
    bool t_valid = _acc(tag_valid[req_idx][i]);
#ifndef __SYNTHESIS__
    if(t_valid) {
      DBG("tag[" << req_idx << "][" << i << "] = " << t << std::endl);
    }
#endif
    if(t_valid && t.tag == req_tag) {
      hit = true;
      hit_way = i;
      tag_entry = hit_tag = t;
    }
    if(!t_valid) {
      has_vacant = true;
      vacant_idx = i;
    }
  }
  llc_check:
  for(int i = 0; i < LLC_NWAY; i++) {
#pragma HLS UNROLL
    addr_t t = _acc(data_tag[llc_req_idx][i]);
    bool t_valid = _acc(data_valid[llc_req_idx][i]);
    DBG("data[" << llc_req_idx << "][" << i << "] = " << tag_to_addr(t).to_string(16) << ", " << t << std::endl);
    if(t_valid && t == req_tag) {
      hit_llc = true;
      llc_hit_way = i;
    }
    if(!t_valid) {
      llc_has_vacant = true;
      llc_vacant_idx = i;
    }
    if(t_valid && _acc(data_age[llc_req_idx][i]) >= llc_max_age) {
      llc_max_age = _acc(data_age[llc_req_idx][i]);
      llc_victim_idx = i;
      llc_victim_tag = t;
    }
  }

  // reverse checking llc victim
  ap_uint<log2c_DIR_NSET> co_req_idx = llc_victim_tag & (DIR_NSET - 1);
  DBG("victim_tag = " << llc_victim_tag.to_string(16) << std::endl);
  ap_uint<log2c_DIR_NWAY> co_hit_way;
  bool co_hit = false;
  llc_tag_t co_hit_tag, co_tag_entry;
  llc_tag_co_check_dir:
  for(int i = 0; i < DIR_NWAY; i++) {
#pragma HLS UNROLL
    llc_tag_t t = _acc(tag[co_req_idx][i]);
    bool t_valid = _acc(tag_valid[co_req_idx][i]);
#ifndef __SYNTHESIS__
    if(t_valid) {
      DBG("tag[" << co_req_idx << "][" << i << "] = " << t << std::endl);
    }
#endif
    if(t_valid && t.tag == llc_victim_tag) {
      co_hit = true;
      co_hit_way = i;
      co_tag_entry = co_hit_tag = t;
    }
  }
  DBG("co_hit = " << co_hit << std::endl);


  // Stage 2: encode events & data array read
  cacheline_t cl = req.data;
  // if(hit_llc) {
  //   hit_data = _acc(data[llc_req_idx][llc_hit_way]);
  // }
  // when hit_llc, this is the hit data
  // otherwise, this is the victim data
  if(hit) {
    state = hit_tag.coh;
    way_idx = hit_way;
    llc_way_idx = llc_hit_way;
  } else if(has_vacant) {
    way_idx = vacant_idx;
    if(hit_llc) {
      state = V; // valid in LLC but not in directory/core
    } else {
      if(llc_has_vacant) {
        llc_way_idx = llc_vacant_idx;
      } else {
        llc_way_idx = llc_victim_idx;
      }
    }
  } else {
#ifndef __SYNTHESIS__
    DBG("No vacant entry in directory" << std::endl);
    assert(false);
#endif
  }
  if(hit || llc_has_vacant || hit_llc) {
#ifndef __SYNTHESIS__
#endif
    if(get_type(req) == GetS) {
      event = DirGetS;
    } else if(get_type(req) == GetM) {
      event = DirGetM;
    } else if(get_type(req) == PutM) {
      if(hit && get_src(req) == tag_entry.owner) {
        event = DirPutMFromOwner;
      } else {
        event = DirPutMFromNonOwner;
      }
    } else if(get_type(req) == PutS) {
#ifndef __SYNTHESIS__
      assert(hit);
#endif
      if(tag_entry.sharer_count == 1) {
        event = DirLastPutS;
      } else {
        event = DirPutS;
      }
    } else if(get_type(req) == FwdAck) {
      event = DirFwdAck;
    } else if(get_type(req) == Data) {
      event = DirDataResp;
    } else if(get_type(req) == RespOk) {
      event = DirRespOk;
    } else if(get_type(req) == Ack) {
      if(tag_entry.sharer_count == 1) {
        event = DirLastInvAck;
      } else {
        event = DirInvAck;
      }
    } else {
#ifndef __SYNTHESIS__
      DBG("Unknown request type " << get_type(req).to_string(10) << std::endl);
      assert(false);
#endif
    }
  } else if (!llc_has_vacant){
#ifndef __SYNTHESIS__
    assert(has_vacant);
#endif
    event = Replacement;
    llc_way_idx = llc_victim_idx;
    if(co_hit) {
      state = co_hit_tag.coh;
      // hit = co_hit;
      // hit_way = co_hit_way;
      tag_entry = hit_tag = co_hit_tag;
      req_idx = co_req_idx;
      way_idx = co_hit_way;
    } else {
      state = V;
    }
  } else {
#ifndef __SYNTHESIS__
    DBG("Unrecognized event" << std::endl);
    assert(false);
#endif
  }
  // cache read
  hit_data = _acc(data[llc_req_idx][llc_way_idx]);

  // Stage 3: Coherence checks
  // TODO: insert coherence table
#ifndef GP
  MSIProtocolDirControl ctrl;
#else
  GPMSIProtocolDirControl ctrl;
#endif
  log_entry_t lg;
  lg.addr = req.addr;
  lg.state = state;
  lg.event = event;
  lg.covered = true;

  ctrl = llc_coherence_query(event, state, id);
  DBG("Blocked ? " << blocking << std::endl);

  // TODO: add addr
  if(ctrl.dram_read) {
    DRAMRequest dreq { 0xDEAD, to_cl_addr(req.addr)};
#ifndef __SYNTHESIS__
    DBG("Reading from dram: " << dreq << std::endl);
#endif
    dram_req.write(dreq);
    dresp = dram_resp.read();
#ifndef __SYNTHESIS__
    DBG("Response: " << dresp << std::endl);
#endif
  } else if(ctrl.dram_write) {

    if(ctrl.dram_data_from_llc) {
#ifndef __SYNTHESIS__
      // not necessary - could be victim
      // assert(hit_llc);
#endif
      dram_req.write(DRAMRequest{ hit_data, to_cl_addr(req.addr) | 1 });
    } else if(ctrl.dram_data_from_req) {
      dram_req.write(DRAMRequest{ req.data, to_cl_addr(req.addr) | 1});
    } else {
#ifndef __SYNTHESIS__
      assert(false);
#endif
    }
  }

  if(ctrl.blockage_block) {
    blocking = true;
  } else if(ctrl.blockage_unblock) {
    blocking = false;
  }
  if(ctrl.blockage_set_fwd_ack_one) {
    fwd_acks = 1;
  }
  if(ctrl.blockage_set_fwd_ack_sharer) {
    fwd_acks = tag_entry.sharer_count;
  }
#ifndef __SYNTHESIS__
  assert(!ctrl.blockage_queue_req || !ctrl.blockage_unqueue_req);
#endif
  if(ctrl.blockage_queue_req) {
    DBG("Queueing request " << req << std::endl);
    pending_request.write(req);
  } else if(ctrl.blockage_unqueue_req) {
#ifndef __SYNTHESIS__
    assert(!pending_request.empty());
#endif
    DBG("Unqueueing request" << std::endl);
    pending_request.read();
  }
  if(ctrl.to_noc_send) {
    Request req_to_noc;
    req_to_noc.respond_to__src__network_id = 0;
    machine_destination_t  dst {0};
    if(ctrl.to_noc_to_requestor) {
      dst = single_destination(get_src(req));
    } else if(ctrl.to_noc_to_sharer) {
#ifndef __SYNTHESIS__
      DBG("Sending to sharers " << tag_entry.sharer.to_string(2) << std::endl);
      // assert(hit && hit_llc); // inclusive
#endif
      dst = tag_entry.sharer;
      if(event != Replacement && contains_destination(tag_entry.sharer, get_src(req))) {
        dst = remove_dest(dst, get_src(req));
      }
    } else if(ctrl.to_noc_to_owner) {
#ifndef __SYNTHESIS__
      DBG("Sending to owner " << tag_entry.owner.to_string(10) << std::endl);
#endif
      // may be replacement
      dst = single_destination(tag_entry.owner);
    } else {
#ifndef __SYNTHESIS__
      assert(false);
#endif
    }
    req_to_noc.addr = event == Replacement ? tag_to_addr(tag_entry.tag) : to_cl_addr(req.addr);
    set_type(req_to_noc, ctrl.to_noc_message);
    set_network_id(req_to_noc, ctrl.to_noc_network);
    set_src(req_to_noc, getDirID(0));
    if(ctrl.to_noc_mark_forward_to_requestor) {
      set_respond_to(req_to_noc, get_src(req));
    } else if(ctrl.to_noc_mark_forward_to_dir) {
      set_respond_to(req_to_noc, getDirID(0));
    } else {
      set_respond_to(req_to_noc, 0);
    }
    if(ctrl.to_noc_no_ack) {
      set_acks(req_to_noc,0);
    } else if(ctrl.to_noc_sharer_ack) {
      set_acks(req_to_noc, tag_entry.sharer_count);
      if(contains_destination(tag_entry.sharer, get_src(req))) {
        auto acks = get_acks(req_to_noc);
        set_acks(req_to_noc, acks - 1);
      }
    } else {
#ifndef __SYNTHESIS__
      assert(false);
#endif
    }
    if(ctrl.to_noc_data_from_dram) {
      req_to_noc.data = dresp.data;
    } else if(ctrl.to_noc_data_from_llc) {
#ifndef __SYNTHESIS__
      assert(hit_llc);
#endif
      req_to_noc.data = hit_data;
    } else {
      req_to_noc.data = req.data;
    }
    req_to_noc.dst = dst;
    DBG("Sending to... " << dst.to_string(2) << std::endl);
    to_intc.write(req_to_noc);
  }
  // sending the response
  if(ctrl.send_resp_to_requestor_with_data_valid) {

    Request req_to_noc;
    machine_destination_t  dst {0};
    req_to_noc.addr = to_cl_addr(req.addr);
    set_type(req_to_noc, Data);
    set_src(req_to_noc, getDirID(0));
    set_network_id(req_to_noc, RespNetwork);
    dst = single_destination(get_src(req));
#ifndef __SYNTHESIS__
    assert(hit);
#endif
    req_to_noc.data = hit_data;
    if(ctrl.send_resp_to_requestor_with_data_no_ack) {
      set_acks(req_to_noc, 0);
    } else if(ctrl.send_resp_to_requestor_with_data_sharer_ack) {
      set_acks(req_to_noc, tag_entry.sharer_count);
#ifndef __SYNTHESIS__
      assert(event != Replacement);
#endif
      if(contains_destination(tag_entry.sharer, get_src(req))) {
        DBG("Excluding requestor from acks" << std::endl);
        auto acks = get_acks(req_to_noc);
        set_acks(req_to_noc, acks - 1);
      }
    }
    req_to_noc.dst = dst;
    to_intc.write(req_to_noc);
  }


  llc_tag_t new_tag;
  if(ctrl.update_tag_update) {
    if (ctrl.update_tag_insert) {
      new_tag.tag = to_tag(req.addr);
      new_tag.sharer = machine_destination_t(0);
      new_tag.sharer_count = 0;
      new_tag.owner = -1;
      tag_entry = new_tag;
    }
    if(ctrl.update_tag_update_state) {
      tag_entry.coh = ctrl.update_tag_next_state;
    }
    if (ctrl.update_tag_add_requestor_as_sharer) {
      DBG("Original sharer " << tag_entry.sharer.to_string(2) << std::endl);
      DBG("Sharer count " << tag_entry.sharer_count.to_string(2) << std::endl);
      tag_entry.sharer = add_dest(tag_entry.sharer, get_src(req));
      DBG("After sharer " << tag_entry.sharer.to_string(2) << std::endl);
      tag_entry.sharer_count++;
    } else if (ctrl.update_tag_remove_requestor_from_sharer) {
#ifndef __SYNTHESIS__
      assert(contains_destination(tag_entry.sharer, get_src(req)));
#endif
      tag_entry.sharer = remove_dest(tag_entry.sharer, get_src(req));
      tag_entry.sharer_count--;
    } else if(ctrl.update_tag_mark_requestor_as_owner) {
      machine_destination_t sharer = single_destination(get_src(req));
      tag_entry.sharer = sharer;
      tag_entry.owner = get_src(req);
      tag_entry.sharer_count = 1;
    } else if(ctrl.update_tag_transition_to_shared) {
      machine_destination_t sharer = single_destination(tag_entry.owner);
      sharer = add_dest(sharer, get_src(req));
      tag_entry.sharer = sharer;
      tag_entry.owner = -1;
      tag_entry.sharer_count = 2;
    }
    _acc(tag[req_idx][way_idx]) = tag_entry;
    if(!hit && ctrl.update_tag_insert) {
      _acc(tag_valid[req_idx][way_idx]) = true;
    }
    if(ctrl.update_tag_remove) {
      _acc(tag_valid[req_idx][way_idx]) = false;
    }
  }

  if(ctrl.data_insert) {
#ifndef __SYNTHESIS__
    assert(llc_has_vacant);
    assert(ctrl.data_data_from_dram);
#endif
    _acc(data_tag[llc_req_idx][llc_vacant_idx]) = req_tag;
    _acc(data_valid[llc_req_idx][llc_vacant_idx]) = true;
    _acc(data[llc_req_idx][llc_vacant_idx]) = dresp.data;
  } else if(ctrl.data_remove) {
#ifndef __SYNTHESIS__
    // not necessary
    // assert(hit_llc);
#endif

    _acc(data_valid[llc_req_idx][llc_way_idx]) = false;
  } else if(ctrl.data_update) {
#ifndef __SYNTHESIS__
    assert(hit_llc);
    assert(ctrl.data_data_from_req);
#endif
    _acc(data[llc_req_idx][llc_hit_way]) = req.data;
  }





  // to_log.write(lg);
}
