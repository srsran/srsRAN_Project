/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "f1ap_cu_test_messages.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_ue_context_update.h"

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::f1ap;

asn1::f1ap::gnb_du_served_cells_item_s srsran::srs_cu_cp::generate_served_cells_item(unsigned nrcell_id, pci_t nrpci)
{
  asn1::f1ap::gnb_du_served_cells_item_s served_cells_item;
  served_cells_item.served_cell_info.nr_cgi.plmn_id.from_string("00f110");
  served_cells_item.served_cell_info.nr_cgi.nr_cell_id.from_number(nrcell_id);
  served_cells_item.served_cell_info.nr_pci              = nrpci;
  served_cells_item.served_cell_info.five_gs_tac_present = true;
  served_cells_item.served_cell_info.five_gs_tac.from_number(7);

  asn1::f1ap::served_plmns_item_s served_plmn;
  served_plmn.plmn_id.from_string("00f110");
  asn1::f1ap::slice_support_item_s slice_support_item;
  slice_support_item.snssai.sst.from_number(1);
  served_plmn.ie_exts.tai_slice_support_list_present = true;
  served_plmn.ie_exts.tai_slice_support_list.push_back(slice_support_item);
  served_cells_item.served_cell_info.served_plmns.push_back(served_plmn);

  served_cells_item.served_cell_info.nr_mode_info.set_tdd();
  served_cells_item.served_cell_info.nr_mode_info.tdd().nr_freq_info.nr_arfcn = 626748;
  asn1::f1ap::freq_band_nr_item_s freq_band_nr_item;
  freq_band_nr_item.freq_band_ind_nr = 78;
  served_cells_item.served_cell_info.nr_mode_info.tdd().nr_freq_info.freq_band_list_nr.push_back(freq_band_nr_item);
  served_cells_item.served_cell_info.nr_mode_info.tdd().tx_bw.nr_scs.value = asn1::f1ap::nr_scs_opts::scs30;
  served_cells_item.served_cell_info.nr_mode_info.tdd().tx_bw.nr_nrb.value = asn1::f1ap::nr_nrb_opts::nrb51;
  served_cells_item.served_cell_info.meas_timing_cfg.from_string("101105af4084");

  served_cells_item.gnb_du_sys_info_present = true;
  served_cells_item.gnb_du_sys_info.mib_msg.from_string("01c586");
  served_cells_item.gnb_du_sys_info.sib1_msg.from_string(
      "92002808241099000001000000000a4213407800008c98d6d8d7f616e0804000020107e28180008000088a0dc7008000088a0007141a22"
      "81c874cc00020000232d5c6b6c65462001ec4cc5fc9c0493946a98d4d1e99355c00a1aba010580ec024646f62180");

  return served_cells_item;
}

f1ap_message srsran::srs_cu_cp::generate_f1_setup_request(unsigned gnb_du_id, unsigned nrcell_id, pci_t pci)
{
  f1ap_message msg;
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

  auto& setup_req                = msg.pdu.init_msg().value.f1_setup_request();
  setup_req->transaction_id      = 99;
  setup_req->gnb_du_id           = gnb_du_id;
  setup_req->gnb_du_name_present = true;
  setup_req->gnb_du_name.from_string("srsDU");
  setup_req->gnb_du_rrc_version.latest_rrc_version.from_number(1);
  setup_req->gnb_du_served_cells_list_present = true;
  setup_req->gnb_du_served_cells_list.resize(1);
  setup_req->gnb_du_served_cells_list[0].load_info_obj(ASN1_F1AP_ID_GNB_DU_SERVED_CELLS_ITEM);
  setup_req->gnb_du_served_cells_list[0].value().gnb_du_served_cells_item() =
      generate_served_cells_item(nrcell_id, pci);

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
  init_ul_rrc->gnb_du_ue_f1ap_id          = (unsigned)du_ue_id;

  init_ul_rrc->nr_cgi.nr_cell_id.from_string("000000000000000000000001100110110000"); // 6576 in decimal
  init_ul_rrc->nr_cgi.plmn_id.from_string("00f110");
  init_ul_rrc->c_rnti = crnti;

  init_ul_rrc->sul_access_ind_present = true;
  init_ul_rrc->sul_access_ind.value   = asn1::f1ap::sul_access_ind_opts::options::true_value;

  init_ul_rrc->rrc_container.from_string("1dec89d05766");

  init_ul_rrc->du_to_cu_rrc_container_present = true;
  if (cell_group_cfg.empty()) {
    init_ul_rrc->du_to_cu_rrc_container.from_string(
        "5c00b001117aec701061e0007c20408d07810020a2090480ca8000f800000000008370842000088165000048200002069a06aa49880002"
        "00204000400d008013b64b1814400e468acf120000096070820f177e060870000000e25038000040bde802000400000000028201950300"
        "c400");
  } else {
    init_ul_rrc->du_to_cu_rrc_container = std::move(cell_group_cfg);
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

  auto& ul_rrc              = ul_rrc_msg.pdu.init_msg().value.ul_rrc_msg_transfer();
  ul_rrc->gnb_du_ue_f1ap_id = (unsigned)du_ue_id;
  ul_rrc->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  ul_rrc->srb_id            = (unsigned)srb_id;
  ul_rrc->rrc_container     = std::move(rrc_container);

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
  rel_complete_msg->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  rel_complete_msg->gnb_du_ue_f1ap_id = (unsigned)du_ue_id;

  return ue_ctxt_rel_complete_msg;
}

f1ap_message srsran::srs_cu_cp::generate_ue_context_setup_request(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                  gnb_du_ue_f1ap_id_t du_ue_id)
{
  f1ap_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);

  auto& ue_ctx_req              = msg.pdu.init_msg().value.ue_context_setup_request();
  ue_ctx_req->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  ue_ctx_req->gnb_du_ue_f1ap_id = (unsigned)du_ue_id;

  ue_ctx_req->srbs_to_be_setup_list_present = true;
  ue_ctx_req->srbs_to_be_setup_list.resize(1);
  ue_ctx_req->srbs_to_be_setup_list[0].load_info_obj(ASN1_F1AP_ID_SRBS_TO_BE_SETUP_ITEM);
  srbs_to_be_setup_item_s& srb2     = ue_ctx_req->srbs_to_be_setup_list[0].value().srbs_to_be_setup_item();
  srb2.srb_id                       = 2;
  ue_ctx_req->rrc_container_present = true;
  ue_ctx_req->rrc_container.from_string("000320080895755b0c");

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
  ue_context_setup_resp->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  ue_context_setup_resp->gnb_du_ue_f1ap_id = (unsigned)du_ue_id;
  ue_context_setup_resp->c_rnti_present    = true;
  ue_context_setup_resp->c_rnti            = (unsigned)crnti;
  ue_context_setup_resp->du_to_cu_rrc_info.cell_group_cfg =
      make_byte_buffer("5c02b091117aec701061e000b1c03544cde4a20c7c080408c008241000100000");

  return ue_context_setup_response;
}

f1ap_message srsran::srs_cu_cp::generate_ue_context_setup_failure(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                  gnb_du_ue_f1ap_id_t du_ue_id)
{
  f1ap_message ue_context_setup_failure = {};

  ue_context_setup_failure.pdu.set_unsuccessful_outcome();
  ue_context_setup_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);

  auto& ue_context_setup_fail = ue_context_setup_failure.pdu.unsuccessful_outcome().value.ue_context_setup_fail();
  ue_context_setup_fail->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  ue_context_setup_fail->gnb_du_ue_f1ap_id = (unsigned)du_ue_id;
  ue_context_setup_fail->cause.set_radio_network();
  ue_context_setup_fail->cause.radio_network() =
      cause_radio_network_opts::options::unknown_or_already_allocated_gnb_cu_ue_f1ap_id;

  return ue_context_setup_failure;
}

f1ap_ue_context_modification_request srsran::srs_cu_cp::generate_ue_context_modification_request(ue_index_t ue_index)
{
  f1ap_ue_context_modification_request msg;

  msg.ue_index = ue_index;

  // sp cell id
  nr_cell_global_id_t sp_cell_id;
  sp_cell_id.nci      = 6576;
  sp_cell_id.plmn_hex = "00f110";
  msg.sp_cell_id      = sp_cell_id;

  // serv cell idx
  msg.serv_cell_idx = 1;

  // sp cell ul cfg
  msg.sp_cell_ul_cfg = f1ap_cell_ul_cfg::ul;

  // drx cycle
  f1ap_drx_cycle drx_cycle;
  drx_cycle.long_drx_cycle_len    = 5;
  drx_cycle.short_drx_cycle_len   = 1;
  drx_cycle.short_drx_cycle_timer = 10;
  msg.drx_cycle                   = drx_cycle;

  // cu to du to rrc info
  f1ap_cu_to_du_rrc_info cu_to_du_rrc_info;
  cu_to_du_rrc_info.cg_cfg_info               = make_byte_buffer("deadbeef");
  cu_to_du_rrc_info.ue_cap_rat_container_list = make_byte_buffer("deadbeef");
  cu_to_du_rrc_info.meas_cfg                  = make_byte_buffer("deadbeef");
  msg.cu_to_du_rrc_info                       = cu_to_du_rrc_info;

  // tx action ind
  msg.tx_action_ind = f1ap_tx_action_ind::stop;

  // res coordination transfer container
  msg.res_coordination_transfer_container = make_byte_buffer("deadbeef");

  // rrc recfg complete ind
  msg.rrc_recfg_complete_ind = f1ap_rrc_recfg_complete_ind::true_value;

  // rrc container
  msg.rrc_container = make_byte_buffer("deadbeef");

  // scell to be setup mod list
  f1ap_scell_to_be_setup_mod_item scell_to_be_setup_mod_item;
  scell_to_be_setup_mod_item.scell_id.nci      = 6576;
  scell_to_be_setup_mod_item.scell_id.plmn_hex = "00f110";
  scell_to_be_setup_mod_item.scell_idx         = 1;
  scell_to_be_setup_mod_item.scell_ul_cfg      = f1ap_cell_ul_cfg::ul;
  msg.scell_to_be_setup_mod_list.push_back(scell_to_be_setup_mod_item);

  // scell to be remd list
  f1ap_scell_to_be_remd_item scell_to_be_remd_item;
  scell_to_be_remd_item.scell_id.nci      = 6576;
  scell_to_be_remd_item.scell_id.plmn_hex = "00f110";
  msg.scell_to_be_remd_list.push_back(scell_to_be_remd_item);

  // srbs to be setup mod list
  f1ap_srbs_to_be_setup_mod_item srbs_to_be_setup_mod_item;
  srbs_to_be_setup_mod_item.srb_id   = int_to_srb_id(1);
  srbs_to_be_setup_mod_item.dupl_ind = f1ap_dupl_ind::true_value;
  msg.srbs_to_be_setup_mod_list.emplace(int_to_srb_id(1), srbs_to_be_setup_mod_item);

  // drbs to be setup mod list
  f1ap_drbs_to_be_setup_mod_item drbs_to_be_setup_mod_item;
  drbs_to_be_setup_mod_item.drb_id = uint_to_drb_id(1);
  // qos info

  // drb qos
  // qos flow level qos params
  // qos characteristics
  non_dyn_5qi_descriptor_t non_dyn_5qi;
  non_dyn_5qi.five_qi                                                        = uint_to_five_qi(8);
  non_dyn_5qi.qos_prio_level                                                 = uint_to_qos_prio_level(1);
  non_dyn_5qi.averaging_win                                                  = 3;
  non_dyn_5qi.max_data_burst_volume                                          = 1000;
  drbs_to_be_setup_mod_item.qos_info.drb_qos.qos_characteristics.non_dyn_5qi = non_dyn_5qi;

  // ng ran alloc retention prio
  drbs_to_be_setup_mod_item.qos_info.drb_qos.alloc_and_retention_prio.prio_level_arp  = 1;
  drbs_to_be_setup_mod_item.qos_info.drb_qos.alloc_and_retention_prio.pre_emption_cap = "shall-not-trigger-pre-emption";
  drbs_to_be_setup_mod_item.qos_info.drb_qos.alloc_and_retention_prio.pre_emption_vulnerability = "not-pre-emptable";

  // gbr qos flow info
  cu_cp_gbr_qos_info gbr_qos_info;
  gbr_qos_info.max_flow_bit_rate_dl                       = 100000;
  gbr_qos_info.max_flow_bit_rate_ul                       = 100000;
  gbr_qos_info.guaranteed_flow_bit_rate_dl                = 100000;
  gbr_qos_info.guaranteed_flow_bit_rate_ul                = 100000;
  gbr_qos_info.max_packet_loss_rate_dl                    = 30;
  gbr_qos_info.max_packet_loss_rate_ul                    = 30;
  drbs_to_be_setup_mod_item.qos_info.drb_qos.gbr_qos_info = gbr_qos_info;

  // reflective qos attribute
  drbs_to_be_setup_mod_item.qos_info.drb_qos.reflective_qos_attribute = true;

  // s nssai
  drbs_to_be_setup_mod_item.qos_info.s_nssai.sst = 1;
  drbs_to_be_setup_mod_item.qos_info.s_nssai.sd  = 128;

  // notif ctrl
  drbs_to_be_setup_mod_item.qos_info.notif_ctrl = f1ap_notif_ctrl::active;

  // flows mapped to drb list
  f1ap_flows_mapped_to_drb_item flows_mapped_to_drb_item;
  flows_mapped_to_drb_item.qos_flow_id = uint_to_qos_flow_id(1);
  // qos characteristics
  flows_mapped_to_drb_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi = non_dyn_5qi;
  // ng ran alloc retention prio
  flows_mapped_to_drb_item.qos_flow_level_qos_params.alloc_and_retention_prio.prio_level_arp = 1;
  flows_mapped_to_drb_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_cap =
      "shall-not-trigger-pre-emption";
  flows_mapped_to_drb_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_vulnerability =
      "not-pre-emptable";
  // gbr qos flow info
  flows_mapped_to_drb_item.qos_flow_level_qos_params.gbr_qos_info = gbr_qos_info;
  // reflective qos attribute
  flows_mapped_to_drb_item.qos_flow_level_qos_params.reflective_qos_attribute = true;

  drbs_to_be_setup_mod_item.qos_info.flows_mapped_to_drb_list.emplace(uint_to_qos_flow_id(1), flows_mapped_to_drb_item);

  // ul up tnl info to be setup list
  up_transport_layer_info ul_up_tnl_info_item = {transport_layer_address{"127.0.0.1"}, int_to_gtpu_teid(1)};
  drbs_to_be_setup_mod_item.ul_up_tnl_info_to_be_setup_list.push_back(ul_up_tnl_info_item);

  // rlc mode
  drbs_to_be_setup_mod_item.rlc_mod = rlc_mode::am;

  // ul cfg
  f1ap_ul_cfg ul_cfg;
  ul_cfg.ul_ue_cfg                 = f1ap_ul_ue_cfg::no_data;
  drbs_to_be_setup_mod_item.ul_cfg = ul_cfg;

  // dupl activation
  drbs_to_be_setup_mod_item.dupl_activation = f1ap_dupl_activation::active;

  msg.drbs_to_be_setup_mod_list.emplace(uint_to_drb_id(1), drbs_to_be_setup_mod_item);

  // drbs to be modified list
  f1ap_drbs_to_be_modified_item drbs_to_be_modified_item;

  drbs_to_be_modified_item.drb_id = uint_to_drb_id(1);

  // qos info
  f1ap_drb_info qos_info;
  // drb qos
  // qos flow level qos params
  // qos characteristics
  qos_info.drb_qos.qos_characteristics.non_dyn_5qi = non_dyn_5qi;

  // ng ran alloc retention prio
  qos_info.drb_qos.alloc_and_retention_prio.prio_level_arp            = 1;
  qos_info.drb_qos.alloc_and_retention_prio.pre_emption_cap           = "shall-not-trigger-pre-emption";
  qos_info.drb_qos.alloc_and_retention_prio.pre_emption_vulnerability = "not-pre-emptable";

  // gbr qos flow info
  qos_info.drb_qos.gbr_qos_info = gbr_qos_info;

  // reflective qos attribute
  qos_info.drb_qos.reflective_qos_attribute = true;

  // s nssai
  qos_info.s_nssai.sst = 1;
  qos_info.s_nssai.sd  = 128;

  // notif ctrl
  qos_info.notif_ctrl = f1ap_notif_ctrl::active;

  // flows mapped to drb list
  qos_info.flows_mapped_to_drb_list.emplace(uint_to_qos_flow_id(1), flows_mapped_to_drb_item);

  // qos info
  drbs_to_be_modified_item.qos_info = qos_info;

  // ul up tnl info to be setup list
  drbs_to_be_modified_item.ul_up_tnl_info_to_be_setup_list.push_back(ul_up_tnl_info_item);

  // ul cfg
  drbs_to_be_modified_item.ul_cfg = ul_cfg;

  msg.drbs_to_be_modified_list.emplace(uint_to_drb_id(1), drbs_to_be_modified_item);

  // srbs to be released list
  msg.srbs_to_be_released_list.push_back(int_to_srb_id(1));

  // drbs to be released list
  msg.drbs_to_be_released_list.push_back(uint_to_drb_id(1));

  // inactivity monitoring request
  msg.inactivity_monitoring_request = true;

  // rat freq prio info
  f1ap_rat_freq_prio_info rat_freq_prio_info;
  rat_freq_prio_info.ngran.emplace();
  rat_freq_prio_info.ngran = 1;
  msg.rat_freq_prio_info   = rat_freq_prio_info;

  // drx cfg ind
  msg.drx_cfg_ind = true;

  // rlc fail ind
  f1ap_rlc_fail_ind rlc_fail_ind;
  rlc_fail_ind.assocated_lcid = lcid_t::LCID_SRB1;
  msg.rlc_fail_ind            = rlc_fail_ind;

  // ul tx direct current list info
  msg.ul_tx_direct_current_list_info = make_byte_buffer("deadbeef");

  // gnb du cfg query
  msg.gnb_du_cfg_query = true;

  // gnb du ue ambr ul
  msg.gnb_du_ue_ambr_ul = 200000000;

  // execute dupl
  msg.execute_dupl = true;

  // rrc delivery status request
  msg.rrc_delivery_status_request = true;

  // res coordination transfer info
  f1ap_res_coordination_transfer_info res_coordination_transfer_info;
  res_coordination_transfer_info.m_enb_cell_id = 6576;
  msg.res_coordination_transfer_info           = res_coordination_transfer_info;

  // serving cell mo
  msg.serving_cell_mo = 1;

  // need for gap
  msg.need_for_gap = true;

  // full cfg
  msg.full_cfg = true;

  return msg;
}

f1ap_message srsran::srs_cu_cp::generate_ue_context_modification_response(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                          gnb_du_ue_f1ap_id_t du_ue_id,
                                                                          rnti_t              crnti)
{
  f1ap_message ue_context_modification_response = {};

  ue_context_modification_response.pdu.set_successful_outcome();
  ue_context_modification_response.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);

  auto& ue_context_mod_resp = ue_context_modification_response.pdu.successful_outcome().value.ue_context_mod_resp();
  ue_context_mod_resp->gnb_cu_ue_f1ap_id           = (unsigned)cu_ue_id;
  ue_context_mod_resp->gnb_du_ue_f1ap_id           = (unsigned)du_ue_id;
  ue_context_mod_resp->c_rnti_present              = true;
  ue_context_mod_resp->c_rnti                      = (unsigned)crnti;
  ue_context_mod_resp->drbs_setup_mod_list_present = true;

  ue_context_mod_resp->drbs_setup_mod_list.push_back({});
  ue_context_mod_resp->drbs_setup_mod_list.back().load_info_obj(ASN1_F1AP_ID_DRBS_SETUP_MOD_ITEM);
  ue_context_mod_resp->drbs_setup_mod_list.back().value().drbs_setup_mod_item().drb_id = 1;

  return ue_context_modification_response;
}

f1ap_message srsran::srs_cu_cp::generate_ue_context_modification_failure(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                         gnb_du_ue_f1ap_id_t du_ue_id)
{
  f1ap_message ue_context_modification_failure = {};

  ue_context_modification_failure.pdu.set_unsuccessful_outcome();
  ue_context_modification_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);

  auto& ue_context_mod_fail = ue_context_modification_failure.pdu.unsuccessful_outcome().value.ue_context_mod_fail();
  ue_context_mod_fail->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  ue_context_mod_fail->gnb_du_ue_f1ap_id = (unsigned)du_ue_id;
  ue_context_mod_fail->cause.set_radio_network();
  ue_context_mod_fail->cause.radio_network() =
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
  paging_msg.paging_origin = true;

  // add assist data for paging
  cu_cp_assist_data_for_paging assist_data_for_paging;

  // add assist data for recommended cells
  cu_cp_assist_data_for_recommended_cells assist_data_for_recommended_cells;

  cu_cp_recommended_cell_item recommended_cell_item;

  // add ngran cgi
  recommended_cell_item.ngran_cgi.nci      = 6576;
  recommended_cell_item.ngran_cgi.plmn_hex = "00f110";

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

f1ap_message srsran::srs_cu_cp::generate_ue_context_release_request(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                    gnb_du_ue_f1ap_id_t du_ue_id)
{
  f1ap_message msg;
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE_REQUEST);

  auto& release_req              = msg.pdu.init_msg().value.ue_context_release_request();
  release_req->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  release_req->gnb_du_ue_f1ap_id = (unsigned)du_ue_id;
  release_req->cause.set_radio_network();
  release_req->cause.radio_network().value = asn1::f1ap::cause_radio_network_e::rl_fail_others;

  return msg;
}
