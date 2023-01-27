#include "common.h"
#include "addr_map.h"
#include "protocol.h"
#include "private_cache.h"
#include "debug.h"

#include <ap_int.h>
#include <hls_stream.h>
#ifndef __SYNTHESIS__

#define HLS_ASSERT(cond, log_to) \
    ++cur_error_position; \
if(!(cond)) { \
log_entry_t le; \
le.error_code = cur_error_position; \
log_to.write(le); \
assert(false); \
}
#else
// #define HLS_ASSERT(cond, log_to) \
//     ++cur_error_position; \
// if(!(cond)) { \
// log_entry_t le; \
// le.error_code = cur_error_position; \
// log_to.write(le); \
// }
 #define HLS_ASSERT(cond, log_to)
#endif



// TODO: add support for multiple requests
// TODO: add log interface for debugging
void private_cache(
                   hls::stream<Request>& request_if,
    // Request req,
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
)
{

// #pragma HLS pipeline style=frp
#pragma HLS pipeline off

// #pragma HLS INTERFACE mode=ap_none register_mode=both port=req register
// #pragma HLS DISAGGREGATE variable=req
// #pragma HLS INTERFACE mode=ap_ctrl_hs port=return
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
#pragma HLS INTERFACE mode=axis register_mode=both port=request_if register
#pragma HLS INTERFACE mode=axis register_mode=both port=to_core register
#pragma HLS INTERFACE mode=axis register_mode=both port=to_intc register
// #pragma HLS INTERFACE mode=axis register_mode=both port=to_log register
// #pragma HLS INTERFACE mode=axis register_mode=both port=replay register


  // Tag array
  INSTANCE_SPECIFIC_VAR(static tag_entry_t tag[NSET][NWAY]);
  INSTANCE_SPECIFIC_VAR(static bool tag_valid[NSET][NWAY]);
#pragma HLS BIND_STORAGE variable=tag type=ram_t2p impl=bram
#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=tag
#pragma HLS reset variable=tag_valid
#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=tag_valid

  // Replacement policy
  INSTANCE_SPECIFIC_VAR(static age_t  age[NSET][NWAY]);
#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=age
#pragma HLS reset variable=age

  // Data array
  INSTANCE_SPECIFIC_VAR(static cacheline_t data[NSET][NWAY]);
#pragma HLS BIND_STORAGE variable=data type=ram_t2p impl=uram
#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=data

  // MSHR for request from core
  INSTANCE_SPECIFIC_VAR(static Request mshr[MSHR_ENTRIES]);
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=mshr
  INSTANCE_SPECIFIC_VAR(static bool	mshr_valid[MSHR_ENTRIES]);
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=mshr_valid
#pragma HLS reset variable=mshr_valid

  // MSHR for request from core
  INSTANCE_SPECIFIC_VAR(static Request replay_buffer[REPLAY_BUFFER_ENTRIES]);
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=replay_buffer
  INSTANCE_SPECIFIC_VAR(static bool	replay_buffer_valid[REPLAY_BUFFER_ENTRIES]);
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=replay_buffer_valid

  // Pending request: age is meaning less
  INSTANCE_SPECIFIC_VAR(static tag_entry_t pr[OUTSTANDING_REQUESTS]);
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=pr
  INSTANCE_SPECIFIC_VAR(static ap_uint<log2c_NWAY> pr_way[OUTSTANDING_REQUESTS]);
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=pr_way
  // received acks from other caches
  INSTANCE_SPECIFIC_VAR(static ap_uint<log2c_N> pr_acks[OUTSTANDING_REQUESTS]);
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=pr_acks
#pragma HLS reset variable=pr_acks
  INSTANCE_SPECIFIC_VAR(static ap_uint<log2c_N> pr_acks_total[OUTSTANDING_REQUESTS]);
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=pr_acks_total
#pragma HLS reset variable=pr_acks_total
  INSTANCE_SPECIFIC_VAR(static bool		 pr_valid[OUTSTANDING_REQUESTS]);
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=pr_valid
#pragma HLS reset variable=pr_valid
  INSTANCE_SPECIFIC_VAR(static bool		 pr_data_valid[OUTSTANDING_REQUESTS]);
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=pr_data_valid
#pragma HLS reset variable=pr_data_valid
  // forward only to a single destination
  INSTANCE_SPECIFIC_VAR(static target_t pr_fwd_dest[OUTSTANDING_REQUESTS]);
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=pr_fwd_dest

#ifndef  __SYNTHESIS__
  DBG("====== New Request ====== \n");

  if(reset) {
    // resetting static logic for software simulation
    DBG("resetting internal structures\n");
    ___ext_tag = &tag[0][0];
    ___ext_data = &data[0][0];
    for (int i = 0; i < NSET; i++) {
      for (int j = 0; j < NWAY; j++) {
        _acc(tag[i][j]).valid = false;
        _acc(tag_valid[i][j]) = false;
      }
    }
    for (int i = 0; i < MSHR_ENTRIES; i++) {
      _acc(mshr_valid[i]) = false;
    }
    for (int i = 0; i < REPLAY_BUFFER_ENTRIES; i++) {
      _acc(replay_buffer_valid[i]) = false;
    }
    for (int i = 0; i < OUTSTANDING_REQUESTS; i++) {
      _acc(pr_valid[i]) = false;
      _acc(pr_acks[i]) = 0;
      _acc(pr_acks_total[i]) = 0;
      _acc(pr_data_valid[i]) = 0;
    }
    return;
  }
#endif
  Request req = request_if.read();
  DBG("Received request: " << req << std::endl);

  event_t event = NONE;
  coherence_state_t state = I, next_state = I;

  // Stage 1: check tags & victim buffer
  tag_t req_tag = to_tag(req.addr);
  addr_t addr = to_cl_addr(req.addr);
  ap_uint<log2c_NSET> req_idx = req_tag & (NSET - 1);
  ap_uint<log2c_NWAY> hit_way, vacant_idx, victim_idx, max_age = 0, pr_way_idx, way_idx;
  tag_entry_t hit_tag, hit_pr_tag, victim_tag, tag_entry;
  bool hit = false;
  bool has_vacant = false;
  bool hit_pr = false;
  bool has_vacant_pr = false;
  int cur_error_position = 0;
  ap_uint<log2c_MSHR_ENTRIES> hit_mshr_idx, vacant_mshr_idx;
  ap_uint<log2c_OUTSTANDING_REQUESTS> hit_pr_idx, vacant_pr_idx;
  bool has_vacant_mshr = false;
  tag_check:
  for(int i = 0; i < NWAY; i++) {
#pragma HLS UNROLL
    tag_entry_t t = _acc(tag[req_idx][i]);
    bool t_valid = _acc(tag_valid[req_idx][i]);
#ifndef __SYNTHESIS__
    if(t_valid) {
      DBG("tag[" << req_idx << "][" << i << "] = " << t << std::endl);
    }
#endif
    if(t_valid && t.tag == req_tag) {
      hit = true;
      hit_way = i;
      hit_tag = t;
    }
    if(!t_valid) {
      has_vacant = true;
      vacant_idx = i;
    }
    // LRU
    if(t_valid && _acc(age[req_idx][i]) >= max_age) {
      max_age = _acc(age[req_idx][i]);
      victim_idx = i;
      // tag_entry = t;
      victim_tag = t;
    }
  }

  pr_check:
  for(int i = 0; i < OUTSTANDING_REQUESTS; i++) {
#pragma HLS UNROLL
    tag_entry_t pr_req = _acc(pr[i]);
    bool vld = _acc(pr_valid[i]);
    if(vld && pr_req.tag == to_tag(req.addr)) {
      DBG("Hit PR entry\n");
      hit_pr = true;
      hit_pr_tag = pr_req;
      pr_way_idx = _acc(pr_way[i]);
      // tag_entry = pr_req;
      hit_pr_idx = i;
    }
    if(!vld) {
      has_vacant_pr = true;
      vacant_pr_idx = i;
    }
  }

  // HLS_ASSERT(!(hit_pr && hit), to_log);

  mshr_check:
  for(int i = 0; i < MSHR_ENTRIES; i++) {
    auto vld = _acc(mshr_valid[i]);
    if(!vld) {
      vacant_mshr_idx = i;
      has_vacant_mshr = true;
      break;
    }
  }

  if(is_core(req)) {
    // HLS_ASSERT(has_vacant_mshr, to_log);
  }
  // fetch state
  if(hit) {
    DBG("Hit tag array: " << hit_tag << ", " << hit_way << std::endl);
    state = hit_tag.coh;
    way_idx = hit_way;
    tag_entry = hit_tag;
  } else if(hit_pr) {
    DBG("Hit PR: " << hit_pr_tag << ", pr_data_valid? " << pr_data_valid[hit_pr_idx] << std::endl);
    DBG("addr = " << std::hex << (unsigned long) addr << std::hex << std::endl);
    state = hit_pr_tag.coh;
    way_idx = pr_way_idx;
    tag_entry = hit_pr_tag;
  } else if(has_vacant) {
    way_idx = vacant_idx;
  } else if(!has_vacant) {
    DBG("No vacant, replacement required: " << victim_tag << std::endl);
    state = I; // invalid
    way_idx = victim_idx;
    // state = victim_tag.coh;
    // addr = tag_to_addr(victim_tag.tag);
    // way_idx = victim_idx;
  } // otherwise the state is I

  // Stage 2: encode events & data array read
  cacheline_t cl = req.data;
  bool        is_combine = false;
  addr_t      combine_addr;
  request_t   wb_req;
  cacheline_t victim_data;
  // collect read operations of the data cache
  if(hit || (get_network_id(req) == CoreNetwork && !has_vacant) || (hit_pr && pr_data_valid[hit_pr_idx])) {
    // the hit_pr && ... condition happens when the data arrives earlier than the ack
    cl = _acc(data[req_idx][way_idx]);
    DBG("Getting data from data array " << cl.to_string(16) << std::endl);
  }

  if(get_network_id(req) == CoreNetwork) {
    if(get_type(req) == RequestEnum::LD) {
      event = Load;
    } else if(get_type(req) == RequestEnum::ST) {
      event = Store;
    } else {
#ifndef __SYNTHESIS__
      assert(false);
#endif
    }
    // TODO: move this data out
    if(!has_vacant && !hit) { // only activated when LD/ST
      DBG("Core trying to issue combined request" << std::endl);
      is_combine = true;
      combine_addr = tag_to_addr(victim_tag.tag);
      victim_data = _acc(data[req_idx][victim_idx]);
      if(victim_tag.coh == S) {
        wb_req = PutS;
      } else if(victim_tag.coh == M) {
        wb_req = PutM;
      } else {
#ifndef __SYNTHESIS__
        assert(false);
#endif
      }
      has_vacant = true;
    }
  } else if(get_network_id(req) == ReqNetwork) {
    // should not happen
#ifndef __SYNTHESIS__
    assert(false);
#endif
  } else if(get_network_id(req) == FwdNetwork) {
    if(get_type(req) == GetS) {
      event = FwdGetS;
    } else if(get_type(req) == GetM) {
      // This can be happening on behalf of an LLC
      event = FwdGetM;
    } else if(get_type(req) == Invalidate) {
      // Inv is for shared cache line
      event = Inv;
    } else {
#ifndef __SYNTHESIS__
      assert(false);
#endif
    }
  } else if(get_network_id(req) == RespNetwork) {
    if(get_type(req) == Data) {
      DBG("Data >>> " << std::hex << req.data.to_string(16) << std::dec << std::endl);
#ifndef __SYNTHESIS__
      assert(hit_pr);
#endif
      if (is_dir(req)) {
        if (get_acks(req) == 0) {
          event = DataFromDirNoAck;
        } else {
#ifndef __SYNTHESIS__
          assert(hit_pr);
          DBG("Total acks required: " << get_acks(req) << std::endl);
#endif
          int recvd = pr_acks[hit_pr_idx];
          if(recvd == get_acks(req)) {
            event = DataFromDirLastAck;
          } else {
            event = DataFromDirAck;
          }
        }
      } else {
        event = DataFromOwner;
      }
    } else if(get_type(req) == Ack) {
      DBG("Ack received >>> " << std::hex << req.addr.to_string(16) << std::dec << std::endl);
#ifndef __SYNTHESIS__
      assert(hit_pr);
#endif
      int total = pr_acks_total[hit_pr_idx];
      int recvd = pr_acks[hit_pr_idx];
      if(recvd == total - 1) {
        event = LastInvAck;
      } else {
        event = InvAck;
      }
    }
  }

  // Stage 3: Coherence checks
  // TODO: insert coherence table
  MSIProtocolPrCacheControl ctrl;
  private_cache_data    data_path;
  log_entry_t lg;
  lg.addr = req.addr;
  lg.state = state;
  lg.event = event;
  lg.covered = true;
  lg.error_code = 0xff;


#include "private_cache_actions.inc.h"
  else {
    lg.covered = false;
#ifndef __SYNTHESIS__
    DBG("Unspecified event: " << event.to_string(10) << ", " << state.to_string(10) << std::endl);
    assert(false);
#endif
  }
  // Stage 4: Update & communication
  // tag array modification


  // pr modification
#ifndef __SYNTHESIS__
  assert(!(ctrl.pr_insert && ctrl.pr_remove));
#endif
  if(ctrl.pr_insert) {

    tag_entry_t new_tag;
    new_tag.valid = true;
    new_tag.tag = to_tag(req.addr);
    new_tag.coh = ctrl.update_tag_next_state;
#ifndef __SYNTHESIS__
    DBG("Inserting cache line into PR at " << vacant_pr_idx << ", " << new_tag << std::endl);
    assert(has_vacant_pr);
    // assert(has_vacant);
#endif
    _acc(pr_valid[vacant_pr_idx]) = true;
    _acc(pr[vacant_pr_idx]) = new_tag;
    if(ctrl.pr_migrate_from_tag) { // migrating from tag, no need to perform replacement
#ifndef  __SYNTHESIS__
      assert(hit);
#endif
      _acc(pr_way[vacant_pr_idx]) = hit_way;
    } else {
      _acc(pr_way[vacant_pr_idx]) = way_idx;
      DBG("Setting pr_way to be " << way_idx << std::endl);
    }
    _acc(pr_acks[vacant_pr_idx]) = 0;
    _acc(pr_acks_total[vacant_pr_idx]) = 0;
  } else if(ctrl.pr_remove) {
#ifndef __SYNTHESIS__
    DBG("Removing PR at " << hit_pr_idx << std::endl);
    assert(hit_pr);
#endif

    _acc(pr_valid[hit_pr_idx]) = false;
    _acc(pr_data_valid[hit_pr_idx]) = false;
#ifndef __SYNTHESIS__
    // for(int i = 0; i < OUTSTANDING_REQUESTS; i++) {
    //   DBG("PR_VALID " << i << ", " << (int)_acc(pr_valid[i]) << "\n");
    // }
#endif
  }

  // mshr modification
  // to_core
  if(ctrl.mshr_insert) {
    DBG("Inserting request to 0x" << std::hex << (unsigned long)req.addr << std::dec << " into MSHR\n");
    _acc(mshr_valid[vacant_mshr_idx]) = true;
    _acc(mshr[vacant_mshr_idx]) = req;
  } else if (ctrl.mshr_fulfill_all) {
    // DBG("Cleaning MSHR\n");
    for(int i = 0; i < MSHR_ENTRIES; i++) {
#pragma HLS pipeline
      // send to the request
      Request pending_core_request = _acc(mshr[i]);
      bool req_vld = _acc(mshr_valid[i]);
      // DBG("Checking MSHR[" << i << "] = " << pending_core_request << std::endl);
      // DBG("Checking MSHR[" << i << "] . req.addr = " << std::hex << (unsigned long)addr << std::dec << std::endl);
      if(req_vld && to_cl_addr(pending_core_request.addr) == addr) {
        // potentially transforming the data
        if(get_type(pending_core_request) == LD) {
          Response resp;
          resp.addr = to_cl_addr(pending_core_request.addr);
          resp.data = cl;
          to_core.write(resp);
        } else if(get_type(pending_core_request) == ST) {  // Store or atomic operation
          // We deprecate the support for AMO in L2
          cl = pending_core_request.data;
          // more like an acknowledgement
          Response resp;
          resp.addr = to_cl_addr(pending_core_request.addr);
          resp.data = 0;
          to_core.write(resp);
        } else {
#ifndef __SYNTHESIS__
          DBG("Unsupported");
          assert(false);
#endif
        }
        mshr_valid[i] = false;
      }
    }
  }

  // hit response?

  // potentially transforming the data
  DBG("Cacheline read: " << cl.to_string(16) << std::endl);
  if(ctrl.to_core_respond) {
#ifndef __SYNTHESIS__
    DBG("Responding to core...\n");
    assert(get_network_id(req) == CoreNetwork);
#endif
    if (get_type(req) == LD) {
      Response resp;
      resp.addr = to_cl_addr(req.addr);
      resp.data = cl;
      to_core.write(resp);
    } else if (get_type(req) == ST) {
      DBG("to_core_respond for ST\n");
      word_t _wd = 0;
      cl = req.data;
      Response resp;
      resp.addr = to_cl_addr(req.addr);
      resp.data = 0;
      to_core.write(resp);
    } else {
#ifndef __SYNTHESIS__
      DBG("Unsupported...\n");
      assert(false);
#endif
    }
  }
  DBG("Before: " << tag_entry << std::endl);
  if(ctrl.update_tag_update) {
    // modify tag_entry
    if (ctrl.update_tag_update_state) {
      tag_entry.coh = ctrl.update_tag_next_state;
    }
#ifndef __SYNTHESIS__
    assert(!(ctrl.update_tag_clean && ctrl.update_tag_dirty));
#endif
    if (ctrl.update_tag_dirty) {
      tag_entry.dirty = true;
    } else if (ctrl.update_tag_clean) {
      tag_entry.dirty = false;
    }
    // insert or update data
    if (ctrl.update_tag_insert || (hit && !ctrl.update_tag_remove) ) {
      tag_entry.valid = true;
      DBG("Finally inserting or updating to " << tag_entry << std::endl);
      _acc(tag[req_idx][way_idx]) = tag_entry;
      _acc(tag_valid[req_idx][way_idx]) = true;
    } else if (ctrl.update_tag_remove) {
#ifndef  __SYNTHESIS__
      DBG("Removing tag at " << req_idx << ", " << way_idx << std::endl);
      assert(hit);
#endif
      tag_entry_t t;
      t.valid = false;
      _acc(tag[req_idx][way_idx]) = t;
      _acc(tag_valid[req_idx][way_idx]) = false;
    } else if(!(ctrl.pr_insert || ctrl.pr_remove)) {
#ifndef __SYNTHESIS__
      assert(hit_pr);
#endif
      if(ctrl.update_tag_set_ack) {
        _acc(pr_acks_total[hit_pr_idx]) = get_acks(req);
      }
      if(ctrl.update_tag_inc_ack) {
        _acc(pr_acks[hit_pr_idx]) += 1;
      }
      if(ctrl.update_tag_record_forward) {
        _acc(pr_fwd_dest[hit_pr_idx]) = get_respond_to(req);
      }
      _acc(pr[hit_pr_idx]) = tag_entry;
    }
  }

  // data array modification
  if(ctrl.data_insert) {
    DBG("Inserting into data[" << req_idx << "][" << way_idx << "]=" << cl.to_string(16) << "\n");
    _acc(data[req_idx][way_idx]) = cl;
    // TODO: mark valid in pr control isntead of here
    if(hit_pr && !ctrl.pr_remove) {
      _acc(pr_data_valid[hit_pr_idx]) = true;
    }
  }
  if(get_network_id(req) == CoreNetwork) {
    DBG("Updating LRU\n");
    for(unsigned int i = 0; i < NWAY; i++) {
#pragma HLS unroll
      if(i == way_idx) {
        _acc(age[req_idx][way_idx]) = 0;
      } else {
        age_t a = _acc(age[req_idx][i]);
        if (a != age_t(0) - 1) a++;
        _acc(age[req_idx][i]) = a;
      }
    }
  }

  // to_noc
  if(ctrl.to_noc_send) {
    Request req_to_noc;
    /* initialization */
    /* initialization */
    // req_to_noc.type_ = ctrl_path.;
    req_to_noc.addr = to_cl_addr(req.addr);
    req_to_noc.wb_addr = 0;
    req_to_noc.respond_to__src__network_id = 0;
    set_network_id(req_to_noc, ctrl.to_noc_network);
    // req_to_noc.data = cl;
    // CoreNetwork -> RequestNetwork + Directory
    // FwdNetwork -> ResponseNetwork + Core/Directory
    machine_destination_t dst = new_machine_destination();
    if(get_network_id(req) == CoreNetwork) {
      dst = add_dest(dst, address_to_directory_id(addr));
    }
    DBG("DST: before " << dst.to_string(2) << std::endl);;
#ifndef __SYNTHESIS__
    assert(ctrl.to_noc_network != FwdNetwork);
#endif
    if(ctrl.to_noc_network == RespNetwork) {
      // sending to the core receiving the line
      if(ctrl.to_noc_also_to_dir) {
        DBG("Also sending to the directory..." << std::endl);
        dst = add_dest(dst, address_to_directory_id(addr));
      }
      if(ctrl.to_noc_destination == DestFromPR) {
#ifndef __SYNTHESIS__
        DBG("Sending to the core receiving the line\n");
        assert(hit_pr);
#endif
        dst = add_dest(dst, _acc(pr_fwd_dest[hit_pr_idx]));
      } else if(ctrl.to_noc_destination == DestFromMessage) {
        dst = add_dest(dst, get_respond_to(req));
      } else {
#ifndef __SYNTHESIS__
        assert(ctrl.to_noc_also_to_dir);
#endif
      }
    }

    if(is_combine) {
      DBG("Evicting: " << combine_addr << std::endl);
      set_combined_type(req_to_noc, combine_addr, wb_req);
      // req_to_noc.data = victim_data;
      cl = victim_data;
    } else {
      set_combined_type(req_to_noc, 0, 0);
    }
    DBG("Sending message to the NOC... with data: " << cl.to_string(16) << std::endl);
    req_to_noc.data = cl;
    set_dst(req_to_noc, dst);
    // req_to_noc.dst = dst;
    set_type(req_to_noc, ctrl.to_noc_message);
    // req_to_noc.type_ = ctrl.to_noc_message;
    set_src(req_to_noc, getCacheID(id));
    // req_to_noc.src = getCacheID(id);
    DBG("Sending to noc: " << req_to_noc << std::endl);
    to_intc.write(req_to_noc);
  }

  if(is_combine) {
    _acc(tag_valid[req_idx][way_idx]) = false;
  }

  // replay buffer modification
  // replay.write(req);

  // log writing
  // to_log.write(lg);
}
