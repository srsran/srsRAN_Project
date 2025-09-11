/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "lib/ngap/ngap_asn1_converters.h"
#include "srsran/asn1/ngap/common.h"
#include "srsran/asn1/ngap/ngap_ies.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/ngap/ngap_handover.h"
#include "srsran/ngap/ngap_message.h"
#include "srsran/ngap/ngap_types.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/plmn_identity.h"
#include <vector>

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::ngap;

bool srsran::srs_cu_cp::is_same_pdu_type(const ngap_message& lhs, const ngap_message& rhs)
{
  if (lhs.pdu.type().value != rhs.pdu.type().value) {
    return false;
  }
  switch (lhs.pdu.type().value) {
    case ngap_pdu_c::types_opts::init_msg:
      return lhs.pdu.init_msg().value.type().value == rhs.pdu.init_msg().value.type().value;
    case ngap_pdu_c::types_opts::successful_outcome:
      return lhs.pdu.successful_outcome().value.type().value == rhs.pdu.successful_outcome().value.type().value;
    case ngap_pdu_c::types_opts::unsuccessful_outcome:
      return lhs.pdu.unsuccessful_outcome().value.type().value == rhs.pdu.unsuccessful_outcome().value.type().value;
    default:
      break;
  }
  return false;
}

bool srsran::srs_cu_cp::is_pdu_type(const ngap_message&                                    pdu,
                                    const asn1::ngap::ngap_elem_procs_o::init_msg_c::types type)
{
  if (pdu.pdu.type().value != ngap_pdu_c::types_opts::init_msg) {
    return false;
  }
  return pdu.pdu.init_msg().value.type().value == type;
}

bool srsran::srs_cu_cp::is_pdu_type(const ngap_message&                                              pdu,
                                    const asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types type)
{
  if (pdu.pdu.type().value != ngap_pdu_c::types_opts::successful_outcome) {
    return false;
  }
  return pdu.pdu.successful_outcome().value.type().value == type;
}

ngap_message srsran::srs_cu_cp::generate_ng_setup_response(plmn_identity plmn)
{
  ngap_message ng_setup_response = {};

  ng_setup_response.pdu.set_successful_outcome();
  ng_setup_response.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_NG_SETUP);

  auto& setup_res = ng_setup_response.pdu.successful_outcome().value.ng_setup_resp();
  setup_res->amf_name.from_string("open5gs-amf0");

  served_guami_item_s served_guami_item;
  served_guami_item.guami.plmn_id = plmn.to_bytes();
  served_guami_item.guami.amf_region_id.from_number(2);
  served_guami_item.guami.amf_set_id.from_number(1);
  served_guami_item.guami.amf_pointer.from_number(0);
  setup_res->served_guami_list.push_back(served_guami_item);

  setup_res->relative_amf_capacity = 255;

  plmn_support_item_s plmn_support_item = {};
  plmn_support_item.plmn_id             = plmn.to_bytes();

  slice_support_item_s slice_support_item = {};
  slice_support_item.s_nssai.sst.from_number(1);
  plmn_support_item.slice_support_list.push_back(slice_support_item);

  setup_res->plmn_support_list.push_back(plmn_support_item);

  return ng_setup_response;
}

ngap_message srsran::srs_cu_cp::generate_ng_setup_failure()
{
  ngap_message ng_setup_failure = {};

  ng_setup_failure.pdu.set_unsuccessful_outcome();
  ng_setup_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_NGAP_ID_NG_SETUP);

  auto& setup_fail = ng_setup_failure.pdu.unsuccessful_outcome().value.ng_setup_fail();
  setup_fail->cause.set_radio_network();
  setup_fail->cause.radio_network()    = cause_radio_network_opts::options::unspecified;
  setup_fail->time_to_wait_present     = false;
  setup_fail->crit_diagnostics_present = false;
  // TODO: Add critical diagnostics.

  return ng_setup_failure;
}

ngap_message srsran::srs_cu_cp::generate_ng_setup_failure_with_bad_plmn(time_to_wait_e time_to_wait)
{
  ngap_message ng_setup_failure = {};

  ng_setup_failure.pdu.set_unsuccessful_outcome();
  ng_setup_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_NGAP_ID_NG_SETUP);

  auto& setup_fail = ng_setup_failure.pdu.unsuccessful_outcome().value.ng_setup_fail();
  setup_fail->cause.set_misc();
  setup_fail->cause.misc()             = asn1::ngap::cause_misc_opts::unknown_plmn_or_sn_pn;
  setup_fail->time_to_wait_present     = true;
  setup_fail->time_to_wait.value       = time_to_wait;
  setup_fail->crit_diagnostics_present = false;
  // Fill critical diagnostics

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

cu_cp_initial_ue_message srsran::srs_cu_cp::generate_initial_ue_message(ue_index_t ue_index)
{
  cu_cp_initial_ue_message msg = {};
  msg.ue_index                 = ue_index;
  (void)msg.nas_pdu.resize(nas_pdu_len);
  msg.establishment_cause               = static_cast<establishment_cause_t>(rrc_establishment_cause_opts::mo_sig);
  msg.user_location_info.nr_cgi.plmn_id = plmn_identity::test_value();
  msg.user_location_info.nr_cgi.nci     = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
  msg.user_location_info.tai.plmn_id    = plmn_identity::test_value();
  msg.user_location_info.tai.tac        = 7;
  return msg;
}

ngap_message srsran::srs_cu_cp::generate_downlink_nas_transport_message(amf_ue_id_t amf_ue_id,
                                                                        ran_ue_id_t ran_ue_id,
                                                                        byte_buffer nas_pdu)
{
  ngap_message dl_nas_transport = {};

  dl_nas_transport.pdu.set_init_msg();
  dl_nas_transport.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_DL_NAS_TRANSPORT);

  auto& dl_nas_transport_msg           = dl_nas_transport.pdu.init_msg().value.dl_nas_transport();
  dl_nas_transport_msg->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  dl_nas_transport_msg->ran_ue_ngap_id = ran_ue_id_to_uint(ran_ue_id);
  if (nas_pdu.empty()) {
    (void)dl_nas_transport_msg->nas_pdu.resize(nas_pdu_len);
  } else {
    dl_nas_transport_msg->nas_pdu = nas_pdu.copy();
  }

  return dl_nas_transport;
}

ngap_message srsran::srs_cu_cp::generate_downlink_nas_transport_message_with_ue_cap_info_request(amf_ue_id_t amf_ue_id,
                                                                                                 ran_ue_id_t ran_ue_id,
                                                                                                 byte_buffer nas_pdu)
{
  ngap_message dl_nas_transport = generate_downlink_nas_transport_message(amf_ue_id, ran_ue_id, std::move(nas_pdu));

  auto& dl_nas_transport_msg                        = dl_nas_transport.pdu.init_msg().value.dl_nas_transport();
  dl_nas_transport_msg->ue_cap_info_request_present = true;
  dl_nas_transport_msg->ue_cap_info_request         = ue_cap_info_request_opts::options::requested;

  return dl_nas_transport;
}

cu_cp_ul_nas_transport srsran::srs_cu_cp::generate_ul_nas_transport_message(ue_index_t ue_index)
{
  cu_cp_ul_nas_transport ul_nas_transport = {};
  ul_nas_transport.ue_index               = ue_index;
  (void)ul_nas_transport.nas_pdu.resize(nas_pdu_len);
  ul_nas_transport.user_location_info.nr_cgi.plmn_id = plmn_identity::test_value();
  ul_nas_transport.user_location_info.nr_cgi.nci     = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
  ul_nas_transport.user_location_info.tai.plmn_id    = plmn_identity::test_value();
  ul_nas_transport.user_location_info.tai.tac        = 7;

  return ul_nas_transport;
}

ngap_message srsran::srs_cu_cp::generate_uplink_nas_transport_message(amf_ue_id_t amf_ue_id, ran_ue_id_t ran_ue_id)
{
  ngap_message ul_nas_transport = {};

  ul_nas_transport.pdu.set_init_msg();
  ul_nas_transport.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_UL_NAS_TRANSPORT);

  auto& ul_nas_transport_msg           = ul_nas_transport.pdu.init_msg().value.ul_nas_transport();
  ul_nas_transport_msg->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  ul_nas_transport_msg->ran_ue_ngap_id = ran_ue_id_to_uint(ran_ue_id);
  (void)ul_nas_transport_msg->nas_pdu.resize(nas_pdu_len);

  auto& user_loc_info_nr = ul_nas_transport_msg->user_location_info.set_user_location_info_nr();
  user_loc_info_nr.nr_cgi.plmn_id.from_string("00f110");
  user_loc_info_nr.nr_cgi.nr_cell_id.from_number(nr_cell_identity::create(gnb_id_t{411, 22}, 0).value().value());
  user_loc_info_nr.tai.plmn_id.from_string("00f110");
  user_loc_info_nr.tai.tac.from_number(7);

  return ul_nas_transport;
}

ngap_message srsran::srs_cu_cp::generate_initial_context_setup_request_base(amf_ue_id_t amf_ue_id,
                                                                            ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_INIT_CONTEXT_SETUP);

  auto& init_context_setup_req           = ngap_msg.pdu.init_msg().value.init_context_setup_request();
  init_context_setup_req->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  init_context_setup_req->ran_ue_ngap_id = ran_ue_id_to_uint(ran_ue_id);

  init_context_setup_req->guami.plmn_id.from_string("00f110");
  init_context_setup_req->guami.amf_region_id.from_number(4);
  init_context_setup_req->guami.amf_set_id.from_number(1);
  init_context_setup_req->guami.amf_pointer.from_number(0);

  init_context_setup_req->nas_pdu_present = true;
  init_context_setup_req->nas_pdu.from_string(
      "7e02c4f6c22f017e0042010177000bf202f8998000410000001054070002f8990000011500210201005e01b6");

  allowed_nssai_item_s allowed_nssai = {};
  allowed_nssai.s_nssai.sst.from_number(1);
  allowed_nssai.s_nssai.sd_present = true;
  allowed_nssai.s_nssai.sd.from_string("db2700");

  init_context_setup_req->allowed_nssai.push_back(allowed_nssai);

  init_context_setup_req->masked_imeisv_present = true;
  init_context_setup_req->masked_imeisv.from_string(
      "0000000100100011010001010110011100000000111111111111111100000001"); // 0123456700ffff01

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_valid_initial_context_setup_request_message(amf_ue_id_t amf_ue_id,
                                                                                     ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg = generate_initial_context_setup_request_base(amf_ue_id, ran_ue_id);

  auto& init_context_setup_req = ngap_msg.pdu.init_msg().value.init_context_setup_request();
  init_context_setup_req->ue_security_cap.nr_encryption_algorithms.from_number(49152);
  init_context_setup_req->ue_security_cap.nr_integrity_protection_algorithms.from_number(49152);
  init_context_setup_req->ue_security_cap.eutr_aencryption_algorithms.from_number(0);
  init_context_setup_req->ue_security_cap.eutr_aintegrity_protection_algorithms.from_number(0);

  init_context_setup_req->security_key.from_string(
      "1111111000001101100111110001011010001110110010111010001100110111100111011000110110011010000110000011000010111000"
      "0010001100001010000001111111100000100111101011000011110000110101110010001010001010101000101100101100100000001110"
      "00010001000110001101110101100110"); // fe0d9f168ecba3379d8d9a1830b8230a07f827ac3c35c8a2a8b2c80e1118dd66

  return ngap_msg;
}

ngap_message
srsran::srs_cu_cp::generate_valid_initial_context_setup_request_message_with_pdu_session(amf_ue_id_t amf_ue_id,
                                                                                         ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg = generate_valid_initial_context_setup_request_message(amf_ue_id, ran_ue_id);

  auto& init_context_setup_req = ngap_msg.pdu.init_msg().value.init_context_setup_request();
  init_context_setup_req->pdu_session_res_setup_list_cxt_req_present = true;

  asn1::ngap::pdu_session_res_setup_item_cxt_req_s pdu_session_item;
  pdu_session_item.pdu_session_id = 1U;
  pdu_session_item.s_nssai.sst.from_number(1U);
  pdu_session_item.pdu_session_res_setup_request_transfer =
      make_byte_buffer("0000040082000a0c400000003040000000008b000a01f00a3213020000049a00860001000088000700010000091c00")
          .value();

  init_context_setup_req->pdu_session_res_setup_list_cxt_req.push_back(pdu_session_item);

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_valid_ue_context_release_command_with_amf_ue_ngap_id(amf_ue_id_t amf_ue_id)
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_UE_CONTEXT_RELEASE);

  auto& ue_context_release_cmd                             = ngap_msg.pdu.init_msg().value.ue_context_release_cmd();
  ue_context_release_cmd->ue_ngap_ids.set_amf_ue_ngap_id() = amf_ue_id_to_uint(amf_ue_id);
  auto& cause                                              = ue_context_release_cmd->cause.set_radio_network();
  cause = asn1::ngap::cause_radio_network_opts::options::radio_conn_with_ue_lost;

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_valid_ue_context_release_command_with_ue_ngap_id_pair(amf_ue_id_t amf_ue_id,
                                                                                               ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_UE_CONTEXT_RELEASE);

  auto& ue_context_release_cmd = ngap_msg.pdu.init_msg().value.ue_context_release_cmd();
  auto& ue_id_pair             = ue_context_release_cmd->ue_ngap_ids.set_ue_ngap_id_pair();
  ue_id_pair.amf_ue_ngap_id    = amf_ue_id_to_uint(amf_ue_id);
  ue_id_pair.ran_ue_ngap_id    = ran_ue_id_to_uint(ran_ue_id);
  auto& cause                  = ue_context_release_cmd->cause.set_radio_network();
  cause                        = asn1::ngap::cause_radio_network_opts::options::radio_conn_with_ue_lost;

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_invalid_initial_context_setup_request_message(amf_ue_id_t amf_ue_id,
                                                                                       ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg = generate_initial_context_setup_request_base(amf_ue_id, ran_ue_id);

  // NIA0 is not allowed.
  auto& init_context_setup_req = ngap_msg.pdu.init_msg().value.init_context_setup_request();
  init_context_setup_req->ue_security_cap.nr_encryption_algorithms.from_number(0);
  init_context_setup_req->ue_security_cap.nr_integrity_protection_algorithms.from_number(0);
  init_context_setup_req->ue_security_cap.eutr_aencryption_algorithms.from_number(0);
  init_context_setup_req->ue_security_cap.eutr_aintegrity_protection_algorithms.from_number(0);

  return ngap_msg;
}

ngap_message
srsran::srs_cu_cp::generate_invalid_initial_context_setup_request_message_with_pdu_session(amf_ue_id_t amf_ue_id,
                                                                                           ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg = generate_invalid_initial_context_setup_request_message(amf_ue_id, ran_ue_id);

  auto& init_context_setup_req = ngap_msg.pdu.init_msg().value.init_context_setup_request();
  init_context_setup_req->pdu_session_res_setup_list_cxt_req_present = true;

  asn1::ngap::pdu_session_res_setup_item_cxt_req_s pdu_session_item;
  pdu_session_item.pdu_session_id = 1U;
  pdu_session_item.s_nssai.sst.from_number(1U);
  pdu_session_item.pdu_session_res_setup_request_transfer =
      make_byte_buffer("0000040082000a0c400000003040000000008b000a01f00a3213020000049a00860001000088000700010000091c00")
          .value();

  init_context_setup_req->pdu_session_res_setup_list_cxt_req.push_back(pdu_session_item);

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_pdu_session_resource_setup_request_base(amf_ue_id_t amf_ue_id,
                                                                                 ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg;

  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_PDU_SESSION_RES_SETUP);

  auto& pdu_session_res_setup_req           = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();
  pdu_session_res_setup_req->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  pdu_session_res_setup_req->ran_ue_ngap_id = ran_ue_id_to_uint(ran_ue_id);

  pdu_session_res_setup_req->ue_aggr_max_bit_rate_present                 = true;
  pdu_session_res_setup_req->ue_aggr_max_bit_rate.ue_aggr_max_bit_rate_dl = 300000000U;
  pdu_session_res_setup_req->ue_aggr_max_bit_rate.ue_aggr_max_bit_rate_ul = 200000000U;

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_valid_pdu_session_resource_setup_request_message(
    amf_ue_id_t                                                amf_ue_id,
    ran_ue_id_t                                                ran_ue_id,
    const std::map<pdu_session_id_t, pdu_session_test_params>& pdu_sessions)
{
  ngap_message ngap_msg = generate_pdu_session_resource_setup_request_base(amf_ue_id, ran_ue_id);

  auto& pdu_session_res_setup_req = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

  for (const auto& [pdu_session_id, pdu_session_params] : pdu_sessions) {
    pdu_session_res_setup_item_su_req_s pdu_session_res_item;

    pdu_session_res_item.pdu_session_id = pdu_session_id_to_uint(pdu_session_id);

    // Fill PDU session NAS PDU.
    pdu_session_res_item.pdu_session_nas_pdu.from_string("7e02e9b0a23c027e006801006e2e0115c211000901000631310101ff08060"
                                                         "6014a06014a2905010c02010c2204010027db79000608204101"
                                                         "01087b002080802110030000108106ac1503648306ac150364000d04ac150"
                                                         "364001002054e251c036f61690469707634066d6e6330393906"
                                                         "6d636332303804677072731201");

    // Fill S-NSSAI.
    pdu_session_res_item.s_nssai.sst.from_number(1);
    pdu_session_res_item.s_nssai.sd_present = true;
    pdu_session_res_item.s_nssai.sd.from_string("0027db");

    // Fill PDU session resource setup request transfer.
    asn1::ngap::pdu_session_res_setup_request_transfer_s asn1_setup_req_transfer;
    {
      // Fill PDU session aggregate maximum bit rate.
      asn1_setup_req_transfer->pdu_session_aggr_max_bit_rate_present                          = true;
      asn1_setup_req_transfer->pdu_session_aggr_max_bit_rate.pdu_session_aggr_max_bit_rate_dl = 1000000000U;
      asn1_setup_req_transfer->pdu_session_aggr_max_bit_rate.pdu_session_aggr_max_bit_rate_ul = 1000000000U;

      // Fill UL NGU UP TNL Info.
      asn1_setup_req_transfer->ul_ngu_up_tnl_info.set_gtp_tunnel();
      auto addr = transport_layer_address::create_from_string("127.0.0.1");
      asn1_setup_req_transfer->ul_ngu_up_tnl_info.gtp_tunnel().transport_layer_address.from_string(addr.to_bitstring());
      asn1_setup_req_transfer->ul_ngu_up_tnl_info.gtp_tunnel().gtp_teid.from_number(0x00005e6c);

      // Fill PDU session type.
      asn1_setup_req_transfer->pdu_session_type = pdu_session_type_to_asn1(pdu_session_params.pdu_session_type);

      // Fill QoS flow setup request list.
      for (const auto& qos_flow_test_item : pdu_session_params.qos_flows) {
        qos_flow_setup_request_item_s qos_flow_setup_req_item;
        qos_flow_setup_req_item.qos_flow_id = qos_flow_id_to_uint(qos_flow_test_item.qos_flow_id);

        // Fill QoS characteristics.
        qos_flow_setup_req_item.qos_flow_level_qos_params.qos_characteristics.set_non_dyn5qi();
        qos_flow_setup_req_item.qos_flow_level_qos_params.qos_characteristics.non_dyn5qi().five_qi =
            qos_flow_test_item.five_qi;

        // Fill allocation and retention priority.
        qos_flow_setup_req_item.qos_flow_level_qos_params.alloc_and_retention_prio.prio_level_arp = 8;
        qos_flow_setup_req_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_cap =
            pre_emption_cap_opts::shall_not_trigger_pre_emption;
        qos_flow_setup_req_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_vulnerability =
            pre_emption_vulnerability_opts::not_pre_emptable;

        // Fill gbr QoS info.
        qos_flow_setup_req_item.qos_flow_level_qos_params.gbr_qos_info_present                     = true;
        qos_flow_setup_req_item.qos_flow_level_qos_params.gbr_qos_info.max_flow_bit_rate_dl        = 1000000000U;
        qos_flow_setup_req_item.qos_flow_level_qos_params.gbr_qos_info.max_flow_bit_rate_ul        = 1000000000U;
        qos_flow_setup_req_item.qos_flow_level_qos_params.gbr_qos_info.guaranteed_flow_bit_rate_dl = 1000000000U;
        qos_flow_setup_req_item.qos_flow_level_qos_params.gbr_qos_info.guaranteed_flow_bit_rate_ul = 1000000000U;

        asn1_setup_req_transfer->qos_flow_setup_request_list.push_back(qos_flow_setup_req_item);
      }
    }
    pdu_session_res_item.pdu_session_res_setup_request_transfer = pack_into_pdu(asn1_setup_req_transfer);

    pdu_session_res_setup_req->pdu_session_res_setup_list_su_req.push_back(pdu_session_res_item);
  }

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_invalid_pdu_session_resource_setup_request_message(amf_ue_id_t amf_ue_id,
                                                                                            ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg = generate_pdu_session_resource_setup_request_base(amf_ue_id, ran_ue_id);

  auto& pdu_session_res_setup_req = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

  // Fill PDU sessions with duplicate ids.
  for (auto it = 0; it < 2; ++it) {
    pdu_session_res_setup_item_su_req_s pdu_session_res_item;

    pdu_session_res_item.pdu_session_id = 1;

    // Fill PDU session NAS PDU.
    pdu_session_res_item.pdu_session_nas_pdu.from_string("7e02e9b0a23c027e006801006e2e0115c211000901000631310101ff08060"
                                                         "6014a06014a2905010c02010c2204010027db79000608204101"
                                                         "01087b002080802110030000108106ac1503648306ac150364000d04ac150"
                                                         "364001002054e251c036f61690469707634066d6e6330393906"
                                                         "6d636332303804677072731201");

    // Fill S-NSSAI.
    pdu_session_res_item.s_nssai.sst.from_number(1);
    pdu_session_res_item.s_nssai.sd_present = true;
    pdu_session_res_item.s_nssai.sd.from_string("0027db");

    // Fill PDU session resource setup request transfer.
    pdu_session_res_item.pdu_session_res_setup_request_transfer.from_string(
        "0000040082000a0c400000003040000000008b000a01f00a321302000028d600860001000088000700010000091c00");

    pdu_session_res_setup_req->pdu_session_res_setup_list_su_req.push_back(pdu_session_res_item);
  }

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::
    generate_pdu_session_resource_setup_request_with_pdu_session_type_ipv4_and_ipv4v6_transport_layer_address(
        amf_ue_id_t amf_ue_id,
        ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg = generate_valid_pdu_session_resource_setup_request_message(
      amf_ue_id, ran_ue_id, {{uint_to_pdu_session_id(1), {pdu_session_type_t::ipv4, {{uint_to_qos_flow_id(1), 9}}}}});

  // Fill invalid PDU session resource setup request transfer.
  auto& pdu_session_res_setup_req = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();
  pdu_session_res_setup_req->pdu_session_res_setup_list_su_req.begin()
      ->pdu_session_res_setup_request_transfer.from_string("0000040082000a0c1dcd6500301dcd6500008b001a09f00a0c01bbfdf66"
                                                           "cf3768500000000000000000bf1010000010086000100008800"
                                                           "0700010000093800");

  return ngap_msg;
}

cu_cp_pdu_session_resource_setup_response
srsran::srs_cu_cp::generate_cu_cp_pdu_session_resource_setup_response(cu_cp_pdu_session_resource_setup_request& request)
{
  cu_cp_pdu_session_resource_setup_response pdu_session_res_setup_resp;

  for (const auto& pdu_session : request.pdu_session_res_setup_items) {
    cu_cp_pdu_session_res_setup_response_item pdu_session_setup_response_item;
    pdu_session_setup_response_item.pdu_session_id = pdu_session.pdu_session_id;

    auto& dlqos_flow_per_tnl_info =
        pdu_session_setup_response_item.pdu_session_resource_setup_response_transfer.dlqos_flow_per_tnl_info;
    dlqos_flow_per_tnl_info.up_tp_layer_info = {transport_layer_address::create_from_string("0.0.0.0"),
                                                int_to_gtpu_teid(0)};
    cu_cp_associated_qos_flow assoc_qos_flow;
    assoc_qos_flow.qos_flow_id = uint_to_qos_flow_id(1);
    dlqos_flow_per_tnl_info.associated_qos_flow_list.emplace(uint_to_qos_flow_id(1), assoc_qos_flow);

    pdu_session_res_setup_resp.pdu_session_res_setup_response_items.emplace(pdu_session.pdu_session_id,
                                                                            pdu_session_setup_response_item);
  }

  return pdu_session_res_setup_resp;
}

ngap_message srsran::srs_cu_cp::generate_pdu_session_resource_release_command_base(amf_ue_id_t amf_ue_id,
                                                                                   ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg;

  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_PDU_SESSION_RES_RELEASE);

  auto& pdu_session_res_release_cmd           = ngap_msg.pdu.init_msg().value.pdu_session_res_release_cmd();
  pdu_session_res_release_cmd->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  pdu_session_res_release_cmd->ran_ue_ngap_id = ran_ue_id_to_uint(ran_ue_id);

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_valid_pdu_session_resource_release_command(amf_ue_id_t      amf_ue_id,
                                                                                    ran_ue_id_t      ran_ue_id,
                                                                                    pdu_session_id_t pdu_session_id)
{
  ngap_message ngap_msg = generate_pdu_session_resource_release_command_base(amf_ue_id, ran_ue_id);

  auto& pdu_session_res_release_cmd = ngap_msg.pdu.init_msg().value.pdu_session_res_release_cmd();

  // Fill PDU session NAS PDU.
  pdu_session_res_release_cmd->nas_pdu_present = true;
  pdu_session_res_release_cmd->nas_pdu         = make_byte_buffer("7e02bcb47dc1137e00680100052e01b3d3241201").value();

  // Fill  PDU session resource to release list.
  asn1::ngap::pdu_session_res_to_release_item_rel_cmd_s pdu_session_res_to_release_item_rel_cmd;
  pdu_session_res_to_release_item_rel_cmd.pdu_session_id                       = pdu_session_id_to_uint(pdu_session_id);
  pdu_session_res_to_release_item_rel_cmd.pdu_session_res_release_cmd_transfer = make_byte_buffer("10").value();
  pdu_session_res_release_cmd->pdu_session_res_to_release_list_rel_cmd.push_back(
      pdu_session_res_to_release_item_rel_cmd);

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_invalid_pdu_session_resource_release_command(amf_ue_id_t amf_ue_id,
                                                                                      ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg = generate_pdu_session_resource_release_command_base(amf_ue_id, ran_ue_id);

  return ngap_msg;
}

cu_cp_pdu_session_resource_release_response
srsran::srs_cu_cp::generate_cu_cp_pdu_session_resource_release_response(pdu_session_id_t pdu_session_id)
{
  cu_cp_pdu_session_resource_release_response pdu_session_res_release_resp;

  cu_cp_pdu_session_res_released_item_rel_res pdu_session_res_released_item_rel_res;

  pdu_session_res_released_item_rel_res.pdu_session_id = pdu_session_id;

  pdu_session_res_release_resp.released_pdu_sessions.emplace(pdu_session_id, pdu_session_res_released_item_rel_res);

  return pdu_session_res_release_resp;
}

ngap_message srsran::srs_cu_cp::generate_pdu_session_resource_modify_request_base(amf_ue_id_t amf_ue_id,
                                                                                  ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg;

  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_PDU_SESSION_RES_MODIFY);

  auto& pdu_session_res_modify_req           = ngap_msg.pdu.init_msg().value.pdu_session_res_modify_request();
  pdu_session_res_modify_req->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  pdu_session_res_modify_req->ran_ue_ngap_id = ran_ue_id_to_uint(ran_ue_id);

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_valid_pdu_session_resource_modify_request_message(
    amf_ue_id_t                       amf_ue_id,
    ran_ue_id_t                       ran_ue_id,
    pdu_session_id_t                  pdu_session_id,
    const std::vector<qos_flow_id_t>& qos_flow_add_or_modify_list,
    const std::vector<qos_flow_id_t>& qos_flow_to_release_list)
{
  ngap_message ngap_msg = generate_pdu_session_resource_modify_request_base(amf_ue_id, ran_ue_id);

  auto& pdu_session_res_modify_req = ngap_msg.pdu.init_msg().value.pdu_session_res_modify_request();

  pdu_session_res_modify_item_mod_req_s pdu_session_res_item;

  pdu_session_res_item.pdu_session_id = pdu_session_id_to_uint(pdu_session_id);

  // Fill PDU session resource modify request transfer.
  asn1::ngap::pdu_session_res_modify_request_transfer_s pdu_session_res_modify_request_transfer;
  {
    // Fill QoS flow add or modify.
    pdu_session_res_modify_request_transfer->qos_flow_add_or_modify_request_list_present =
        !qos_flow_add_or_modify_list.empty();
    for (const auto& qos_flow_id : qos_flow_add_or_modify_list) {
      // Fill QoS flow modify request list.

      asn1::ngap::qos_flow_add_or_modify_request_item_s qos_flow_add_item;

      // Fill QoS flow identifier.
      qos_flow_add_item.qos_flow_id = qos_flow_id_to_uint(qos_flow_id);

      // Fill QoS characteristics.
      qos_flow_add_item.qos_flow_level_qos_params_present = true;
      qos_flow_add_item.qos_flow_level_qos_params.qos_characteristics.set_non_dyn5qi();
      qos_flow_add_item.qos_flow_level_qos_params.qos_characteristics.non_dyn5qi().five_qi = 9;

      // Fill allocation and retention priority.
      qos_flow_add_item.qos_flow_level_qos_params.alloc_and_retention_prio.prio_level_arp = 8;
      qos_flow_add_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_cap =
          asn1::ngap::pre_emption_cap_opts::shall_not_trigger_pre_emption;
      qos_flow_add_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_vulnerability =
          asn1::ngap::pre_emption_vulnerability_opts::not_pre_emptable;

      pdu_session_res_modify_request_transfer->qos_flow_add_or_modify_request_list.push_back(qos_flow_add_item);
    }

    // Fill QoS flow to release.
    pdu_session_res_modify_request_transfer->qos_flow_to_release_list_present = !qos_flow_to_release_list.empty();
    for (const auto& qos_flow_id : qos_flow_to_release_list) {
      asn1::ngap::qos_flow_with_cause_item_s qos_flow_release_item;

      // Fill QoS flow identifier.
      qos_flow_release_item.qos_flow_id = qos_flow_id_to_uint(qos_flow_id);

      // Fill cause.
      qos_flow_release_item.cause.set_radio_network();
      qos_flow_release_item.cause.radio_network() = asn1::ngap::cause_radio_network_opts::options::unspecified;

      pdu_session_res_modify_request_transfer->qos_flow_to_release_list.push_back(qos_flow_release_item);
    }
  }

  pdu_session_res_item.pdu_session_res_modify_request_transfer = pack_into_pdu(pdu_session_res_modify_request_transfer);

  pdu_session_res_modify_req->pdu_session_res_modify_list_mod_req.push_back(pdu_session_res_item);

  return ngap_msg;
}

ngap_message
srsran::srs_cu_cp::generate_invalid_pdu_session_resource_modify_request_message(amf_ue_id_t      amf_ue_id,
                                                                                ran_ue_id_t      ran_ue_id,
                                                                                pdu_session_id_t pdu_session_id)
{
  ngap_message ngap_msg = generate_pdu_session_resource_modify_request_base(amf_ue_id, ran_ue_id);

  auto& pdu_session_res_modify_req = ngap_msg.pdu.init_msg().value.pdu_session_res_modify_request();

  // Fill pdu sessions with duplicate IDs.
  for (auto it = 0; it < 2; ++it) {
    pdu_session_res_modify_item_mod_req_s pdu_session_res_item;

    pdu_session_res_item.pdu_session_id = pdu_session_id_to_uint(pdu_session_id);

    // Fill PDU session resource modify request transfer.
    pdu_session_res_modify_request_transfer_s pdu_session_res_modify_request_transfer;

    // Fill QoS flow add or modify request item.
    pdu_session_res_modify_request_transfer->qos_flow_add_or_modify_request_list_present = true;
    qos_flow_add_or_modify_request_item_s qos_flow_add_item;

    // Fill QoS flow identifier.
    qos_flow_add_item.qos_flow_id = 1;

    pdu_session_res_modify_request_transfer->qos_flow_add_or_modify_request_list.push_back(qos_flow_add_item);

    pdu_session_res_item.pdu_session_res_modify_request_transfer =
        pack_into_pdu(pdu_session_res_modify_request_transfer);

    pdu_session_res_modify_req->pdu_session_res_modify_list_mod_req.push_back(pdu_session_res_item);
  }

  return ngap_msg;
}

cu_cp_pdu_session_resource_modify_response
srsran::srs_cu_cp::generate_cu_cp_pdu_session_resource_modify_response(pdu_session_id_t pdu_session_id,
                                                                       qos_flow_id_t    qos_flow_id)
{
  cu_cp_pdu_session_resource_modify_response pdu_session_res_modify_resp;

  cu_cp_pdu_session_resource_modify_response_item pdu_session_modify_response_item;
  pdu_session_modify_response_item.pdu_session_id = pdu_session_id;

  cu_cp_qos_flow_per_tnl_information qos_flow_per_tnl_info;
  qos_flow_per_tnl_info.up_tp_layer_info = {transport_layer_address::create_from_string("127.0.0.1"),
                                            int_to_gtpu_teid(1)};

  cu_cp_associated_qos_flow assoc_qos_flow;
  assoc_qos_flow.qos_flow_id = qos_flow_id;

  qos_flow_per_tnl_info.associated_qos_flow_list.emplace(qos_flow_id, assoc_qos_flow);

  pdu_session_modify_response_item.transfer.add_dl_qos_flow_per_tnl_info.push_back(qos_flow_per_tnl_info);

  pdu_session_res_modify_resp.pdu_session_res_modify_list.emplace(pdu_session_id, pdu_session_modify_response_item);

  return pdu_session_res_modify_resp;
}

ngap_message srsran::srs_cu_cp::generate_valid_minimal_paging_message()
{
  ngap_message ngap_msg;

  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_PAGING);
  auto& paging = ngap_msg.pdu.init_msg().value.paging();

  // Fill UE paging id.
  auto& five_g_s_tmsi = paging->ue_paging_id.set_five_g_s_tmsi();
  five_g_s_tmsi.amf_set_id.from_number(1);
  five_g_s_tmsi.amf_pointer.from_number(0);
  five_g_s_tmsi.five_g_tmsi.from_number(4211117727);

  // Fill TAI list for paging.
  asn1::ngap::tai_list_for_paging_item_s paging_item;
  paging_item.tai.plmn_id.from_string("00f110");
  paging_item.tai.tac.from_number(7);
  paging->tai_list_for_paging.push_back(paging_item);

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_valid_paging_message()
{
  ngap_message ngap_msg;

  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_PAGING);
  auto& paging = ngap_msg.pdu.init_msg().value.paging();

  // Fill UE paging id.
  auto& five_g_s_tmsi = paging->ue_paging_id.set_five_g_s_tmsi();
  five_g_s_tmsi.amf_set_id.from_number(1);
  five_g_s_tmsi.amf_pointer.from_number(0);
  five_g_s_tmsi.five_g_tmsi.from_number(4211117727);

  // Fill paging DRX.
  paging->paging_drx_present = true;
  paging->paging_drx.value   = asn1::ngap::paging_drx_opts::options::v64;

  // Fill TAI list for paging.
  asn1::ngap::tai_list_for_paging_item_s paging_item;
  paging_item.tai.plmn_id.from_string("00f110");
  paging_item.tai.tac.from_number(7);
  paging->tai_list_for_paging.push_back(paging_item);

  // Fill paging prio.
  paging->paging_prio_present = true;
  paging->paging_prio.value   = asn1::ngap::paging_prio_opts::options::priolevel5;

  // Fill UE radio capabilities for paging.
  paging->ue_radio_cap_for_paging_present                       = true;
  paging->ue_radio_cap_for_paging.ue_radio_cap_for_paging_of_nr = make_byte_buffer("deadbeef").value();

  // Fill paging origin.
  paging->paging_origin_present = true;
  paging->paging_origin.value   = asn1::ngap::paging_origin_opts::options::non_neg3gpp;

  // Fill assist data for paging.
  paging->assist_data_for_paging_present                                   = true;
  paging->assist_data_for_paging.assist_data_for_recommended_cells_present = true;

  asn1::ngap::recommended_cell_item_s recommended_cell_item;
  auto&                               nr_cgi = recommended_cell_item.ngran_cgi.set_nr_cgi();
  nr_cgi.plmn_id.from_string("00f110");
  nr_cell_identity nci = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
  nr_cgi.nr_cell_id.from_number(nci.value());
  recommended_cell_item.time_stayed_in_cell_present = true;
  recommended_cell_item.time_stayed_in_cell         = 5;

  paging->assist_data_for_paging.assist_data_for_recommended_cells.recommended_cells_for_paging.recommended_cell_list
      .push_back(recommended_cell_item);

  paging->assist_data_for_paging.paging_attempt_info_present                        = true;
  paging->assist_data_for_paging.paging_attempt_info.paging_attempt_count           = 3;
  paging->assist_data_for_paging.paging_attempt_info.intended_nof_paging_attempts   = 4;
  paging->assist_data_for_paging.paging_attempt_info.next_paging_area_scope_present = true;
  paging->assist_data_for_paging.paging_attempt_info.next_paging_area_scope.value =
      asn1::ngap::next_paging_area_scope_opts::options::changed;

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_invalid_paging_message()
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_PAGING);

  // Fill UE paging id.
  auto& paging = ngap_msg.pdu.init_msg().value.paging();
  paging->ue_paging_id.set_choice_exts();
  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_error_indication_message(amf_ue_id_t amf_ue_id, ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg;

  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_ERROR_IND);

  auto& error_indication = ngap_msg.pdu.init_msg().value.error_ind();

  error_indication->amf_ue_ngap_id_present = true;
  error_indication->amf_ue_ngap_id         = amf_ue_id_to_uint(amf_ue_id);

  error_indication->ran_ue_ngap_id_present = true;
  error_indication->ran_ue_ngap_id         = ran_ue_id_to_uint(ran_ue_id);

  error_indication->cause_present = true;
  auto& cause                     = error_indication->cause.set_radio_network();
  cause                           = asn1::ngap::cause_radio_network_opts::options::unknown_pdu_session_id;

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_valid_handover_request(amf_ue_id_t amf_ue_id)
{
  ngap_message ngap_msg;

  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_HO_RES_ALLOC);

  auto& ho_request = ngap_msg.pdu.init_msg().value.ho_request();

  ho_request->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  // Fill Handover type.
  ho_request->handov_type = asn1::ngap::handov_type_opts::options::intra5gs;
  // Fill cause.
  ho_request->cause.set_radio_network() = asn1::ngap::cause_radio_network_opts::options::ho_desirable_for_radio_reason;
  // Fill UE aggregated max bit rate.
  ho_request->ue_aggr_max_bit_rate.ue_aggr_max_bit_rate_dl = 1073741824;
  ho_request->ue_aggr_max_bit_rate.ue_aggr_max_bit_rate_ul = 1073741824;
  // Fill UE security capabilities.
  ho_request->ue_security_cap.nr_encryption_algorithms.from_number(49152);
  ho_request->ue_security_cap.nr_integrity_protection_algorithms.from_number(49152);
  ho_request->ue_security_cap.eutr_aencryption_algorithms.from_number(0);
  ho_request->ue_security_cap.eutr_aintegrity_protection_algorithms.from_number(0);
  // Fill security context
  ho_request->security_context.next_hop_chaining_count = 2;
  ho_request->security_context.next_hop_nh.from_string(
      "1000100100100011110101001110000001001000000001111000010110011110001010011100101010110010000001010110110000100111"
      "0110101111001100000001100111100010001011111000111111101000100110011101110111100010101101000101000010100001000001"
      "00000101010000111100001010001001"); // 8923d4e04807859e29cab2056c276bcc06788be3fa267778ad1428410543c289
  // Fill PDU session resource setup list HO req.
  asn1::ngap::pdu_session_res_setup_item_ho_req_s setup_item;
  setup_item.pdu_session_id = 1;
  setup_item.s_nssai.sst.from_number(1);
  setup_item.ho_request_transfer =
      make_byte_buffer(
          "0000050082000a0c400000003040000000008b000a01f00a32130200001b13007f00010000860001000088000700010000091c00")
          .value();
  ho_request->pdu_session_res_setup_list_ho_req.push_back(setup_item);
  // Fill allowed S-NSSAI.
  asn1::ngap::allowed_nssai_item_s allowed_nssai;
  allowed_nssai.s_nssai.sst.from_number(1);
  ho_request->allowed_nssai.push_back(allowed_nssai);
  // Fill masked IMEISV
  ho_request->masked_imeisv_present = true;
  ho_request->masked_imeisv.from_number(81985526923525889); // 0123456700ffff01
  // Fill source to target transparent container.
  asn1::ngap::source_ngran_node_to_target_ngran_node_transparent_container_s transparent_container;
  // Fill RRC container.
  transparent_container.rrc_container =
      make_byte_buffer("0021930680ce811d1968097e340e1480005824c5c00060fc2c00637fe002e00131401a0000000880058d006007a071e"
                       "439f0000240400e03"
                       "00000000100186c0000700809df0000000000000103a0002000012cb2800281c50f0007000f00000004008010240a0")
          .value();
  // Fill PDU session resource info list.
  asn1::ngap::pdu_session_res_info_item_s session_item;
  session_item.pdu_session_id = 1;
  asn1::ngap::qos_flow_info_item_s flow_item;
  flow_item.qos_flow_id = 0;
  session_item.qos_flow_info_list.push_back(flow_item);
  transparent_container.pdu_session_res_info_list.push_back(session_item);
  // Fill NR CGI.
  auto& cgi = transparent_container.target_cell_id.set_nr_cgi();
  cgi.plmn_id.from_string("00f110");
  nr_cell_identity nci = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
  cgi.nr_cell_id.from_number(nci.value());
  // Fill UE history info.
  asn1::ngap::last_visited_cell_item_s cell_item;
  auto&                                cell       = cell_item.last_visited_cell_info.set_ngran_cell();
  auto&                                ngran_cell = cell.global_cell_id.set_nr_cgi();
  ngran_cell.plmn_id.from_string("00f110");
  ngran_cell.nr_cell_id.from_number(nci.value());
  cell.cell_type.cell_size    = asn1::ngap::cell_size_opts::options::small;
  cell.time_ue_stayed_in_cell = 0;
  transparent_container.ue_history_info.push_back(cell_item);

  byte_buffer   pdu{};
  asn1::bit_ref bref{pdu};
  if (transparent_container.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
    srslog::fetch_basic_logger("NGAP").error("Failed to pack source to target container.");
  }
  ho_request->source_to_target_transparent_container = pdu.copy();

  // Fill GUAMI.
  ho_request->guami.plmn_id.from_string("00f110");
  ho_request->guami.amf_region_id.from_number(2);
  ho_request->guami.amf_set_id.from_number(1);
  ho_request->guami.amf_pointer.from_number(0);

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_handover_preparation_failure(amf_ue_id_t amf_ue_id, ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg;

  ngap_msg.pdu.set_unsuccessful_outcome();
  ngap_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_NGAP_ID_HO_PREP);

  auto& ho_prep_fail = ngap_msg.pdu.unsuccessful_outcome().value.ho_prep_fail();

  ho_prep_fail->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  ho_prep_fail->ran_ue_ngap_id = ran_ue_id_to_uint(ran_ue_id);

  // Fill cause.
  ho_prep_fail->cause.set_radio_network() = asn1::ngap::cause_radio_network_opts::options::unspecified;

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_valid_handover_command(amf_ue_id_t amf_ue_id, ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg;

  ngap_msg.pdu.set_successful_outcome();
  ngap_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_HO_PREP);

  auto& ho_cmd = ngap_msg.pdu.successful_outcome().value.ho_cmd();

  ho_cmd->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  ho_cmd->ran_ue_ngap_id = ran_ue_id_to_uint(ran_ue_id);
  // Fill handover type.
  ho_cmd->handov_type = asn1::ngap::handov_type_opts::options::intra5gs;

  // Fill PDU session resource HO list.
  ho_cmd->pdu_session_res_ho_list_present = true;
  asn1::ngap::pdu_session_res_ho_item_s ho_item;
  ho_item.pdu_session_id  = 1;
  ho_item.ho_cmd_transfer = make_byte_buffer("00").value();
  ho_cmd->pdu_session_res_ho_list.push_back(ho_item);

  // Fill target to source transparent container.
  asn1::ngap::target_ngran_node_to_source_ngran_node_transparent_container_s transparent_container;
  // Fill RRC container.
  transparent_container.rrc_container = make_byte_buffer("08190115200204d00f00102f1f852020605701ac00445ebb1c041878002c0"
                                                         "0445ebb1c041878002c24445ebb1c041878002c700d3133b414"
                                                         "831f0203e0102341e0400024a771002900000000c000140000034ec00187c"
                                                         "8a000000697386589000401833251870024e1106fbf56c70eb0"
                                                         "04162301620981950001ffff8000000306e10840000702ca0041904000040"
                                                         "d31a01100102002a28908900081001514488500040800a8a246"
                                                         "30002040054514060088681aab2420e2048a163068e1e4a78fa0428918f04"
                                                         "000850404800b50405000850505800b50506000850606800b50"
                                                         "6071a48500079a4b5000b9a4b5040f0050703e68410101a10484268414111"
                                                         "a10584668418129a10720496302645c24d03a41078bbf030438"
                                                         "00000071ffa5294a529e502c0000432ec000000000000a0000018ad545004"
                                                         "7001800082002a210054401c040421000a88401560070201104"
                                                         "002a210055801c0c0421000a88401568070401104002a210055a001000000"
                                                         "10430102030403834000a8a2000000200400080600080900220"
                                                         "0a600002298094e3800c00")
                                            .value();
  ho_cmd->target_to_source_transparent_container = pack_into_pdu(transparent_container);

  return ngap_msg;
}

ngap_handover_preparation_request srsran::srs_cu_cp::generate_handover_preparation_request(
    ue_index_t                                                ue_index,
    const std::map<pdu_session_id_t, up_pdu_session_context>& pdu_sessions,
    nr_cell_identity                                          nci,
    uint32_t                                                  gnb_id_bit_length)
{
  ngap_handover_preparation_request request = {};
  request.ue_index                          = ue_index;
  request.gnb_id                            = nci.gnb_id(gnb_id_bit_length);
  request.nci                               = nci;
  // Fill create a map of all PDU sessions and their associated QoS flows.
  for (const auto& pdu_session : pdu_sessions) {
    std::vector<qos_flow_id_t> qos_flows;
    for (const auto& drb : pdu_session.second.drbs) {
      for (const auto& qos_flow : drb.second.qos_flows) {
        qos_flows.push_back(qos_flow.first);
      }
    }
    request.pdu_sessions.insert({pdu_session.first, qos_flows});
  }

  return request;
}

ngap_message srsran::srs_cu_cp::generate_handover_cancel_ack(amf_ue_id_t amf_ue_id, ran_ue_id_t ran_ue_id)
{
  ngap_message ngap_msg;

  ngap_msg.pdu.set_successful_outcome();
  ngap_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_HO_CANCEL);

  auto& ho_cancel_ack = ngap_msg.pdu.successful_outcome().value.ho_cancel_ack();

  ho_cancel_ack->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  ho_cancel_ack->ran_ue_ngap_id = ran_ue_id_to_uint(ran_ue_id);

  return ngap_msg;
}

ngap_message srsran::srs_cu_cp::generate_ng_reset_ack(const asn1::ngap::ue_associated_lc_ng_conn_list_l& ng_reset_ues)
{
  ngap_message ngap_msg;

  ngap_msg.pdu.set_successful_outcome();
  ngap_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_NG_RESET);
  if (ng_reset_ues.size() > 0) {
    auto& ng_reset_ack                                  = ngap_msg.pdu.successful_outcome().value.ng_reset_ack();
    ng_reset_ack->ue_associated_lc_ng_conn_list_present = true;
    ng_reset_ack->ue_associated_lc_ng_conn_list         = ng_reset_ues;
  }

  return ngap_msg;
}
