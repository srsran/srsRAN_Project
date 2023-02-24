/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_cu_test_messages.h"

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::f1ap;

asn1::f1ap::gnb_du_served_cells_item_s srsran::srs_cu_cp::generate_served_cells_item(unsigned nrcell_id, unsigned nrpci)
{
  asn1::f1ap::gnb_du_served_cells_item_s served_cells_item;
  served_cells_item.served_cell_info.nr_cgi.plmn_id.from_string("00f110");
  served_cells_item.served_cell_info.nr_cgi.nr_cell_id.from_number(nrcell_id);
  served_cells_item.served_cell_info.nr_pci              = nrpci;
  served_cells_item.served_cell_info.five_gs_tac_present = true;
  served_cells_item.served_cell_info.five_gs_tac.from_number(7);

  asn1::f1ap::served_plmns_item_s served_plmn;
  served_plmn.plmn_id.from_string("00f110");
  asn1::protocol_ext_field_s<asn1::f1ap::served_plmns_item_ext_ies_o> plmn_ext_container = {};
  plmn_ext_container.set_item(ASN1_F1AP_ID_TAI_SLICE_SUPPORT_LIST);
  auto&                            tai_slice_support_list = plmn_ext_container.value().tai_slice_support_list();
  asn1::f1ap::slice_support_item_s slice_support_item;
  slice_support_item.snssai.sst.from_number(1);
  tai_slice_support_list.push_back(slice_support_item);
  served_plmn.ie_exts.push_back(plmn_ext_container);
  served_cells_item.served_cell_info.served_plmns.push_back(served_plmn);

  served_cells_item.served_cell_info.nr_mode_info.set_tdd();
  served_cells_item.served_cell_info.nr_mode_info.tdd().nr_freq_info.nr_arfcn = 626748;
  asn1::f1ap::freq_band_nr_item_s freq_band_nr_item;
  freq_band_nr_item.freq_band_ind_nr = 78;
  served_cells_item.served_cell_info.nr_mode_info.tdd().nr_freq_info.freq_band_list_nr.push_back(freq_band_nr_item);
  served_cells_item.served_cell_info.nr_mode_info.tdd().tx_bw.nr_scs.value = asn1::f1ap::nr_scs_opts::scs30;
  served_cells_item.served_cell_info.nr_mode_info.tdd().tx_bw.nr_nrb.value = asn1::f1ap::nr_nrb_opts::nrb51;
  served_cells_item.served_cell_info.meas_timing_cfg.from_string("30");

  served_cells_item.gnb_du_sys_info_present = true;
  served_cells_item.gnb_du_sys_info.mib_msg.from_string("01c586");
  served_cells_item.gnb_du_sys_info.sib1_msg.from_string(
      "92002808241099000001000000000a4213407800008c98d6d8d7f616e0804000020107e28180008000088a0dc7008000088a0007141a22"
      "81c874cc00020000232d5c6b6c65462001ec4cc5fc9c0493946a98d4d1e99355c00a1aba010580ec024646f62180");

  return served_cells_item;
}

f1ap_message srsran::srs_cu_cp::generate_f1_setup_request()
{
  f1ap_message msg;
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

  auto& setup_req                 = msg.pdu.init_msg().value.f1_setup_request();
  setup_req->transaction_id.value = 99;
  setup_req->gnb_du_id.value      = 0x11;
  setup_req->gnb_du_name_present  = true;
  setup_req->gnb_du_name.value.from_string("srsDU");
  setup_req->gnb_du_rrc_version.value.latest_rrc_version.from_number(1);
  setup_req->gnb_du_served_cells_list_present = true;
  setup_req->gnb_du_served_cells_list->resize(1);
  setup_req->gnb_du_served_cells_list.value[0].load_info_obj(ASN1_F1AP_ID_GNB_DU_SERVED_CELLS_ITEM);
  setup_req->gnb_du_served_cells_list.value[0].value().gnb_du_served_cells_item() =
      generate_served_cells_item(12345678, 0);

  return msg;
}

f1ap_message srsran::srs_cu_cp::generate_init_ul_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                                                      rnti_t              crnti,
                                                                      byte_buffer         cell_group_cfg)
{
  f1ap_message init_ul_rrc_msg;

  init_ul_rrc_msg.pdu.set_init_msg();
  init_ul_rrc_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_INIT_UL_RRC_MSG_TRANSFER);

  init_ul_rrc_msg_transfer_s& init_ul_rrc = init_ul_rrc_msg.pdu.init_msg().value.init_ul_rrc_msg_transfer();
  init_ul_rrc->gnb_du_ue_f1ap_id.value    = (unsigned)du_ue_id;

  init_ul_rrc->nr_cgi.value.nr_cell_id.from_string("000000000000101111000110000101001110"); // 12345678 in decimal
  init_ul_rrc->nr_cgi.value.plmn_id.from_string("02f899");
  init_ul_rrc->c_rnti.value = crnti;

  init_ul_rrc->sul_access_ind_present     = true;
  init_ul_rrc->sul_access_ind.value.value = asn1::f1ap::sul_access_ind_opts::options::true_value;

  init_ul_rrc->rrc_container.value.from_string("1dec89d05766");

  init_ul_rrc->du_to_cu_rrc_container_present = true;
  if (cell_group_cfg.empty()) {
    init_ul_rrc->du_to_cu_rrc_container.value.from_string(
        "5c00b001117aec701061e0007c20408d07810020a2090480ca8000f800000000008370842000088165000048200002069a06aa49880002"
        "00204000400d008013b64b1814400e468acf120000096070820f177e060870000000e25038000040bde802000400000000028201950300"
        "c400");
  } else {
    init_ul_rrc->du_to_cu_rrc_container.value = std::move(cell_group_cfg);
  }

  return init_ul_rrc_msg;
}

f1ap_message srsran::srs_cu_cp::generate_ul_rrc_message_transfer(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                 gnb_du_ue_f1ap_id_t du_ue_id,
                                                                 srb_id_t            srb_id,
                                                                 byte_buffer         rrc_container)
{
  f1ap_message ul_rrc_msg = {};

  ul_rrc_msg.pdu.set_init_msg();
  ul_rrc_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_UL_RRC_MSG_TRANSFER);

  auto& ul_rrc                    = ul_rrc_msg.pdu.init_msg().value.ul_rrc_msg_transfer();
  ul_rrc->gnb_du_ue_f1ap_id.value = (unsigned)du_ue_id;
  ul_rrc->gnb_cu_ue_f1ap_id.value = (unsigned)cu_ue_id;
  ul_rrc->srb_id.value            = (unsigned)srb_id;
  ul_rrc->rrc_container.value     = std::move(rrc_container);

  return ul_rrc_msg;
}

f1ap_message srsran::srs_cu_cp::generate_ue_context_release_complete(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                     gnb_du_ue_f1ap_id_t du_ue_id)
{
  f1ap_message ue_ctxt_rel_complete_msg = {};
  ue_ctxt_rel_complete_msg.pdu.set_successful_outcome();
  ue_ctxt_rel_complete_msg.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE);
  ue_context_release_complete_s& rel_complete_msg =
      ue_ctxt_rel_complete_msg.pdu.successful_outcome().value.ue_context_release_complete();
  rel_complete_msg->gnb_cu_ue_f1ap_id.value = (unsigned)cu_ue_id;
  rel_complete_msg->gnb_du_ue_f1ap_id.value = (unsigned)du_ue_id;

  return ue_ctxt_rel_complete_msg;
}

f1ap_message srsran::srs_cu_cp::generate_ue_context_setup_request(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                  gnb_du_ue_f1ap_id_t du_ue_id)
{
  f1ap_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);

  auto& ue_ctx_req                    = msg.pdu.init_msg().value.ue_context_setup_request();
  ue_ctx_req->gnb_cu_ue_f1ap_id.value = (unsigned)cu_ue_id;
  ue_ctx_req->gnb_du_ue_f1ap_id.value = (unsigned)du_ue_id;

  ue_ctx_req->srbs_to_be_setup_list_present = true;
  ue_ctx_req->srbs_to_be_setup_list.value.resize(1);
  ue_ctx_req->srbs_to_be_setup_list.value[0].load_info_obj(ASN1_F1AP_ID_SRBS_TO_BE_SETUP_ITEM);
  srbs_to_be_setup_item_s& srb2     = ue_ctx_req->srbs_to_be_setup_list.value[0].value().srbs_to_be_setup_item();
  srb2.srb_id                       = 2;
  ue_ctx_req->rrc_container_present = true;
  ue_ctx_req->rrc_container.value.from_string("000320080895755b0c");

  return msg;
}

f1ap_message srsran::srs_cu_cp::generate_ue_context_setup_response(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                   gnb_du_ue_f1ap_id_t du_ue_id,
                                                                   rnti_t              crnti)
{
  f1ap_message ue_context_setup_response = {};

  ue_context_setup_response.pdu.set_successful_outcome();
  ue_context_setup_response.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);

  auto& ue_context_setup_resp = ue_context_setup_response.pdu.successful_outcome().value.ue_context_setup_resp();
  ue_context_setup_resp->gnb_cu_ue_f1ap_id.value = (unsigned)cu_ue_id;
  ue_context_setup_resp->gnb_du_ue_f1ap_id.value = (unsigned)du_ue_id;
  ue_context_setup_resp->c_rnti_present          = true;
  ue_context_setup_resp->c_rnti.value            = (unsigned)crnti;

  return ue_context_setup_response;
}

f1ap_message srsran::srs_cu_cp::generate_ue_context_setup_failure(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                  gnb_du_ue_f1ap_id_t du_ue_id)
{
  f1ap_message ue_context_setup_failure = {};

  ue_context_setup_failure.pdu.set_unsuccessful_outcome();
  ue_context_setup_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);

  auto& ue_context_setup_fail = ue_context_setup_failure.pdu.unsuccessful_outcome().value.ue_context_setup_fail();
  ue_context_setup_fail->gnb_cu_ue_f1ap_id.value = (unsigned)cu_ue_id;
  ue_context_setup_fail->gnb_du_ue_f1ap_id.value = (unsigned)du_ue_id;
  ue_context_setup_fail->cause.value.set_radio_network();
  ue_context_setup_fail->cause.value.radio_network() =
      cause_radio_network_opts::options::unknown_or_already_allocated_gnb_cu_ue_f1ap_id;

  return ue_context_setup_failure;
}

cu_cp_ue_context_modification_request
srsran::srs_cu_cp::generate_ue_context_modification_request(ue_index_t                             ue_index,
                                                            const std::initializer_list<drb_id_t>& drbs_to_add)
{
  cu_cp_ue_context_modification_request msg;

  msg.ue_index                         = ue_index;
  msg.ue_aggregate_maximum_bit_rate_ul = 200000000;

  for (drb_id_t drb_id : drbs_to_add) {
    cu_cp_drb_setup_message drb_setup_msg{};
    drb_setup_msg.drb_id                  = drb_id;
    drb_setup_msg.rlc                     = srsran::rlc_mode::am;
    drb_setup_msg.qos_info.is_dynamic_5qi = true;
    drb_setup_msg.qos_info.five_qi        = 8;
    drb_setup_msg.qos_info.prio_level_arp = 1;

    msg.cu_cp_drb_setup_msgs.emplace(drb_id, drb_setup_msg);
  }

  return msg;
}

f1ap_message
srsran::srs_cu_cp::generate_ue_context_modification_response(gnb_cu_ue_f1ap_id_t                    cu_ue_id,
                                                             gnb_du_ue_f1ap_id_t                    du_ue_id,
                                                             rnti_t                                 crnti,
                                                             const std::initializer_list<drb_id_t>& drbs_added)
{
  f1ap_message ue_context_modification_response = {};

  ue_context_modification_response.pdu.set_successful_outcome();
  ue_context_modification_response.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);

  auto& ue_context_mod_resp = ue_context_modification_response.pdu.successful_outcome().value.ue_context_mod_resp();
  ue_context_mod_resp->gnb_cu_ue_f1ap_id.value     = (unsigned)cu_ue_id;
  ue_context_mod_resp->gnb_du_ue_f1ap_id.value     = (unsigned)du_ue_id;
  ue_context_mod_resp->c_rnti_present              = true;
  ue_context_mod_resp->c_rnti.value                = (unsigned)crnti;
  ue_context_mod_resp->drbs_setup_mod_list_present = drbs_added.size() > 0;
  for (drb_id_t drb_id : drbs_added) {
    ue_context_mod_resp->drbs_setup_mod_list.value.push_back({});
    ue_context_mod_resp->drbs_setup_mod_list.value.back().load_info_obj(ASN1_F1AP_ID_DRBS_SETUP_MOD_ITEM);
    ue_context_mod_resp->drbs_setup_mod_list.value.back().value().drbs_setup_mod_item().drb_id = drb_id_to_uint(drb_id);
  }

  return ue_context_modification_response;
}

f1ap_message srsran::srs_cu_cp::generate_ue_context_modification_failure(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                         gnb_du_ue_f1ap_id_t du_ue_id)
{
  f1ap_message ue_context_modification_failure = {};

  ue_context_modification_failure.pdu.set_unsuccessful_outcome();
  ue_context_modification_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);

  auto& ue_context_mod_fail = ue_context_modification_failure.pdu.unsuccessful_outcome().value.ue_context_mod_fail();
  ue_context_mod_fail->gnb_cu_ue_f1ap_id.value = (unsigned)cu_ue_id;
  ue_context_mod_fail->gnb_du_ue_f1ap_id.value = (unsigned)du_ue_id;
  ue_context_mod_fail->cause.value.set_radio_network();
  ue_context_mod_fail->cause.value.radio_network() =
      cause_radio_network_opts::options::unknown_or_already_allocated_gnb_cu_ue_f1ap_id;

  return ue_context_modification_failure;
}

cu_cp_paging_message srsran::srs_cu_cp::generate_paging_message()
{
  cu_cp_paging_message paging_msg;

  // add ue paging id
  paging_msg.ue_paging_id.amf_set_id  = 1;
  paging_msg.ue_paging_id.amf_pointer = 0;
  paging_msg.ue_paging_id.five_g_tmsi = 4211117727;

  // add paging drx
  paging_msg.paging_drx = 64;

  // add tai list for paging
  cu_cp_tai_list_for_paging_item tai_item;
  tai_item.tai.plmn_id = "00f110";
  tai_item.tai.tac     = 7;
  paging_msg.tai_list_for_paging.push_back(tai_item);

  // add paging prio
  paging_msg.paging_prio = 5;

  // add ue radio cap for paging
  cu_cp_ue_radio_cap_for_paging ue_radio_cap_for_paging;
  ue_radio_cap_for_paging.ue_radio_cap_for_paging_of_nr = make_byte_buffer("deadbeef");
  paging_msg.ue_radio_cap_for_paging                    = ue_radio_cap_for_paging;

  // add paging origin
  paging_msg.paging_origin = "non-3gpp";

  // add assist data for paging
  cu_cp_assist_data_for_paging assist_data_for_paging;

  // add assist data for recommended cells
  cu_cp_assist_data_for_recommended_cells assist_data_for_recommended_cells;

  cu_cp_recommended_cell_item recommended_cell_item;

  // add ngran cgi
  recommended_cell_item.ngran_cgi.nci.packed = 12345678;
  recommended_cell_item.ngran_cgi.plmn_hex   = "00f110";

  // add time stayed in cell
  recommended_cell_item.time_stayed_in_cell = 5;

  assist_data_for_recommended_cells.recommended_cells_for_paging.recommended_cell_list.push_back(recommended_cell_item);

  assist_data_for_paging.assist_data_for_recommended_cells = assist_data_for_recommended_cells;

  // add paging attempt info
  cu_cp_paging_attempt_info paging_attempt_info;

  paging_attempt_info.paging_attempt_count         = 3;
  paging_attempt_info.intended_nof_paging_attempts = 4;
  paging_attempt_info.next_paging_area_scope       = "changed";

  assist_data_for_paging.paging_attempt_info = paging_attempt_info;

  paging_msg.assist_data_for_paging = assist_data_for_paging;

  return paging_msg;
}
