if ((event == DirPutS) && (state == MS_A)) {
  DBG("DirPutS, MS_A\n");
  ctrl.update_tag_remove_requestor_from_sharer = true;
  ctrl.update_tag_update = true;
} else if ((event == DirGetM) && (state == I)) {
  DBG("DirGetM, I\n");
  ctrl.update_tag_next_state = M;
  ctrl.to_noc_network = RespNetwork;
  ctrl.data_data_from_dram = true;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_mark_requestor_as_owner = true;
  ctrl.to_noc_data_from_dram = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_insert = true;
  ctrl.data_insert = true;
  ctrl.to_noc_no_ack = true;
  ctrl.to_noc_send = true;
  ctrl.to_noc_message = Data;
  ctrl.dram_read = true;
  ctrl.to_noc_to_requestor = true;
} else if ((event == DirGetM) && (state == S)) {
  DBG("DirGetM, S\n");
  ctrl.to_noc_to_sharer = true;
  ctrl.send_resp_to_requestor_with_data_valid = true;
  ctrl.blockage_block = true;
  ctrl.update_tag_next_state = SM;
  ctrl.to_noc_network = FwdNetwork;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_mark_requestor_as_owner = true;
  ctrl.update_tag_update = true;
  ctrl.blockage_queue_req = true;
  ctrl.to_noc_mark_forward_to_requestor = true;
  ctrl.to_noc_no_ack = true;
  ctrl.to_noc_send = true;
  ctrl.to_noc_message = Invalidate;
  ctrl.send_resp_to_requestor_with_data_sharer_ack = true;
} else if ((event == DirRespOk) && (state == MM)) {
  DBG("DirRespOk, MM\n");
  ctrl.update_tag_next_state = M;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_update = true;
  ctrl.blockage_unqueue_req = true;
  ctrl.blockage_unblock = true;
} else if ((event == DirRespOk) && (state == MI_AD)) {
  DBG("DirRespOk, MI_AD\n");
  ctrl.update_tag_next_state = MI_D;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_update = true;
} else if ((event == DirGetS) && (state == S)) {
  DBG("DirGetS, S\n");
  ctrl.to_noc_network = RespNetwork;
  ctrl.update_tag_update = true;
  ctrl.to_noc_no_ack = true;
  ctrl.to_noc_send = true;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_data_from_llc = true;
  ctrl.update_tag_add_requestor_as_sharer = true;
  ctrl.to_noc_to_requestor = true;
} else if ((event == DirPutS) && (state == MS_AD)) {
  DBG("DirPutS, MS_AD\n");
  ctrl.update_tag_remove_requestor_from_sharer = true;
  ctrl.update_tag_update = true;
} else if ((event == DirRespOk) && (state == MI_RespOk)) {
  DBG("DirRespOk, MI_RespOk\n");
  ctrl.update_tag_remove = true;
  ctrl.update_tag_update = true;
  ctrl.blockage_unqueue_req = true;
  ctrl.blockage_unblock = true;
} else if ((event == DirPutS) && (state == SI)) {
  DBG("DirPutS, SI\n");
  ctrl.update_tag_remove_requestor_from_sharer = true;
  ctrl.update_tag_update = true;
} else if ((event == DirPutMFromNonOwner) && (state == MM)) {
  DBG("DirPutMFromNonOwner, MM\n");
  ctrl.update_tag_remove = true;
  ctrl.data_update = true;
  ctrl.data_data_from_req = true;
  ctrl.update_tag_update = true;
  ctrl.blockage_unblock = true;
} else if ((event == Replacement) && (state == M)) {
  DBG("Replacement, M\n");
  ctrl.blockage_block = true;
  ctrl.update_tag_next_state = MI;
  ctrl.to_noc_network = FwdNetwork;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_update = true;
  ctrl.blockage_queue_req = true;
  ctrl.to_noc_no_ack = true;
  ctrl.to_noc_send = true;
  ctrl.to_noc_message = GetM;
  ctrl.to_noc_mark_forward_to_dir = true;
  ctrl.to_noc_to_owner = true;
} else if ((event == DirPutMFromOwner) && (state == SM)) {
  DBG("DirPutMFromOwner, SM\n");
  ctrl.update_tag_next_state = MI_RespOk;
  ctrl.data_update = true;
  ctrl.data_data_from_req = true;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_update = true;
} else if ((event == Replacement) && (state == S)) {
  DBG("Replacement, S\n");
  ctrl.to_noc_to_sharer = true;
  ctrl.blockage_block = true;
  ctrl.update_tag_next_state = SI;
  ctrl.to_noc_network = FwdNetwork;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_update = true;
  ctrl.blockage_queue_req = true;
  ctrl.to_noc_no_ack = true;
  ctrl.to_noc_send = true;
  ctrl.to_noc_message = Invalidate;
  ctrl.to_noc_mark_forward_to_dir = true;
} else if ((event == DirGetS) && (state == I)) {
  DBG("DirGetS, I\n");
  ctrl.update_tag_next_state = S;
  ctrl.to_noc_network = RespNetwork;
  ctrl.data_data_from_dram = true;
  ctrl.update_tag_update_state = true;
  ctrl.to_noc_data_from_dram = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_insert = true;
  ctrl.data_insert = true;
  ctrl.to_noc_no_ack = true;
  ctrl.to_noc_send = true;
  ctrl.to_noc_message = Data;
  ctrl.dram_read = true;
  ctrl.update_tag_add_requestor_as_sharer = true;
  ctrl.to_noc_to_requestor = true;
} else if ((event == DirPutS) && (state == S)) {
  DBG("DirPutS, S\n");
  ctrl.update_tag_remove_requestor_from_sharer = true;
  ctrl.update_tag_update = true;
} else if ((event == DirGetM) && (state == M)) {
  DBG("DirGetM, M\n");
  ctrl.blockage_block = true;
  ctrl.update_tag_next_state = MM;
  ctrl.to_noc_network = FwdNetwork;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_mark_requestor_as_owner = true;
  ctrl.update_tag_update = true;
  ctrl.blockage_queue_req = true;
  ctrl.to_noc_mark_forward_to_requestor = true;
  ctrl.to_noc_no_ack = true;
  ctrl.to_noc_send = true;
  ctrl.to_noc_message = GetM;
  ctrl.to_noc_to_owner = true;
} else if ((event == DirPutMFromOwner) && (state == MM)) {
  DBG("DirPutMFromOwner, MM\n");
  ctrl.update_tag_next_state = MI_RespOk;
  ctrl.data_update = true;
  ctrl.data_data_from_req = true;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_update = true;
} else if ((event == DirRespOk) && (state == SM)) {
  DBG("DirRespOk, SM\n");
  ctrl.update_tag_next_state = M;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_update = true;
  ctrl.blockage_unqueue_req = true;
  ctrl.blockage_unblock = true;
} else if ((event == DirGetM) && (state == V)) {
  DBG("DirGetM, V\n");
  ctrl.update_tag_next_state = M;
  ctrl.to_noc_network = RespNetwork;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_mark_requestor_as_owner = true;
  ctrl.to_noc_data_from_dram = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_insert = true;
  ctrl.to_noc_no_ack = true;
  ctrl.to_noc_send = true;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_to_requestor = true;
} else if ((event == DirRespOk) && (state == MI_A)) {
  DBG("DirRespOk, MI_A\n");
  ctrl.update_tag_remove = true;
  ctrl.update_tag_update = true;
  ctrl.blockage_unqueue_req = true;
  ctrl.blockage_unblock = true;
} else if ((event == DirPutMFromOwner) && (state == MI)) {
  DBG("DirPutMFromOwner, MI\n");
  ctrl.update_tag_remove = true;
  ctrl.dram_data_from_req = true;
  ctrl.dram_write = true;
  ctrl.update_tag_update = true;
  ctrl.blockage_unblock = true;
  ctrl.data_remove = true;
} else if ((event == DirLastPutS) && (state == SM)) {
  DBG("DirLastPutS, SM\n");
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_no_ack = true;
  ctrl.to_noc_send = true;
  ctrl.to_noc_message = Ack;
  ctrl.to_noc_to_owner = true;
} else if ((event == DirGetS) && (state == M)) {
  DBG("DirGetS, M\n");
  ctrl.blockage_block = true;
  ctrl.blockage_set_fwd_ack_one = true;
  ctrl.update_tag_next_state = MS_AD;
  ctrl.to_noc_network = FwdNetwork;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_update = true;
  ctrl.blockage_queue_req = true;
  ctrl.update_tag_transition_to_shared = true;
  ctrl.to_noc_mark_forward_to_requestor = true;
  ctrl.to_noc_no_ack = true;
  ctrl.to_noc_send = true;
  ctrl.to_noc_message = GetS;
  ctrl.to_noc_to_owner = true;
} else if ((event == DirDataResp) && (state == MS_D)) {
  DBG("DirDataResp, MS_D\n");
  ctrl.update_tag_next_state = S;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_update = true;
  ctrl.blockage_unqueue_req = true;
  ctrl.blockage_unblock = true;
} else if ((event == DirLastPutS) && (state == S)) {
  DBG("DirLastPutS, S\n");
  ctrl.update_tag_remove = true;
  ctrl.update_tag_update = true;
} else if ((event == DirDataResp) && (state == MI_D)) {
  DBG("DirDataResp, MI_D\n");
  ctrl.update_tag_remove = true;
  ctrl.update_tag_update = true;
  ctrl.blockage_unqueue_req = true;
  ctrl.blockage_unblock = true;
} else if ((event == DirGetS) && (state == V)) {
  DBG("DirGetS, V\n");
  ctrl.update_tag_next_state = S;
  ctrl.to_noc_network = RespNetwork;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_insert = true;
  ctrl.to_noc_no_ack = true;
  ctrl.to_noc_send = true;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_data_from_llc = true;
  ctrl.update_tag_add_requestor_as_sharer = true;
  ctrl.to_noc_to_requestor = true;
} else if ((event == DirInvAck) && (state == SI)) {
  DBG("DirInvAck, SI\n");
  ctrl.update_tag_remove_requestor_from_sharer = true;
  ctrl.update_tag_update = true;
} else if ((event == Replacement) && (state == V)) {
  DBG("Replacement, V\n");
  ctrl.dram_write = true;
  ctrl.blockage_queue_req = true;
  ctrl.dram_data_from_llc = true;
  ctrl.data_remove = true;
} else if ((event == DirLastPutS) && (state == MS_AD)) {
  DBG("DirLastPutS, MS_AD\n");
  ctrl.update_tag_next_state = MI_AD;
  ctrl.update_tag_remove_requestor_from_sharer = true;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_update = true;
} else if ((event == DirDataResp) && (state == MI_AD)) {
  DBG("DirDataResp, MI_AD\n");
  ctrl.update_tag_next_state = MI_A;
  ctrl.data_update = true;
  ctrl.data_data_from_req = true;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_update = true;
} else if ((event == DirLastPutS) && (state == MS_A)) {
  DBG("DirLastPutS, MS_A\n");
  ctrl.update_tag_next_state = MI_RespOk;
  ctrl.update_tag_remove_requestor_from_sharer = true;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_update = true;
} else if ((event == DirPutS) && (state == SM)) {
  DBG("DirPutS, SM\n");
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_no_ack = true;
  ctrl.to_noc_send = true;
  ctrl.to_noc_message = Ack;
  ctrl.to_noc_to_owner = true;
} else if ((event == DirDataResp) && (state == MS_AD)) {
  DBG("DirDataResp, MS_AD\n");
  ctrl.update_tag_next_state = MS_A;
  ctrl.data_update = true;
  ctrl.data_data_from_req = true;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_update = true;
} else if ((event == DirRespOk) && (state == MS_A)) {
  DBG("DirRespOk, MS_A\n");
  ctrl.update_tag_next_state = S;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_update = true;
  ctrl.blockage_unqueue_req = true;
  ctrl.blockage_unblock = true;
} else if ((event == DirRespOk) && (state == MS_AD)) {
  DBG("DirRespOk, MS_AD\n");
  ctrl.update_tag_next_state = MS_D;
  ctrl.update_tag_update_state = true;
  ctrl.update_tag_update = true;
} else if ((event == DirPutMFromNonOwner) && (state == MS_AD)) {
  DBG("DirPutMFromNonOwner, MS_AD\n");
  ctrl.update_tag_remove = true;
  ctrl.data_update = true;
  ctrl.data_data_from_req = true;
  ctrl.update_tag_update = true;
  ctrl.blockage_unblock = true;
} else if ((event == DirPutMFromOwner) && (state == M)) {
  DBG("DirPutMFromOwner, M\n");
  ctrl.update_tag_remove = true;
  ctrl.data_update = true;
  ctrl.data_data_from_req = true;
  ctrl.update_tag_update = true;
} else if ((event == DirDataResp) && (state == MI)) {
  DBG("DirDataResp, MI\n");
  ctrl.update_tag_remove = true;
  ctrl.dram_data_from_req = true;
  ctrl.dram_write = true;
  ctrl.update_tag_update = true;
  ctrl.blockage_unblock = true;
  ctrl.data_remove = true;
} else if ((event == DirLastPutS) && (state == SI)) {
  DBG("DirLastPutS, SI\n");
  ctrl.update_tag_remove = true;
  ctrl.dram_write = true;
  ctrl.update_tag_update = true;
  ctrl.dram_data_from_llc = true;
  ctrl.blockage_unblock = true;
  ctrl.data_remove = true;
} else if ((event == DirLastInvAck) && (state == SI)) {
  DBG("DirLastInvAck, SI\n");
  ctrl.update_tag_remove = true;
  ctrl.dram_write = true;
  ctrl.update_tag_update = true;
  ctrl.dram_data_from_llc = true;
  ctrl.blockage_unblock = true;
  ctrl.data_remove = true;
}