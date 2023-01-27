#ifndef __MSIProtocol__AUTOGEN_GUARD_H
#define __MSIProtocol__AUTOGEN_GUARD_H
#include <ap_int.h>

enum StateEnum {
  I = 0,
  S = 1,
  M = 2,
  IS_AD = 3,
  IS_D = 4,
  IS_DI = 5,
  IM_AD = 6,
  IM_A = 7,
  IM_AD_I = 8,
  IM_AD_S = 9,
  IM_A_I = 10,
  IM_A_S = 11,
  SM_AD = 12,
  SM_A = 13,
  SM_A_I = 14,
  SM_A_S = 15,
  SM_A_S_I = 16,
  SM_AD_I = 17,
  SM_AD_S = 18,
  SM_AD_S_I = 19,
  MS_AD = 20,
  MS_D = 21,
  MS_A = 22,
  MI_AD = 23,
  MI_A = 24,
  MI_D = 25,
  V = 26,
  MM = 27,
  MI_RespOk = 28,
  SM = 29,
  MI = 30,
  SI = 31
};
typedef ap_uint<5> coherence_state_t;

enum EventEnum {
  NONE = 0,
  Load = 1,
  Store = 2,
  Replacement = 3,
  FwdGetS = 4,
  FwdGetM = 5,
  Inv = 6,
  DataFromDirNoAck = 7,
  DataFromDirAck = 8,
  DataFromDirLastAck = 9,
  InvAck = 10,
  LastInvAck = 11,
  DataFromOwner = 12,
  DirGetS = 13,
  DirGetM = 14,
  DirLastPutS = 15,
  DirInvAck = 16,
  DirLastInvAck = 17,
  DirPutS = 18,
  DirPutM = 19,
  DirPutMFromNonOwner = 20,
  DirPutMFromOwner = 21,
  DirFwdAck = 22,
  DataFromNonOwner = 23,
  DirDataResp = 24,
  DirRespOk = 25
};
typedef ap_uint<5> event_t;

enum RequestEnum {
  LD = 0,
  ST = 1,
  AMO = 2,
  GetM = 3,
  GetS = 4,
  PutM = 5,
  PutS = 6,
  Data = 7,
  Invalidate = 8,
  Ack = 9,
  PutAck = 10,
  FwdAck = 11,
  RespOk = 12
};
typedef ap_uint<4> request_t;

enum NetworkEnum {
  CoreNetwork = 0,
  ReqNetwork = 1,
  FwdNetwork = 2,
  RespNetwork = 3
};
typedef ap_uint<2> network_t;

enum DestinationEnum {
  None = 0,
  DestFromMessage = 1,
  DestFromPR = 2
};
typedef ap_uint<2> respdest_t;


struct MSIProtocolPrCacheControl {
  bool blockage_block = false;
  bool blockage_queue_req = false;
  bool blockage_unblock = false;
  bool blockage_unqueue_req = false;
  bool data_insert = false;
  bool mshr_fulfill_all = false;
  bool mshr_insert = false;
  bool pr_increase_ack = false;
  bool pr_insert = false;
  bool pr_migrate_from_tag = false;
  bool pr_remove = false;
  bool pr_update_pending_acks = false;
  bool replay_insert = false;
  bool replay_send = false;
  bool to_core_respond = false;
  bool to_noc_also_to_dir = false;
  respdest_t to_noc_destination;
  request_t to_noc_message;
  network_t to_noc_network;
  bool to_noc_send = false;
  bool to_noc_with_current_data = false;
  bool update_tag_clean = false;
  bool update_tag_dirty = false;
  bool update_tag_inc_ack = false;
  bool update_tag_insert = false;
  coherence_state_t update_tag_next_state;
  bool update_tag_pr_data_valid = false;
  bool update_tag_record_forward = false;
  bool update_tag_remove = false;
  bool update_tag_set_ack = false;
  bool update_tag_update = false;
  bool update_tag_update_state = false;
};

struct MSIProtocolDirControl {
  bool update_tag_remove = false;
  bool dram_data_from_req = false;
  bool to_noc_to_sharer = false;
  bool send_resp_to_requestor_with_data_valid = false;
  bool blockage_block = false;
  bool blockage_set_fwd_ack_one = false;
  coherence_state_t update_tag_next_state;
  network_t to_noc_network;
  bool data_update = false;
  bool remove_tag = false;
  bool data_data_from_dram = false;
  bool update_tag_remove_requestor_from_sharer = false;
  bool data_data_from_req = false;
  bool update_tag_update_state = false;
  bool dram_write = false;
  bool update_tag_mark_requestor_as_owner = false;
  bool to_noc_data_from_dram = false;
  bool update_tag_dirty = false;
  bool update_tag_update = false;
  bool blockage_queue_req = false;
  bool update_tag_insert = false;
  bool dram_data_from_llc = false;
  bool update_tag_transition_to_shared = false;
  bool to_noc_mark_forward_to_requestor = false;
  bool data_insert = false;
  bool to_noc_no_ack = false;
  bool to_noc_send = false;
  bool blockage_unqueue_req = false;
  request_t to_noc_message;
  bool to_noc_sharer_ack = false;
  bool dram_data_from_dram = false;
  bool to_noc_copy_owner = false;
  bool to_noc_data_from_llc = false;
  bool send_resp_to_requestor_with_data_sharer_ack = false;
  bool blockage_set_fwd_ack_sharer = false;
  bool dram_read = false;
  bool update_tag_add_requestor_as_sharer = false;
  bool to_noc_to_requestor = false;
  bool to_noc_mark_forward_to_dir = false;
  bool to_noc_to_owner = false;
  bool send_resp_to_requestor_with_data_no_ack = false;
  bool blockage_unblock = false;
  bool data_remove = false;
  bool update_tag_clean = false;
};


#endif
