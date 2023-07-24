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

#include "e1ap_cu_cp_test_messages.h"

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::e1ap;

cu_cp_e1_setup_request srsran::srs_cu_cp::generate_cu_cp_e1_setup_request()
{
  cu_cp_e1_setup_request e1_setup_request = {};
  e1_setup_request.gnb_cu_cp_name         = "srsCU-CP";

  return e1_setup_request;
}

asn1::e1ap::supported_plmns_item_s srsran::srs_cu_cp::generate_supported_plmns_item(unsigned nrcell_id)
{
  asn1::e1ap::supported_plmns_item_s supported_plmns_item = {};
  supported_plmns_item.plmn_id.from_string("00f110");

  asn1::e1ap::slice_support_item_s slice_support_item = {};
  slice_support_item.snssai.sst.from_number(1);
  supported_plmns_item.slice_support_list.push_back(slice_support_item);

  asn1::e1ap::nr_cgi_support_item_s nr_cgi_support_item;
  nr_cgi_support_item.nr_cgi.plmn_id.from_string("00f110");
  nr_cgi_support_item.nr_cgi.nr_cell_id.from_number(nrcell_id);
  supported_plmns_item.nr_cgi_support_list.push_back(nr_cgi_support_item);

  supported_plmns_item.qos_params_support_list_present = false;

  return supported_plmns_item;
}

e1ap_message srsran::srs_cu_cp::generate_cu_cp_e1_setup_respose(unsigned transaction_id)
{
  e1ap_message e1_setup_response = {};
  e1_setup_response.pdu.set_successful_outcome();
  e1_setup_response.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_GNB_CU_CP_E1_SETUP);

  auto& setup_resp                   = e1_setup_response.pdu.successful_outcome().value.gnb_cu_cp_e1_setup_resp();
  setup_resp->transaction_id         = transaction_id;
  setup_resp->gnb_cu_up_id           = 1;
  setup_resp->gnb_cu_up_name_present = true;
  setup_resp->gnb_cu_up_name.from_string("srsCU-UP");
  setup_resp->cn_support.value = asn1::e1ap::cn_support_opts::c_5gc;

  setup_resp->supported_plmns.push_back(generate_supported_plmns_item(6576));

  setup_resp->gnb_cu_up_capacity_present = false;

  return e1_setup_response;
}

e1ap_message srsran::srs_cu_cp::generate_cu_cp_e1_setup_failure(unsigned transaction_id)
{
  e1ap_message e1_setup_failure = {};

  e1_setup_failure.pdu.set_unsuccessful_outcome();
  e1_setup_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_E1AP_ID_GNB_CU_CP_E1_SETUP);

  auto& setup_fail           = e1_setup_failure.pdu.unsuccessful_outcome().value.gnb_cu_cp_e1_setup_fail();
  setup_fail->transaction_id = transaction_id;
  setup_fail->cause.set_radio_network();
  setup_fail->cause.radio_network()    = asn1::e1ap::cause_radio_network_opts::options::unspecified;
  setup_fail->time_to_wait_present     = false;
  setup_fail->crit_diagnostics_present = false;
  // add critical diagnostics

  return e1_setup_failure;
}

e1ap_message
srsran::srs_cu_cp::generate_cu_cp_e1_setup_failure_with_time_to_wait(unsigned                   transaction_id,
                                                                     asn1::e1ap::time_to_wait_e time_to_wait)
{
  e1ap_message e1_setup_failure = generate_cu_cp_e1_setup_failure(transaction_id);

  auto& setup_fail                 = e1_setup_failure.pdu.unsuccessful_outcome().value.gnb_cu_cp_e1_setup_fail();
  setup_fail->time_to_wait_present = true;
  setup_fail->time_to_wait.value   = time_to_wait;

  return e1_setup_failure;
}

e1ap_message srsran::srs_cu_cp::generate_cu_up_e1_setup_request_base()
{
  e1ap_message e1_setup_request_base = {};

  e1_setup_request_base.pdu.set_init_msg();
  e1_setup_request_base.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_GNB_CU_UP_E1_SETUP);

  auto& setup_req                   = e1_setup_request_base.pdu.init_msg().value.gnb_cu_up_e1_setup_request();
  setup_req->transaction_id         = 99;
  setup_req->gnb_cu_up_id           = 1;
  setup_req->gnb_cu_up_name_present = true;
  setup_req->gnb_cu_up_name.from_string("srsCU-UP");
  setup_req->cn_support.value = asn1::e1ap::cn_support_opts::c_5gc;

  return e1_setup_request_base;
}

e1ap_message srsran::srs_cu_cp::generate_valid_cu_up_e1_setup_request()
{
  e1ap_message e1_setup_request = generate_cu_up_e1_setup_request_base();
  auto&        setup_req        = e1_setup_request.pdu.init_msg().value.gnb_cu_up_e1_setup_request();

  setup_req->supported_plmns.push_back(generate_supported_plmns_item(6576));

  return e1_setup_request;
}

e1ap_message srsran::srs_cu_cp::generate_cu_up_e1_setup_respose(unsigned transaction_id)
{
  e1ap_message e1_setup_response = {};
  e1_setup_response.pdu.set_successful_outcome();
  e1_setup_response.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_GNB_CU_CP_E1_SETUP);

  auto& setup_resp                   = e1_setup_response.pdu.successful_outcome().value.gnb_cu_up_e1_setup_resp();
  setup_resp->transaction_id         = transaction_id;
  setup_resp->gnb_cu_cp_name_present = true;
  setup_resp->gnb_cu_cp_name.from_string("srsCU-CP");

  return e1_setup_response;
}

e1ap_bearer_context_setup_request srsran::srs_cu_cp::generate_bearer_context_setup_request(ue_index_t ue_index)
{
  e1ap_bearer_context_setup_request request = {};

  request.ue_index                                        = ue_index;
  request.security_info.security_algorithm.ciphering_algo = srsran::security::ciphering_algorithm::nea0;
  request.security_info.up_security_key.encryption_key    = make_byte_buffer("9950ab8083ed034257d900e9a6a06236");
  request.ue_dl_aggregate_maximum_bit_rate                = 300000000;
  request.serving_plmn                                    = "00101";
  request.activity_notif_level                            = "ue";

  e1ap_pdu_session_res_to_setup_item res_to_setup_item;
  res_to_setup_item.pdu_session_id                              = uint_to_pdu_session_id(0);
  res_to_setup_item.pdu_session_type                            = "ipv4";
  res_to_setup_item.snssai.sst                                  = 1;
  res_to_setup_item.snssai.sd                                   = 10203;
  res_to_setup_item.security_ind.integrity_protection_ind       = integrity_protection_indication_t::not_needed;
  res_to_setup_item.security_ind.confidentiality_protection_ind = confidentiality_protection_indication_t::not_needed;
  res_to_setup_item.pdu_session_res_dl_ambr                     = 330000000;
  res_to_setup_item.ng_ul_up_tnl_info = {transport_layer_address{"0.0.0.0"}, int_to_gtpu_teid(0)};

  e1ap_drb_to_setup_item_ng_ran drb_to_setup_item;
  drb_to_setup_item.drb_id                      = uint_to_drb_id(1);
  drb_to_setup_item.sdap_cfg.default_drb        = true;
  drb_to_setup_item.sdap_cfg.sdap_hdr_ul        = sdap_hdr_ul_cfg::present;
  drb_to_setup_item.sdap_cfg.sdap_hdr_dl        = sdap_hdr_dl_cfg::present;
  drb_to_setup_item.pdcp_cfg.pdcp_sn_size_ul    = pdcp_sn_size::size18bits;
  drb_to_setup_item.pdcp_cfg.pdcp_sn_size_dl    = pdcp_sn_size::size18bits;
  drb_to_setup_item.pdcp_cfg.rlc_mod            = pdcp_rlc_mode::am;
  drb_to_setup_item.pdcp_cfg.t_reordering_timer = pdcp_t_reordering::ms100;
  drb_to_setup_item.pdcp_cfg.discard_timer      = pdcp_discard_timer::infinity;

  e1ap_cell_group_info_item cell_group_info_item;
  cell_group_info_item.cell_group_id = 0;
  drb_to_setup_item.cell_group_info.push_back(cell_group_info_item);

  e1ap_qos_flow_qos_param_item qos_flow_info;
  qos_flow_info.qos_flow_id = uint_to_qos_flow_id(8);
  non_dyn_5qi_descriptor_t non_dyn_5qi;
  non_dyn_5qi.five_qi                                                                 = uint_to_five_qi(8);
  qos_flow_info.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi             = non_dyn_5qi;
  qos_flow_info.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.prio_level      = 1;
  qos_flow_info.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.pre_emption_cap = "shall-not-trigger-pre-emption";
  qos_flow_info.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.pre_emption_vulnerability = "not-pre-emptable";
  drb_to_setup_item.qos_flow_info_to_be_setup.emplace(qos_flow_info.qos_flow_id, qos_flow_info);

  res_to_setup_item.drb_to_setup_list_ng_ran.emplace(drb_to_setup_item.drb_id, drb_to_setup_item);

  request.pdu_session_res_to_setup_list.emplace(res_to_setup_item.pdu_session_id, res_to_setup_item);

  return request;
}

e1ap_message srsran::srs_cu_cp::generate_bearer_context_setup_response(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                                       gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id)
{
  e1ap_message bearer_context_setup_response = {};

  bearer_context_setup_response.pdu.set_successful_outcome();
  bearer_context_setup_response.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_SETUP);

  auto& bearer_context_setup_resp =
      bearer_context_setup_response.pdu.successful_outcome().value.bearer_context_setup_resp();
  bearer_context_setup_resp->gnb_cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_to_uint(cu_cp_ue_e1ap_id);
  bearer_context_setup_resp->gnb_cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_to_uint(cu_up_ue_e1ap_id);
  bearer_context_setup_resp->sys_bearer_context_setup_resp.set_ng_ran_bearer_context_setup_resp();

  auto& ng_ran_bearer_context_setup_resp =
      bearer_context_setup_resp->sys_bearer_context_setup_resp.ng_ran_bearer_context_setup_resp();

  asn1::e1ap::pdu_session_res_setup_item_s pdu_session_res_setup_item = {};
  pdu_session_res_setup_item.pdu_session_id                           = 1;

  pdu_session_res_setup_item.ng_dl_up_tnl_info.set_gtp_tunnel();
  auto& gtp_tunnel = pdu_session_res_setup_item.ng_dl_up_tnl_info.gtp_tunnel();
  gtp_tunnel.transport_layer_address.from_number(2887058953);
  gtp_tunnel.gtp_teid.from_string("00000283");

  asn1::e1ap::drb_setup_item_ng_ran_s drb_setup_item_ng_ran = {};
  drb_setup_item_ng_ran.drb_id                              = 1;

  asn1::e1ap::up_params_item_s up_params_item = {};
  up_params_item.up_tnl_info.set_gtp_tunnel();
  auto& gtp_tunnel2 = up_params_item.up_tnl_info.gtp_tunnel();
  gtp_tunnel2.transport_layer_address.from_number(2887058953);
  gtp_tunnel2.gtp_teid.from_string("80000283");
  up_params_item.cell_group_id = 0;
  drb_setup_item_ng_ran.ul_up_transport_params.push_back(up_params_item);

  asn1::e1ap::qos_flow_item_s qo_s_flow_item = {};
  qo_s_flow_item.qos_flow_id                 = 1;
  drb_setup_item_ng_ran.flow_setup_list.push_back(qo_s_flow_item);

  pdu_session_res_setup_item.drb_setup_list_ng_ran.push_back(drb_setup_item_ng_ran);

  ng_ran_bearer_context_setup_resp.pdu_session_res_setup_list.push_back(pdu_session_res_setup_item);

  ng_ran_bearer_context_setup_resp.pdu_session_res_failed_list_present = false;

  return bearer_context_setup_response;
}

e1ap_message srsran::srs_cu_cp::generate_bearer_context_setup_failure(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                                      gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id)
{
  e1ap_message bearer_context_setup_failure = {};

  bearer_context_setup_failure.pdu.set_unsuccessful_outcome();
  bearer_context_setup_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_SETUP);

  auto& bearer_context_setup_fail =
      bearer_context_setup_failure.pdu.unsuccessful_outcome().value.bearer_context_setup_fail();
  bearer_context_setup_fail->gnb_cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_to_uint(cu_cp_ue_e1ap_id);
  bearer_context_setup_fail->gnb_cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_to_uint(cu_up_ue_e1ap_id);
  bearer_context_setup_fail->cause.set_radio_network();
  bearer_context_setup_fail->cause.radio_network() = asn1::e1ap::cause_radio_network_opts::options::unspecified;

  return bearer_context_setup_failure;
}

e1ap_bearer_context_modification_request
srsran::srs_cu_cp::generate_bearer_context_modification_request(ue_index_t ue_index)
{
  e1ap_bearer_context_modification_request request;
  request.ue_index = ue_index;

  return request;
}

e1ap_message srsran::srs_cu_cp::generate_bearer_context_modification_response(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                                              gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id)
{
  e1ap_message bearer_context_modification_response = {};

  bearer_context_modification_response.pdu.set_successful_outcome();
  bearer_context_modification_response.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_MOD);

  auto& bearer_context_mod_resp =
      bearer_context_modification_response.pdu.successful_outcome().value.bearer_context_mod_resp();
  bearer_context_mod_resp->gnb_cu_cp_ue_e1ap_id                = gnb_cu_cp_ue_e1ap_id_to_uint(cu_cp_ue_e1ap_id);
  bearer_context_mod_resp->gnb_cu_up_ue_e1ap_id                = gnb_cu_up_ue_e1ap_id_to_uint(cu_up_ue_e1ap_id);
  bearer_context_mod_resp->sys_bearer_context_mod_resp_present = true;
  bearer_context_mod_resp->sys_bearer_context_mod_resp.set_ng_ran_bearer_context_mod_resp();

  auto& ng_ran_bearer_context_mod_resp =
      bearer_context_mod_resp->sys_bearer_context_mod_resp.ng_ran_bearer_context_mod_resp();
  ng_ran_bearer_context_mod_resp.pdu_session_res_modified_list_present = true;

  asn1::e1ap::pdu_session_res_modified_item_s pdu_session_res_modified_item = {};
  asn1::e1ap::drb_modified_item_ng_ran_s      drb_modified_item_ng_ran;
  drb_modified_item_ng_ran.drb_id             = 1;
  asn1::e1ap::up_params_item_s up_params_item = {};
  auto&                        gtp_tunnel     = up_params_item.up_tnl_info.set_gtp_tunnel();
  gtp_tunnel.transport_layer_address.from_number(2887058953);
  gtp_tunnel.gtp_teid.from_string("80000283");
  up_params_item.cell_group_id = 0;
  drb_modified_item_ng_ran.ul_up_transport_params.push_back(up_params_item);
  pdu_session_res_modified_item.drb_modified_list_ng_ran.push_back(drb_modified_item_ng_ran);
  ng_ran_bearer_context_mod_resp.pdu_session_res_modified_list.push_back(pdu_session_res_modified_item);

  return bearer_context_modification_response;
}

e1ap_message srsran::srs_cu_cp::generate_bearer_context_modification_failure(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                                             gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id)
{
  e1ap_message bearer_context_modification_failure = {};

  bearer_context_modification_failure.pdu.set_unsuccessful_outcome();
  bearer_context_modification_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_MOD);

  auto& bearer_context_mod_fail =
      bearer_context_modification_failure.pdu.unsuccessful_outcome().value.bearer_context_mod_fail();
  bearer_context_mod_fail->gnb_cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_to_uint(cu_cp_ue_e1ap_id);
  bearer_context_mod_fail->gnb_cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_to_uint(cu_up_ue_e1ap_id);
  bearer_context_mod_fail->cause.set_radio_network();
  bearer_context_mod_fail->cause.radio_network() = asn1::e1ap::cause_radio_network_opts::options::unspecified;

  return bearer_context_modification_failure;
}

e1ap_bearer_context_release_command srsran::srs_cu_cp::generate_bearer_context_release_command(ue_index_t ue_index)
{
  e1ap_bearer_context_release_command command;
  command.ue_index = ue_index;
  command.cause    = cause_t::radio_network;

  return command;
}

e1ap_message srsran::srs_cu_cp::generate_bearer_context_release_complete(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                                         gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id)
{
  e1ap_message bearer_ctxt_rel_complete_msg = {};
  bearer_ctxt_rel_complete_msg.pdu.set_successful_outcome();
  bearer_ctxt_rel_complete_msg.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_RELEASE);
  auto& rel_complete_msg =
      bearer_ctxt_rel_complete_msg.pdu.successful_outcome().value.bearer_context_release_complete();
  rel_complete_msg->gnb_cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_to_uint(cu_cp_ue_e1ap_id);
  rel_complete_msg->gnb_cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_to_uint(cu_up_ue_e1ap_id);

  return bearer_ctxt_rel_complete_msg;
}

e1ap_message srsran::srs_cu_cp::generate_bearer_context_inactivity_notification_with_ue_level(
    gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
    gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id)
{
  e1ap_message inactivity_notification = {};

  inactivity_notification.pdu.set_init_msg();
  inactivity_notification.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_INACTIVITY_NOTIF);

  auto& bearer_context_inactivity_notification =
      inactivity_notification.pdu.init_msg().value.bearer_context_inactivity_notif();
  bearer_context_inactivity_notification->gnb_cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_to_uint(cu_cp_ue_e1ap_id);
  bearer_context_inactivity_notification->gnb_cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_to_uint(cu_up_ue_e1ap_id);

  bearer_context_inactivity_notification->activity_info.set_ue_activity();
  bearer_context_inactivity_notification->activity_info.ue_activity() =
      asn1::e1ap::ue_activity_opts::options::not_active;

  return inactivity_notification;
}

e1ap_message
srsran::srs_cu_cp::generate_invalid_bearer_context_inactivity_notification(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                                           gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id)
{
  e1ap_message inactivity_notification = {};

  inactivity_notification.pdu.set_init_msg();
  inactivity_notification.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_INACTIVITY_NOTIF);

  auto& bearer_context_inactivity_notification =
      inactivity_notification.pdu.init_msg().value.bearer_context_inactivity_notif();
  bearer_context_inactivity_notification->gnb_cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_to_uint(cu_cp_ue_e1ap_id);
  bearer_context_inactivity_notification->gnb_cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_to_uint(cu_up_ue_e1ap_id);

  return inactivity_notification;
}
