if ((event == LastInvAck) && (state == SM_A_S)) {
  DBG("LastInvAck, SM_A_S\n");
  ctrl.data_insert = true;
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_also_to_dir = true;
  ctrl.to_noc_destination = DestFromPR;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.to_noc_with_current_data = true;
  ctrl.update_tag_insert = true;
  ctrl.update_tag_next_state = S;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == DataFromDirNoAck) && (state == SM_AD_S)) {
  DBG("DataFromDirNoAck, SM_AD_S\n");
  ctrl.data_insert = true;
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_also_to_dir = true;
  ctrl.to_noc_destination = DestFromPR;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.to_noc_with_current_data = true;
  ctrl.update_tag_insert = true;
  ctrl.update_tag_next_state = S;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == DataFromDirNoAck) && (state == SM_AD_I)) {
  DBG("DataFromDirNoAck, SM_AD_I\n");
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_destination = DestFromPR;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.to_noc_with_current_data = true;
} else if ((event == InvAck) && (state == SM_AD_S_I)) {
  DBG("InvAck, SM_AD_S_I\n");
  ctrl.update_tag_inc_ack = true;
  ctrl.update_tag_update = true;
} else if ((event == Load) && (state == M)) {
  DBG("Load, M\n");
  ctrl.to_core_respond = true;
} else if ((event == FwdGetM) && (state == IM_AD)) {
  DBG("FwdGetM, IM_AD\n");
  ctrl.update_tag_next_state = IM_AD_I;
  ctrl.update_tag_record_forward = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == InvAck) && (state == SM_A)) {
  DBG("InvAck, SM_A\n");
  ctrl.update_tag_inc_ack = true;
  ctrl.update_tag_update = true;
} else if ((event == DataFromDirLastAck) && (state == SM_AD_S_I)) {
  DBG("DataFromDirLastAck, SM_AD_S_I\n");
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_also_to_dir = true;
  ctrl.to_noc_destination = DestFromPR;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.to_noc_with_current_data = true;
} else if ((event == LastInvAck) && (state == IM_A_S)) {
  DBG("LastInvAck, IM_A_S\n");
  ctrl.data_insert = true;
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_also_to_dir = true;
  ctrl.to_noc_destination = DestFromPR;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.to_noc_with_current_data = true;
  ctrl.update_tag_insert = true;
  ctrl.update_tag_next_state = S;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == DataFromDirLastAck) && (state == SM_AD_I)) {
  DBG("DataFromDirLastAck, SM_AD_I\n");
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_destination = DestFromPR;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.to_noc_with_current_data = true;
} else if ((event == InvAck) && (state == SM_A_I)) {
  DBG("InvAck, SM_A_I\n");
  ctrl.update_tag_inc_ack = true;
  ctrl.update_tag_update = true;
} else if ((event == DataFromDirAck) && (state == SM_AD)) {
  DBG("DataFromDirAck, SM_AD\n");
  ctrl.data_insert = true;
  ctrl.update_tag_next_state = SM_A;
  ctrl.update_tag_set_ack = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == DataFromDirNoAck) && (state == IM_AD_S)) {
  DBG("DataFromDirNoAck, IM_AD_S\n");
  ctrl.data_insert = true;
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_also_to_dir = true;
  ctrl.to_noc_destination = DestFromPR;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.to_noc_with_current_data = true;
  ctrl.update_tag_insert = true;
  ctrl.update_tag_next_state = S;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == LastInvAck) && (state == IM_A)) {
  DBG("LastInvAck, IM_A\n");
  ctrl.data_insert = true;
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_also_to_dir = true;
  ctrl.to_noc_destination = None;
  ctrl.to_noc_message = RespOk;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.update_tag_dirty = true;
  ctrl.update_tag_insert = true;
  ctrl.update_tag_next_state = M;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == InvAck) && (state == IM_AD)) {
  DBG("InvAck, IM_AD\n");
  ctrl.update_tag_inc_ack = true;
  ctrl.update_tag_update = true;
} else if ((event == DataFromDirAck) && (state == SM_AD_S_I)) {
  DBG("DataFromDirAck, SM_AD_S_I\n");
  ctrl.data_insert = true;
  ctrl.update_tag_next_state = SM_A_S_I;
  ctrl.update_tag_set_ack = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == DataFromOwner) && (state == SM_AD)) {
  DBG("DataFromOwner, SM_AD\n");
  ctrl.data_insert = true;
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_also_to_dir = true;
  ctrl.to_noc_destination = None;
  ctrl.to_noc_message = RespOk;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.update_tag_dirty = true;
  ctrl.update_tag_insert = true;
  ctrl.update_tag_next_state = M;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == DataFromDirNoAck) && (state == SM_AD_S_I)) {
  DBG("DataFromDirNoAck, SM_AD_S_I\n");
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_also_to_dir = true;
  ctrl.to_noc_destination = DestFromPR;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.to_noc_with_current_data = true;
} else if ((event == DataFromDirLastAck) && (state == IM_AD)) {
  DBG("DataFromDirLastAck, IM_AD\n");
  ctrl.data_insert = true;
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_also_to_dir = true;
  ctrl.to_noc_destination = None;
  ctrl.to_noc_message = RespOk;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.update_tag_dirty = true;
  ctrl.update_tag_insert = true;
  ctrl.update_tag_next_state = M;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == DataFromDirNoAck) && (state == IS_D)) {
  DBG("DataFromDirNoAck, IS_D\n");
  ctrl.data_insert = true;
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.update_tag_clean = true;
  ctrl.update_tag_insert = true;
  ctrl.update_tag_next_state = S;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == DataFromDirLastAck) && (state == SM_AD)) {
  DBG("DataFromDirLastAck, SM_AD\n");
  ctrl.data_insert = true;
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_also_to_dir = true;
  ctrl.to_noc_destination = None;
  ctrl.to_noc_message = RespOk;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.update_tag_dirty = true;
  ctrl.update_tag_insert = true;
  ctrl.update_tag_next_state = M;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == LastInvAck) && (state == SM_A_I)) {
  DBG("LastInvAck, SM_A_I\n");
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_destination = DestFromPR;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.to_noc_with_current_data = true;
} else if ((event == DataFromDirLastAck) && (state == IM_AD_I)) {
  DBG("DataFromDirLastAck, IM_AD_I\n");
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_destination = DestFromPR;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.to_noc_with_current_data = true;
} else if ((event == FwdGetM) && (state == SM_AD)) {
  DBG("FwdGetM, SM_AD\n");
  ctrl.update_tag_next_state = SM_AD_I;
  ctrl.update_tag_record_forward = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == Inv) && (state == I)) {
  DBG("Inv, I\n");
  
} else if ((event == Store) && (state == M)) {
  DBG("Store, M\n");
  ctrl.to_core_respond = true;
} else if ((event == DataFromDirAck) && (state == IM_AD_I)) {
  DBG("DataFromDirAck, IM_AD_I\n");
  ctrl.data_insert = true;
  ctrl.update_tag_next_state = IM_A_I;
  ctrl.update_tag_set_ack = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == Inv) && (state == SM_AD)) {
  DBG("Inv, SM_AD\n");
  ctrl.to_noc_destination = DestFromMessage;
  ctrl.to_noc_message = Ack;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.update_tag_next_state = IM_AD;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == FwdGetS) && (state == SM_AD)) {
  DBG("FwdGetS, SM_AD\n");
  ctrl.update_tag_next_state = SM_AD_S;
  ctrl.update_tag_record_forward = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == InvAck) && (state == SM_AD)) {
  DBG("InvAck, SM_AD\n");
  ctrl.update_tag_inc_ack = true;
  ctrl.update_tag_update = true;
} else if ((event == FwdGetS) && (state == M)) {
  DBG("FwdGetS, M\n");
  ctrl.to_noc_also_to_dir = true;
  ctrl.to_noc_destination = DestFromMessage;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.to_noc_with_current_data = true;
  ctrl.update_tag_clean = true;
  ctrl.update_tag_next_state = S;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == InvAck) && (state == SM_A_S_I)) {
  DBG("InvAck, SM_A_S_I\n");
  ctrl.update_tag_inc_ack = true;
  ctrl.update_tag_update = true;
} else if ((event == InvAck) && (state == IM_A_I)) {
  DBG("InvAck, IM_A_I\n");
  ctrl.update_tag_inc_ack = true;
  ctrl.update_tag_update = true;
} else if ((event == Inv) && (state == SM_A_S)) {
  DBG("Inv, SM_A_S\n");
  ctrl.to_noc_destination = DestFromMessage;
  ctrl.to_noc_message = Ack;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.update_tag_next_state = SM_A_S_I;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == LastInvAck) && (state == IM_A_I)) {
  DBG("LastInvAck, IM_A_I\n");
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_destination = DestFromPR;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.to_noc_with_current_data = true;
} else if ((event == InvAck) && (state == IM_AD_S)) {
  DBG("InvAck, IM_AD_S\n");
  ctrl.update_tag_inc_ack = true;
  ctrl.update_tag_update = true;
} else if ((event == FwdGetS) && (state == I)) {
  DBG("FwdGetS, I\n");
  
} else if ((event == Store) && (state == I)) {
  DBG("Store, I\n");
  ctrl.mshr_insert = true;
  ctrl.pr_insert = true;
  ctrl.to_noc_destination = None;
  ctrl.to_noc_message = GetM;
  ctrl.to_noc_network = ReqNetwork;
  ctrl.to_noc_send = true;
  ctrl.update_tag_next_state = IM_AD;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == DataFromDirNoAck) && (state == SM_AD)) {
  DBG("DataFromDirNoAck, SM_AD\n");
  ctrl.data_insert = true;
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_also_to_dir = true;
  ctrl.to_noc_destination = None;
  ctrl.to_noc_message = RespOk;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.update_tag_dirty = true;
  ctrl.update_tag_insert = true;
  ctrl.update_tag_next_state = M;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == DataFromDirAck) && (state == SM_AD_I)) {
  DBG("DataFromDirAck, SM_AD_I\n");
  ctrl.data_insert = true;
  ctrl.update_tag_next_state = SM_A_I;
  ctrl.update_tag_set_ack = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == Load) && (state == S)) {
  DBG("Load, S\n");
  ctrl.to_core_respond = true;
} else if ((event == FwdGetM) && (state == I)) {
  DBG("FwdGetM, I\n");
  
} else if ((event == Store) && (state == S)) {
  DBG("Store, S\n");
  ctrl.mshr_insert = true;
  ctrl.pr_insert = true;
  ctrl.pr_migrate_from_tag = true;
  ctrl.to_noc_destination = None;
  ctrl.to_noc_message = GetM;
  ctrl.to_noc_network = ReqNetwork;
  ctrl.to_noc_send = true;
  ctrl.update_tag_next_state = SM_AD;
  ctrl.update_tag_remove = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == DataFromDirLastAck) && (state == IM_AD_S)) {
  DBG("DataFromDirLastAck, IM_AD_S\n");
  ctrl.data_insert = true;
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_also_to_dir = true;
  ctrl.to_noc_destination = DestFromPR;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.to_noc_with_current_data = true;
  ctrl.update_tag_insert = true;
  ctrl.update_tag_next_state = S;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == DataFromDirAck) && (state == SM_AD_S)) {
  DBG("DataFromDirAck, SM_AD_S\n");
  ctrl.data_insert = true;
  ctrl.update_tag_next_state = SM_A_S;
  ctrl.update_tag_set_ack = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == Inv) && (state == S)) {
  DBG("Inv, S\n");
  ctrl.to_noc_destination = DestFromMessage;
  ctrl.to_noc_message = Ack;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.update_tag_remove = true;
  ctrl.update_tag_update = true;
} else if ((event == Load) && (state == I)) {
  DBG("Load, I\n");
  ctrl.mshr_insert = true;
  ctrl.pr_insert = true;
  ctrl.to_noc_destination = None;
  ctrl.to_noc_message = GetS;
  ctrl.to_noc_network = ReqNetwork;
  ctrl.to_noc_send = true;
  ctrl.update_tag_next_state = IS_D;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == InvAck) && (state == SM_A_S)) {
  DBG("InvAck, SM_A_S\n");
  ctrl.update_tag_inc_ack = true;
  ctrl.update_tag_update = true;
} else if ((event == FwdGetS) && (state == IM_AD)) {
  DBG("FwdGetS, IM_AD\n");
  ctrl.update_tag_next_state = IM_AD_S;
  ctrl.update_tag_record_forward = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == DataFromDirNoAck) && (state == IM_AD_I)) {
  DBG("DataFromDirNoAck, IM_AD_I\n");
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_destination = DestFromPR;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.to_noc_with_current_data = true;
} else if ((event == LastInvAck) && (state == SM_A_S_I)) {
  DBG("LastInvAck, SM_A_S_I\n");
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_also_to_dir = true;
  ctrl.to_noc_destination = DestFromPR;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.to_noc_with_current_data = true;
} else if ((event == DataFromDirLastAck) && (state == SM_AD_S)) {
  DBG("DataFromDirLastAck, SM_AD_S\n");
  ctrl.data_insert = true;
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_also_to_dir = true;
  ctrl.to_noc_destination = DestFromPR;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.to_noc_with_current_data = true;
  ctrl.update_tag_insert = true;
  ctrl.update_tag_next_state = S;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == FwdGetS) && (state == IM_A)) {
  DBG("FwdGetS, IM_A\n");
  ctrl.update_tag_next_state = IM_A_S;
  ctrl.update_tag_record_forward = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == DataFromDirNoAck) && (state == IM_AD)) {
  DBG("DataFromDirNoAck, IM_AD\n");
  ctrl.data_insert = true;
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.update_tag_dirty = true;
  ctrl.update_tag_insert = true;
  ctrl.update_tag_next_state = M;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == FwdGetM) && (state == SM_A)) {
  DBG("FwdGetM, SM_A\n");
  ctrl.update_tag_next_state = SM_A_I;
  ctrl.update_tag_record_forward = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == FwdGetM) && (state == IM_A)) {
  DBG("FwdGetM, IM_A\n");
  ctrl.update_tag_next_state = IM_A_I;
  ctrl.update_tag_record_forward = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == DataFromDirAck) && (state == IM_AD)) {
  DBG("DataFromDirAck, IM_AD\n");
  ctrl.data_insert = true;
  ctrl.update_tag_next_state = IM_A;
  ctrl.update_tag_set_ack = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == DataFromDirNoAck) && (state == IS_DI)) {
  DBG("DataFromDirNoAck, IS_DI\n");
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
} else if ((event == LastInvAck) && (state == SM_A)) {
  DBG("LastInvAck, SM_A\n");
  ctrl.data_insert = true;
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_also_to_dir = true;
  ctrl.to_noc_destination = None;
  ctrl.to_noc_message = RespOk;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.update_tag_dirty = true;
  ctrl.update_tag_insert = true;
  ctrl.update_tag_next_state = M;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == InvAck) && (state == IM_AD_I)) {
  DBG("InvAck, IM_AD_I\n");
  ctrl.update_tag_inc_ack = true;
  ctrl.update_tag_update = true;
} else if ((event == FwdGetS) && (state == SM_A)) {
  DBG("FwdGetS, SM_A\n");
  ctrl.update_tag_next_state = SM_A_S;
  ctrl.update_tag_record_forward = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == Inv) && (state == SM_AD_S)) {
  DBG("Inv, SM_AD_S\n");
  ctrl.to_noc_destination = DestFromMessage;
  ctrl.to_noc_message = Ack;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.update_tag_next_state = SM_AD_S_I;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == InvAck) && (state == IM_A)) {
  DBG("InvAck, IM_A\n");
  ctrl.update_tag_inc_ack = true;
  ctrl.update_tag_update = true;
} else if ((event == DataFromDirAck) && (state == IM_AD_S)) {
  DBG("DataFromDirAck, IM_AD_S\n");
  ctrl.data_insert = true;
  ctrl.update_tag_next_state = IM_A_S;
  ctrl.update_tag_set_ack = true;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == InvAck) && (state == SM_AD_S)) {
  DBG("InvAck, SM_AD_S\n");
  ctrl.update_tag_inc_ack = true;
  ctrl.update_tag_update = true;
} else if ((event == Inv) && (state == IS_D)) {
  DBG("Inv, IS_D\n");
  ctrl.to_noc_destination = DestFromMessage;
  ctrl.to_noc_message = Ack;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.update_tag_next_state = IS_DI;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == InvAck) && (state == SM_AD_I)) {
  DBG("InvAck, SM_AD_I\n");
  ctrl.update_tag_inc_ack = true;
  ctrl.update_tag_update = true;
} else if ((event == FwdGetM) && (state == M)) {
  DBG("FwdGetM, M\n");
  ctrl.to_noc_destination = DestFromMessage;
  ctrl.to_noc_message = Data;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.to_noc_with_current_data = true;
  ctrl.update_tag_remove = true;
  ctrl.update_tag_update = true;
} else if ((event == InvAck) && (state == IM_A_S)) {
  DBG("InvAck, IM_A_S\n");
  ctrl.update_tag_inc_ack = true;
  ctrl.update_tag_update = true;
} else if ((event == DataFromOwner) && (state == IM_AD)) {
  DBG("DataFromOwner, IM_AD\n");
  ctrl.data_insert = true;
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_also_to_dir = true;
  ctrl.to_noc_destination = None;
  ctrl.to_noc_message = RespOk;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.update_tag_dirty = true;
  ctrl.update_tag_insert = true;
  ctrl.update_tag_next_state = M;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
} else if ((event == DataFromOwner) && (state == IS_D)) {
  DBG("DataFromOwner, IS_D\n");
  ctrl.data_insert = true;
  ctrl.mshr_fulfill_all = true;
  ctrl.pr_remove = true;
  ctrl.to_noc_also_to_dir = true;
  ctrl.to_noc_destination = None;
  ctrl.to_noc_message = RespOk;
  ctrl.to_noc_network = RespNetwork;
  ctrl.to_noc_send = true;
  ctrl.update_tag_clean = true;
  ctrl.update_tag_insert = true;
  ctrl.update_tag_next_state = S;
  ctrl.update_tag_update = true;
  ctrl.update_tag_update_state = true;
}