/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "e1ap_cu_up_test_messages.h"
#include "srsran/asn1/e1ap/e1ap_pdu_contents.h"
#include "srsran/e1ap/common/e1ap_message.h"

using namespace srsran;
using namespace srs_cu_up;
using namespace asn1::e1ap;

cu_up_e1_setup_request srsran::srs_cu_up::generate_cu_up_e1_setup_request()
{
  cu_up_e1_setup_request request_msg = {};
  request_msg.gnb_cu_up_id           = 1;
  request_msg.gnb_cu_up_name         = "srsCU-CP";
  request_msg.cn_support             = cu_up_cn_support_t::c_5gc;

  return request_msg;
}

e1ap_message srsran::srs_cu_up::generate_cu_up_e1_setup_response(unsigned transaction_id)
{
  e1ap_message e1_setup_response = {};
  e1_setup_response.pdu.set_successful_outcome();
  e1_setup_response.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_GNB_CU_UP_E1_SETUP);

  auto& setup_res = e1_setup_response.pdu.successful_outcome().value.gnb_cu_up_e1_setup_resp();

  setup_res->transaction_id         = transaction_id;
  setup_res->gnb_cu_cp_name_present = true;
  setup_res->gnb_cu_cp_name.from_string("srsCU-CP");

  return e1_setup_response;
}

e1ap_message srsran::srs_cu_up::generate_bearer_context_setup_request(unsigned int cu_cp_ue_e1ap_id)
{
  e1ap_message bearer_context_setup_request = {};

  bearer_context_setup_request.pdu.set_init_msg();
  bearer_context_setup_request.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_SETUP);

  auto& bearer_context_setup_req = bearer_context_setup_request.pdu.init_msg().value.bearer_context_setup_request();
  bearer_context_setup_req->gnb_cu_cp_ue_e1ap_id = cu_cp_ue_e1ap_id;
  bearer_context_setup_req->security_info.security_algorithm.ciphering_algorithm =
      asn1::e1ap::ciphering_algorithm_e::nea0;
  bearer_context_setup_req->security_info.up_securitykey.encryption_key.from_string("a6ae39efbe0d424cd85f4a9c3aee0414");
  bearer_context_setup_req->ue_dl_aggr_max_bit_rate = 1000000000U;
  bearer_context_setup_req->serving_plmn.from_string("02f899");
  bearer_context_setup_req->activity_notif_level        = asn1::e1ap::activity_notif_level_e::ue;
  bearer_context_setup_req->ue_inactivity_timer_present = true;
  bearer_context_setup_req->ue_inactivity_timer         = 60;

  bearer_context_setup_req->sys_bearer_context_setup_request.set_ng_ran_bearer_context_setup_request();

  auto& ng_ran_bearer_context_setup_req =
      bearer_context_setup_req->sys_bearer_context_setup_request.ng_ran_bearer_context_setup_request();

  ng_ran_bearer_context_setup_req.resize(1);

  asn1::e1ap::pdu_session_res_to_setup_item_s pdu_session_res_to_setup_item;
  pdu_session_res_to_setup_item.pdu_session_id   = 1;
  pdu_session_res_to_setup_item.pdu_session_type = asn1::e1ap::pdu_session_type_e::ipv4;
  pdu_session_res_to_setup_item.snssai.sst.from_number(1);
  pdu_session_res_to_setup_item.security_ind.integrity_protection_ind =
      asn1::e1ap::integrity_protection_ind_e::not_needed;
  pdu_session_res_to_setup_item.security_ind.confidentiality_protection_ind =
      asn1::e1ap::confidentiality_protection_ind_e::not_needed;
  pdu_session_res_to_setup_item.pdu_session_res_dl_ambr_present = true;
  pdu_session_res_to_setup_item.pdu_session_res_dl_ambr         = 100000000U;

  pdu_session_res_to_setup_item.ng_ul_up_tnl_info.set_gtp_tunnel();
  auto& gtp_tunnel = pdu_session_res_to_setup_item.ng_ul_up_tnl_info.gtp_tunnel();
  gtp_tunnel.transport_layer_address.from_number(2887058566);
  gtp_tunnel.gtp_teid.from_string("00000036");

  asn1::e1ap::drb_to_setup_item_ng_ran_s drb_to_setup_item_ng_ran;
  drb_to_setup_item_ng_ran.drb_id = 4;

  drb_to_setup_item_ng_ran.sdap_cfg.default_drb = asn1::e1ap::default_drb_e::true_value;
  drb_to_setup_item_ng_ran.sdap_cfg.sdap_hdr_ul = asn1::e1ap::sdap_hdr_ul_e::present;
  drb_to_setup_item_ng_ran.sdap_cfg.sdap_hdr_dl = asn1::e1ap::sdap_hdr_dl_e::absent;

  drb_to_setup_item_ng_ran.pdcp_cfg.pdcp_sn_size_ul                 = asn1::e1ap::pdcp_sn_size_e::s_neg18;
  drb_to_setup_item_ng_ran.pdcp_cfg.pdcp_sn_size_dl                 = asn1::e1ap::pdcp_sn_size_e::s_neg18;
  drb_to_setup_item_ng_ran.pdcp_cfg.rlc_mode                        = asn1::e1ap::rlc_mode_e::rlc_am;
  drb_to_setup_item_ng_ran.pdcp_cfg.t_reordering_timer_present      = true;
  drb_to_setup_item_ng_ran.pdcp_cfg.t_reordering_timer.t_reordering = asn1::e1ap::t_reordering_e::ms100;
  drb_to_setup_item_ng_ran.pdcp_cfg.discard_timer_present           = true;
  drb_to_setup_item_ng_ran.pdcp_cfg.discard_timer                   = asn1::e1ap::discard_timer_e::infinity;

  asn1::e1ap::cell_group_info_item_s cell_group_info_item = {};
  cell_group_info_item.cell_group_id                      = 0;
  drb_to_setup_item_ng_ran.cell_group_info.push_back(cell_group_info_item);

  asn1::e1ap::qos_flow_qos_param_item_s qo_s_flow_qos_param_item{};
  qo_s_flow_qos_param_item.qos_flow_id = 1;
  qo_s_flow_qos_param_item.qos_flow_level_qos_params.qos_characteristics.set_non_dyn_5qi();
  auto& qos_characteristics   = qo_s_flow_qos_param_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi();
  qos_characteristics.five_qi = 9;

  qo_s_flow_qos_param_item.qos_flow_level_qos_params.ngra_nalloc_retention_prio.prio_level = 15;
  qo_s_flow_qos_param_item.qos_flow_level_qos_params.ngra_nalloc_retention_prio.pre_emption_cap =
      asn1::e1ap::pre_emption_cap_e::shall_not_trigger_pre_emption;

  qo_s_flow_qos_param_item.qos_flow_level_qos_params.ngra_nalloc_retention_prio.pre_emption_vulnerability =
      asn1::e1ap::pre_emption_vulnerability_e::pre_emptable;

  drb_to_setup_item_ng_ran.qos_flow_info_to_be_setup.push_back(qo_s_flow_qos_param_item);

  pdu_session_res_to_setup_item.drb_to_setup_list_ng_ran.push_back(drb_to_setup_item_ng_ran);

  ng_ran_bearer_context_setup_req[0].value().pdu_session_res_to_setup_list().push_back(pdu_session_res_to_setup_item);

  return bearer_context_setup_request;
}

e1ap_message srsran::srs_cu_up::generate_invalid_bearer_context_setup_request(unsigned int cu_cp_ue_e1ap_id)
{
  e1ap_message bearer_context_setup_request = {};

  bearer_context_setup_request.pdu.set_init_msg();
  bearer_context_setup_request.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_SETUP);

  auto& bearer_context_setup_req = bearer_context_setup_request.pdu.init_msg().value.bearer_context_setup_request();
  bearer_context_setup_req->gnb_cu_cp_ue_e1ap_id = cu_cp_ue_e1ap_id;
  bearer_context_setup_req->security_info.security_algorithm.ciphering_algorithm =
      asn1::e1ap::ciphering_algorithm_e::nea0;
  bearer_context_setup_req->security_info.up_securitykey.encryption_key.from_string("a6ae39efbe0d424cd85f4a9c3aee0414");
  bearer_context_setup_req->ue_dl_aggr_max_bit_rate = 1000000000U;
  bearer_context_setup_req->serving_plmn.from_string("02f899");
  bearer_context_setup_req->activity_notif_level = asn1::e1ap::activity_notif_level_e::ue;

  bearer_context_setup_req->sys_bearer_context_setup_request.set_e_utran_bearer_context_setup_request();

  return bearer_context_setup_request;
}

e1ap_message
srsran::srs_cu_up::generate_invalid_bearer_context_setup_request_inactivity_timer(unsigned int cu_cp_ue_e1ap_id)
{
  e1ap_message bearer_context_setup_request = {};
  bearer_context_setup_request              = generate_bearer_context_setup_request(cu_cp_ue_e1ap_id);
  // Invalidate UE inactivity timer
  auto& bearer_context_setup_req = bearer_context_setup_request.pdu.init_msg().value.bearer_context_setup_request();
  bearer_context_setup_req->ue_inactivity_timer_present = false;
  return bearer_context_setup_request;
}

e1ap_message srsran::srs_cu_up::generate_bearer_context_modification_request(unsigned int cu_cp_ue_e1ap_id,
                                                                             unsigned int cu_up_ue_e1ap_id)
{
  e1ap_message bearer_context_modification_request = {};

  bearer_context_modification_request.pdu.set_init_msg();
  bearer_context_modification_request.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_MOD);
  auto& bearer_context_mod_req = bearer_context_modification_request.pdu.init_msg().value.bearer_context_mod_request();
  bearer_context_mod_req->gnb_cu_cp_ue_e1ap_id = cu_cp_ue_e1ap_id;
  bearer_context_mod_req->sys_bearer_context_mod_request.set_ng_ran_bearer_context_mod_request();

  auto& ng_ran_bearer_context_mod_req =
      bearer_context_mod_req->sys_bearer_context_mod_request.ng_ran_bearer_context_mod_request();

  ng_ran_bearer_context_mod_req.pdu_session_res_to_modify_list_present = true;

  asn1::e1ap::pdu_session_res_to_modify_item_s pdu_session_res_to_setup_modify_item = {};
  pdu_session_res_to_setup_modify_item.pdu_session_id                               = 1;

  asn1::e1ap::drb_to_modify_item_ng_ran_s drb_to_mod_item_ng_ran;
  drb_to_mod_item_ng_ran.drb_id = 4;

  asn1::e1ap::up_params_item_s up_params_item;
  up_params_item.up_tnl_info.set_gtp_tunnel();
  auto& gtp_tunnel = up_params_item.up_tnl_info.gtp_tunnel();
  gtp_tunnel.transport_layer_address.from_number(2887061624);
  gtp_tunnel.gtp_teid.from_string("7e9fcae5");
  up_params_item.cell_group_id = 0;

  drb_to_mod_item_ng_ran.dl_up_params.push_back(up_params_item);

  pdu_session_res_to_setup_modify_item.drb_to_modify_list_ng_ran.push_back(drb_to_mod_item_ng_ran);

  ng_ran_bearer_context_mod_req.pdu_session_res_to_modify_list.push_back(pdu_session_res_to_setup_modify_item);

  return bearer_context_modification_request;
}

e1ap_message srsran::srs_cu_up::generate_invalid_bearer_context_modification_request(unsigned int cu_cp_ue_e1ap_id,
                                                                                     unsigned int cu_up_ue_e1ap_id)
{
  e1ap_message bearer_context_modification_request = {};

  bearer_context_modification_request.pdu.set_init_msg();
  bearer_context_modification_request.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_MOD);

  auto& bearer_context_mod_req = bearer_context_modification_request.pdu.init_msg().value.bearer_context_mod_request();
  bearer_context_mod_req->gnb_cu_cp_ue_e1ap_id = cu_cp_ue_e1ap_id;
  bearer_context_mod_req->gnb_cu_up_ue_e1ap_id = cu_up_ue_e1ap_id;

  bearer_context_mod_req->sys_bearer_context_mod_request_present = true;
  bearer_context_mod_req->sys_bearer_context_mod_request.set_e_utran_bearer_context_mod_request();

  return bearer_context_modification_request;
}

e1ap_message srsran::srs_cu_up::generate_bearer_context_release_command(unsigned int cu_cp_ue_e1ap_id,
                                                                        unsigned int cu_up_ue_e1ap_id)
{
  e1ap_message bearer_context_release_command = {};

  bearer_context_release_command.pdu.set_init_msg();
  bearer_context_release_command.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_RELEASE);

  auto& bearer_context_release_cmd = bearer_context_release_command.pdu.init_msg().value.bearer_context_release_cmd();
  bearer_context_release_cmd->gnb_cu_cp_ue_e1ap_id = cu_cp_ue_e1ap_id;
  bearer_context_release_cmd->gnb_cu_up_ue_e1ap_id = cu_up_ue_e1ap_id;
  bearer_context_release_cmd->cause.set_radio_network();

  return bearer_context_release_command;
}
