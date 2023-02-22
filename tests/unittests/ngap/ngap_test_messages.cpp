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

#include "ngap_test_messages.h"

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::ngap;

ng_setup_request srsran::srs_cu_cp::generate_ng_setup_request()
{
  ng_setup_request request_msg = {};
  request_msg.msg              = {};
  request_msg.msg->global_ran_node_id.value.set_global_gnb_id();
  request_msg.msg->global_ran_node_id.value.global_gnb_id().gnb_id.set_gnb_id();
  request_msg.msg->global_ran_node_id.value.global_gnb_id().gnb_id.gnb_id().from_number(411);
  request_msg.msg->global_ran_node_id.value.global_gnb_id().plmn_id.from_string("00f110");

  request_msg.msg->ran_node_name_present = true;
  request_msg.msg->ran_node_name.value.from_string("srsgnb01");

  request_msg.msg->supported_ta_list.id   = ASN1_NGAP_ID_SUPPORTED_TA_LIST;
  request_msg.msg->supported_ta_list.crit = asn1::crit_opts::reject;

  supported_ta_item_s supported_ta_item = {};
  supported_ta_item.tac.from_number(7);
  broadcast_plmn_item_s broadcast_plmn_item = {};
  broadcast_plmn_item.plmn_id.from_string("00f110");
  slice_support_item_s slice_support_item = {};
  slice_support_item.s_nssai.sst.from_number(1);
  broadcast_plmn_item.tai_slice_support_list.push_back(slice_support_item);
  supported_ta_item.broadcast_plmn_list.push_back(broadcast_plmn_item);

  request_msg.msg->supported_ta_list.value.push_back(supported_ta_item);

  request_msg.msg->default_paging_drx.value.value = paging_drx_opts::v256;

  return request_msg;
}

ngap_message srsran::srs_cu_cp::generate_ng_setup_response()
{
  ngap_message ng_setup_response = {};

  ng_setup_response.pdu.set_successful_outcome();
  ng_setup_response.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_NG_SETUP);

  auto& setup_res = ng_setup_response.pdu.successful_outcome().value.ng_setup_resp();
  setup_res->amf_name.value.from_string("open5gs-amf0");

  served_guami_item_s served_guami_item;
  served_guami_item.guami.plmn_id.from_string("00f110");
  served_guami_item.guami.amf_region_id.from_number(2);
  served_guami_item.guami.amf_set_id.from_number(1);
  served_guami_item.guami.amf_pointer.from_number(0);
  setup_res->served_guami_list.value.push_back(served_guami_item);

  setup_res->relative_amf_capacity.value.value = 255;

  plmn_support_item_s plmn_support_item = {};
  plmn_support_item.plmn_id.from_string("00f110");

  slice_support_item_s slice_support_item = {};
  slice_support_item.s_nssai.sst.from_number(1);
  plmn_support_item.slice_support_list.push_back(slice_support_item);

  setup_res->plmn_support_list.value.push_back(plmn_support_item);

  return ng_setup_response;
}

ngap_message srsran::srs_cu_cp::generate_ng_setup_failure()
{
  ngap_message ng_setup_failure = {};

  ng_setup_failure.pdu.set_unsuccessful_outcome();
  ng_setup_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_NGAP_ID_NG_SETUP);

  auto& setup_fail = ng_setup_failure.pdu.unsuccessful_outcome().value.ng_setup_fail();
  setup_fail->cause.value.set_radio_network();
  setup_fail->cause.value.radio_network() = cause_radio_network_opts::options::unspecified;
  setup_fail->time_to_wait_present        = false;
  setup_fail->crit_diagnostics_present    = false;
  // add critical diagnostics

  return ng_setup_failure;
}

ngap_message srsran::srs_cu_cp::generate_ng_setup_failure_with_time_to_wait(time_to_wait_e time_to_wait)
{
  ngap_message ng_setup_failure = generate_ng_setup_failure();

  auto& setup_fail                 = ng_setup_failure.pdu.unsuccessful_outcome().value.ng_setup_fail();
  setup_fail->time_to_wait_present = true;
  setup_fail->time_to_wait.value   = time_to_wait;

  return ng_setup_failure;
}

ngap_initial_ue_message srsran::srs_cu_cp::generate_initial_ue_message(ue_index_t ue_index)
{
  ngap_initial_ue_message msg;
  msg.ue_index = ue_index;
  msg.nas_pdu.resize(nas_pdu_len);
  msg.establishment_cause.value = rrc_establishment_cause_opts::mo_sig;
  // msg.nr_cgi = {};
  msg.tac = 7;
  return msg;
}

ngap_message srsran::srs_cu_cp::generate_downlink_nas_transport_message(amf_ue_id_t amf_ue_id, ran_ue_id_t ran_ue_id)
{
  ngap_message dl_nas_transport = {};

  dl_nas_transport.pdu.set_init_msg();
  dl_nas_transport.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_DL_NAS_TRANSPORT);

  auto& dl_nas_transport_msg                 = dl_nas_transport.pdu.init_msg().value.dl_nas_transport();
  dl_nas_transport_msg->amf_ue_ngap_id.value = amf_ue_id_to_uint(amf_ue_id);
  dl_nas_transport_msg->ran_ue_ngap_id.value = ran_ue_id_to_uint(ran_ue_id);
  dl_nas_transport_msg->nas_pdu.value.resize(nas_pdu_len);

  return dl_nas_transport;
}

ngap_ul_nas_transport_message srsran::srs_cu_cp::generate_ul_nas_transport_message(ue_index_t ue_index)
{
  ngap_ul_nas_transport_message ul_nas_transport;
  ul_nas_transport.ue_index = ue_index;
  ul_nas_transport.nas_pdu.resize(nas_pdu_len);
  // ul_nas_transport.nr_cgi = {};

  return ul_nas_transport;
}

ngap_message srsran::srs_cu_cp::generate_initial_context_setup_request_base(amf_ue_id_t amf_ue_id,
                                                                            ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_INIT_CONTEXT_SETUP);

  auto& init_context_setup_req                 = ngap_msg.pdu.init_msg().value.init_context_setup_request();
  init_context_setup_req->amf_ue_ngap_id.value = amf_ue_id_to_uint(amf_ue_id);
  init_context_setup_req->ran_ue_ngap_id.value = ran_ue_id_to_uint(ran_ue_id);

  init_context_setup_req->guami->plmn_id.from_string("02f899");
  init_context_setup_req->guami->amf_region_id.from_number(128);
  init_context_setup_req->guami->amf_set_id.from_number(1);
  init_context_setup_req->guami->amf_pointer.from_number(1);

  init_context_setup_req->nas_pdu_present = true;
  init_context_setup_req->nas_pdu.value.from_string(
      "7e02c4f6c22f017e0042010177000bf202f8998000410000001054070002f8990000011500210201005e01b6");

  init_context_setup_req->allowed_nssai->resize(1);

  allowed_nssai_item_s allowed_nssai;
  allowed_nssai.s_nssai.sst.from_number(1);
  allowed_nssai.s_nssai.sd_present = true;
  allowed_nssai.s_nssai.sd.from_string("db2700");

  init_context_setup_req->allowed_nssai->push_back(allowed_nssai);

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_valid_initial_context_setup_request_message(amf_ue_id_t amf_ue_id,
                                                                                     ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg = generate_initial_context_setup_request_base(amf_ue_id, ran_ue_id);

  auto& init_context_setup_req = ngap_msg.pdu.init_msg().value.init_context_setup_request();
  init_context_setup_req->ue_security_cap->nr_encryption_algorithms.from_number(57344);
  init_context_setup_req->ue_security_cap->nr_integrity_protection_algorithms.from_number(57344);
  init_context_setup_req->ue_security_cap->eutr_aencryption_algorithms.from_number(57344);
  init_context_setup_req->ue_security_cap->eutr_aintegrity_protection_algorithms.from_number(57344);

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_invalid_initial_context_setup_request_message(amf_ue_id_t amf_ue_id,
                                                                                       ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg = generate_initial_context_setup_request_base(amf_ue_id, ran_ue_id);

  // NIA0 is not allowed
  auto& init_context_setup_req = ngap_msg.pdu.init_msg().value.init_context_setup_request();
  init_context_setup_req->ue_security_cap->nr_encryption_algorithms.from_number(0);
  init_context_setup_req->ue_security_cap->nr_integrity_protection_algorithms.from_number(0);
  init_context_setup_req->ue_security_cap->eutr_aencryption_algorithms.from_number(0);
  init_context_setup_req->ue_security_cap->eutr_aintegrity_protection_algorithms.from_number(0);

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_pdu_session_resource_setup_request_base(amf_ue_id_t amf_ue_id,
                                                                                 ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg;

  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_PDU_SESSION_RES_SETUP);

  auto& pdu_session_res_setup_req                 = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();
  pdu_session_res_setup_req->amf_ue_ngap_id.value = amf_ue_id_to_uint(amf_ue_id);
  pdu_session_res_setup_req->ran_ue_ngap_id.value = ran_ue_id_to_uint(ran_ue_id);

  pdu_session_res_setup_req->ue_aggr_max_bit_rate_present                       = true;
  pdu_session_res_setup_req->ue_aggr_max_bit_rate.value.ue_aggr_max_bit_rate_dl = 300000000U;
  pdu_session_res_setup_req->ue_aggr_max_bit_rate.value.ue_aggr_max_bit_rate_ul = 200000000U;

  return ngap_msg;
}

ngap_message
srsran::srs_cu_cp::generate_valid_pdu_session_resource_setup_request_message(amf_ue_id_t      amf_ue_id,
                                                                             ran_ue_id_t      ran_ue_id,
                                                                             pdu_session_id_t pdu_session_id)
{
  ngap_message ngap_msg = generate_pdu_session_resource_setup_request_base(amf_ue_id, ran_ue_id);

  auto& pdu_session_res_setup_req = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

  pdu_session_res_setup_item_su_req_s pdu_session_res_item;

  pdu_session_res_item.pdu_session_id = pdu_session_id_to_uint(pdu_session_id);

  // Add PDU Session NAS PDU
  pdu_session_res_item.pdu_session_nas_pdu.from_string("7e02e9b0a23c027e006801006e2e0115c211000901000631310101ff08060"
                                                       "6014a06014a2905010c02010c2204010027db79000608204101"
                                                       "01087b002080802110030000108106ac1503648306ac150364000d04ac150"
                                                       "364001002054e251c036f61690469707634066d6e6330393906"
                                                       "6d636332303804677072731201");

  // Add S-NSSAI
  pdu_session_res_item.s_nssai.sst.from_number(1);
  pdu_session_res_item.s_nssai.sd_present = true;
  pdu_session_res_item.s_nssai.sd.from_string("0027db");

  // Add PDU Session Resource Setup Request Transfer
  pdu_session_res_item.pdu_session_res_setup_request_transfer.from_string(
      "0000040082000a0c13ab66803013ab6680008b000a01f0ac150a020000000b00860001000088000700080000080000");

  pdu_session_res_setup_req->pdu_session_res_setup_list_su_req.value.push_back(pdu_session_res_item);

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_invalid_pdu_session_resource_setup_request_message(amf_ue_id_t amf_ue_id,
                                                                                            ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg = generate_pdu_session_resource_setup_request_base(amf_ue_id, ran_ue_id);

  return ngap_msg;
}

cu_cp_pdu_session_resource_setup_response
srsran::srs_cu_cp::generate_cu_cp_pdu_session_resource_setup_response(pdu_session_id_t pdu_session_id)
{
  cu_cp_pdu_session_resource_setup_response pdu_session_res_setup_resp;

  cu_cp_pdu_session_res_setup_response_item pdu_session_setup_response_item;
  pdu_session_setup_response_item.pdu_session_id = pdu_session_id;

  auto& dlqos_flow_per_tnl_info =
      pdu_session_setup_response_item.pdu_session_resource_setup_response_transfer.dlqos_flow_per_tnl_info;
  dlqos_flow_per_tnl_info.up_tp_layer_info = {transport_layer_address{"0.0.0.0"}, int_to_gtp_teid(0)};
  cu_cp_associated_qos_flow assoc_qos_flow;
  assoc_qos_flow.qos_flow_id = uint_to_qos_flow_id(1);
  dlqos_flow_per_tnl_info.associated_qos_flow_list.emplace(uint_to_qos_flow_id(1), assoc_qos_flow);

  pdu_session_res_setup_resp.pdu_session_res_setup_response_items.emplace(pdu_session_id,
                                                                          pdu_session_setup_response_item);

  return pdu_session_res_setup_resp;
}