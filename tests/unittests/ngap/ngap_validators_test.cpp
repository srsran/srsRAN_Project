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

#include "lib/ngap/ngap_asn1_helpers.h"
#include "lib/ngap/ngap_validators/ngap_validators.h"
#include "ngap_test_helpers.h"
#include "srsran/cu_cp/cu_cp_configuration_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ngap/ngap_types.h"
#include "srsran/ran/cu_types.h"
#include <gtest/gtest.h>
#include <sys/types.h>

using namespace srsran;
using namespace srs_cu_cp;

class ngap_validator_test : public ngap_test
{
public:
  asn1::ngap::pdu_session_res_setup_item_su_req_s generate_pdu_session_resource_setup_item(pdu_session_id_t psi)
  {
    asn1::ngap::pdu_session_res_setup_item_su_req_s pdu_session_res_item;

    pdu_session_res_item.pdu_session_id = pdu_session_id_to_uint(psi);

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
        "0000040082000a0c400000003040000000008b000a01f00a321302000028d600860001000088000700010000091c00");

    return pdu_session_res_item;
  }

  ngap_message
  generate_pdu_session_resource_setup_request_with_one_unique_and_two_duplicate_pdu_sessions(amf_ue_id_t amf_ue_id,
                                                                                             ran_ue_id_t ran_ue_id)
  {
    ngap_message ngap_msg = generate_invalid_pdu_session_resource_setup_request_message(amf_ue_id, ran_ue_id);

    auto& pdu_session_res_setup_req = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

    // Add unique PDU session
    pdu_session_id_t unique_psi = uint_to_pdu_session_id(2);
    pdu_session_res_setup_req->pdu_session_res_setup_list_su_req.push_back(
        generate_pdu_session_resource_setup_item(unique_psi));

    return ngap_msg;
  }

  ngap_message
  generate_pdu_session_resource_setup_request_with_one_already_setup_and_one_new_pdu_session(amf_ue_id_t amf_ue_id,
                                                                                             ran_ue_id_t ran_ue_id)
  {
    pdu_session_id_t setup_psi = uint_to_pdu_session_id(1);
    ngap_message ngap_msg = generate_valid_pdu_session_resource_setup_request_message(amf_ue_id, ran_ue_id, setup_psi);

    auto& pdu_session_res_setup_req = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

    // Add unique PDU session
    pdu_session_id_t new_psi = uint_to_pdu_session_id(2);
    pdu_session_res_setup_req->pdu_session_res_setup_list_su_req.push_back(
        generate_pdu_session_resource_setup_item(new_psi));

    return ngap_msg;
  }

  ngap_message generate_pdu_session_resource_setup_request_with_non_gbr_qos_flows_and_no_aggregate_maximum_bitrate(
      amf_ue_id_t      amf_ue_id,
      ran_ue_id_t      ran_ue_id,
      pdu_session_id_t psi)
  {
    ngap_message ngap_msg = generate_valid_pdu_session_resource_setup_request_message(amf_ue_id, ran_ue_id, psi);

    auto& asn1_request                         = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();
    asn1_request->ue_aggr_max_bit_rate_present = false;

    // Add PDU Session Resource Setup Request Transfer
    asn1::ngap::pdu_session_res_setup_request_transfer_s asn1_setup_req_transfer;

    asn1_setup_req_transfer->ul_ngu_up_tnl_info.set_gtp_tunnel();
    auto addr = transport_layer_address::create_from_string("127.0.0.1");
    asn1_setup_req_transfer->ul_ngu_up_tnl_info.gtp_tunnel().transport_layer_address.from_string(addr.to_bitstring());
    asn1_setup_req_transfer->ul_ngu_up_tnl_info.gtp_tunnel().gtp_teid.from_number(1);

    asn1_setup_req_transfer->pdu_session_type = asn1::ngap::pdu_session_type_opts::options::ipv4v6;

    asn1::ngap::qos_flow_setup_request_item_s asn1_qos_flow_item;
    asn1_qos_flow_item.qos_flow_id = 1;

    asn1_qos_flow_item.qos_flow_level_qos_params.qos_characteristics.set_dyn5qi();
    asn1_qos_flow_item.qos_flow_level_qos_params.qos_characteristics.dyn5qi().prio_level_qos                 = 1;
    asn1_qos_flow_item.qos_flow_level_qos_params.qos_characteristics.dyn5qi().packet_delay_budget            = 1;
    asn1_qos_flow_item.qos_flow_level_qos_params.qos_characteristics.dyn5qi().packet_error_rate.per_exponent = 1;
    asn1_qos_flow_item.qos_flow_level_qos_params.qos_characteristics.dyn5qi().packet_error_rate.per_scalar   = 1;

    asn1_qos_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.prio_level_arp = 1;
    asn1_qos_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_cap =
        asn1::ngap::pre_emption_cap_opts::options::shall_not_trigger_pre_emption;
    asn1_qos_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_vulnerability =
        asn1::ngap::pre_emption_vulnerability_opts::options::not_pre_emptable;

    asn1_qos_flow_item.qos_flow_level_qos_params.add_qos_flow_info_present = true;
    asn1_qos_flow_item.qos_flow_level_qos_params.add_qos_flow_info =
        asn1::ngap::add_qos_flow_info_opts::options::more_likely;
    asn1_qos_flow_item.qos_flow_level_qos_params.reflective_qos_attribute_present = true;
    asn1_qos_flow_item.qos_flow_level_qos_params.reflective_qos_attribute =
        asn1::ngap::reflective_qos_attribute_opts::options::subject_to;

    asn1_setup_req_transfer->qos_flow_setup_request_list.push_back(asn1_qos_flow_item);

    // Pack pdu_session_res_release_resp_transfer_s
    asn1_request->pdu_session_res_setup_list_su_req.begin()->pdu_session_res_setup_request_transfer =
        pack_into_pdu(asn1_setup_req_transfer);

    return ngap_msg;
  }

  asn1::ngap::pdu_session_res_modify_item_mod_req_s generate_pdu_session_resource_modify_item(pdu_session_id_t psi)
  {
    asn1::ngap::pdu_session_res_modify_item_mod_req_s pdu_session_res_item;

    pdu_session_res_item.pdu_session_id = pdu_session_id_to_uint(psi);

    // Add PDU Session Resource Modify Request Transfer
    asn1::ngap::pdu_session_res_modify_request_transfer_s pdu_session_res_modify_request_transfer;

    // Add qos flow add or modify request item
    pdu_session_res_modify_request_transfer->qos_flow_add_or_modify_request_list_present = true;
    asn1::ngap::qos_flow_add_or_modify_request_item_s qos_flow_add_item;

    // qosFlowIdentifier
    qos_flow_add_item.qos_flow_id = 1;

    pdu_session_res_modify_request_transfer->qos_flow_add_or_modify_request_list.push_back(qos_flow_add_item);

    pdu_session_res_item.pdu_session_res_modify_request_transfer =
        pack_into_pdu(pdu_session_res_modify_request_transfer);

    return pdu_session_res_item;
  }

  ngap_message generate_pdu_session_resource_modify_request_with_one_unique_and_two_duplicate_pdu_sessions(
      amf_ue_id_t      amf_ue_id,
      ran_ue_id_t      ran_ue_id,
      pdu_session_id_t unique_psi,
      pdu_session_id_t duplicate_psi)
  {
    ngap_message ngap_msg =
        generate_invalid_pdu_session_resource_modify_request_message(amf_ue_id, ran_ue_id, duplicate_psi);

    auto& pdu_session_res_modify_req = ngap_msg.pdu.init_msg().value.pdu_session_res_modify_request();

    // Add unique PDU session
    pdu_session_res_modify_req->pdu_session_res_modify_list_mod_req.push_back(
        generate_pdu_session_resource_modify_item(unique_psi));

    return ngap_msg;
  }
};

// Test handling of valid PDU Session Resource Modification Request
TEST_F(ngap_validator_test, when_valid_request_received_then_pdu_session_setup_succeeds)
{
  ue_index_t  ue_index  = uint_to_ue_index(0);
  amf_ue_id_t amf_ue_id = uint_to_amf_ue_id(0);
  ran_ue_id_t ran_ue_id = uint_to_ran_ue_id(0);

  ngap_message ngap_msg =
      generate_valid_pdu_session_resource_setup_request_message(amf_ue_id, ran_ue_id, uint_to_pdu_session_id(1));

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

  cu_cp_pdu_session_resource_setup_request request;
  fill_cu_cp_pdu_session_resource_setup_request(request, asn1_request->pdu_session_res_setup_list_su_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU Session Resource Setup Request
  auto verification_outcome = verify_pdu_session_resource_setup_request(request, asn1_request, ue_logger);

  ASSERT_EQ(verification_outcome.request.pdu_session_res_setup_items.size(), 1U);
  ASSERT_EQ(verification_outcome.response.pdu_session_res_setup_response_items.size(), 0U);
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_setup_items.size(), 0U);
}

// Test handling of duplicate PDU Session IDs in PDU Session Resource Setup Request
TEST_F(ngap_validator_test, when_duplicate_pdu_session_id_then_pdu_session_setup_fails)
{
  ue_index_t  ue_index  = uint_to_ue_index(0);
  amf_ue_id_t amf_ue_id = uint_to_amf_ue_id(0);
  ran_ue_id_t ran_ue_id = uint_to_ran_ue_id(0);

  ngap_message ngap_msg = generate_invalid_pdu_session_resource_setup_request_message(amf_ue_id, ran_ue_id);

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

  cu_cp_pdu_session_resource_setup_request request;
  fill_cu_cp_pdu_session_resource_setup_request(request, asn1_request->pdu_session_res_setup_list_su_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU Session Resource Setup Request
  auto verification_outcome = verify_pdu_session_resource_setup_request(request, asn1_request, ue_logger);

  ASSERT_TRUE(verification_outcome.request.pdu_session_res_setup_items.empty());
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_setup_items.size(), 1U);
}

TEST_F(ngap_validator_test, when_unique_and_duplicate_pdu_session_id_then_pdu_session_setup_partly_fails)
{
  ue_index_t  ue_index  = uint_to_ue_index(0);
  amf_ue_id_t amf_ue_id = uint_to_amf_ue_id(0);
  ran_ue_id_t ran_ue_id = uint_to_ran_ue_id(0);

  ngap_message ngap_msg =
      generate_pdu_session_resource_setup_request_with_one_unique_and_two_duplicate_pdu_sessions(amf_ue_id, ran_ue_id);

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

  cu_cp_pdu_session_resource_setup_request request;
  fill_cu_cp_pdu_session_resource_setup_request(request, asn1_request->pdu_session_res_setup_list_su_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU Session Resource Setup Request
  auto verification_outcome = verify_pdu_session_resource_setup_request(request, asn1_request, ue_logger);

  ASSERT_EQ(verification_outcome.request.pdu_session_res_setup_items.size(), 1U);
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_setup_items.size(), 1U);
}

// Test handling of PduSessionResourceSetupRequest with non-GBR QoS flows but no PDU Session Aggregate Maximum Bit Rate
TEST_F(
    ngap_validator_test,
    when_pdu_session_request_contains_non_gbr_qos_flows_but_no_aggregate_maximum_bitrate_then_pdu_session_setup_fails)
{
  pdu_session_id_t psi       = uint_to_pdu_session_id(1);
  ue_index_t       ue_index  = uint_to_ue_index(0);
  amf_ue_id_t      amf_ue_id = uint_to_amf_ue_id(0);
  ran_ue_id_t      ran_ue_id = uint_to_ran_ue_id(0);

  ngap_message ngap_msg =
      generate_pdu_session_resource_setup_request_with_non_gbr_qos_flows_and_no_aggregate_maximum_bitrate(
          amf_ue_id, ran_ue_id, psi);

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

  cu_cp_pdu_session_resource_setup_request request;
  fill_cu_cp_pdu_session_resource_setup_request(request, asn1_request->pdu_session_res_setup_list_su_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU Session Resource Setup Request
  auto verification_outcome = verify_pdu_session_resource_setup_request(request, asn1_request, ue_logger);

  ASSERT_TRUE(verification_outcome.request.pdu_session_res_setup_items.empty());
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_setup_items.size(), 1U);
}

// Test handling of valid PDU Session Resource Modification Request
TEST_F(ngap_validator_test, when_valid_request_received_then_pdu_session_modify_succeeds)
{
  pdu_session_id_t psi       = uint_to_pdu_session_id(1);
  ue_index_t       ue_index  = uint_to_ue_index(0);
  amf_ue_id_t      amf_ue_id = uint_to_amf_ue_id(0);
  ran_ue_id_t      ran_ue_id = uint_to_ran_ue_id(0);

  ngap_message ngap_msg = generate_valid_pdu_session_resource_modify_request_message(amf_ue_id, ran_ue_id, psi);

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_modify_request();

  cu_cp_pdu_session_resource_modify_request request;
  fill_cu_cp_pdu_session_resource_modify_request(request, asn1_request->pdu_session_res_modify_list_mod_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU Session Resource Modify Request
  auto verification_outcome = verify_pdu_session_resource_modify_request(request, asn1_request, ue_logger);

  ASSERT_EQ(verification_outcome.request.pdu_session_res_modify_items.size(), 1U);
  ASSERT_EQ(verification_outcome.response.pdu_session_res_modify_list.size(), 0U);
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_modify_list.size(), 0U);
}

// Test handling of duplicate PDU Session IDs in PDU Session Resource Modification Request
TEST_F(ngap_validator_test, when_duplicate_pdu_session_id_then_pdu_session_modification_fails)
{
  ue_index_t  ue_index  = uint_to_ue_index(0);
  amf_ue_id_t amf_ue_id = uint_to_amf_ue_id(0);
  ran_ue_id_t ran_ue_id = uint_to_ran_ue_id(0);

  ngap_message ngap_msg =
      generate_invalid_pdu_session_resource_modify_request_message(amf_ue_id, ran_ue_id, uint_to_pdu_session_id(1));

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_modify_request();

  cu_cp_pdu_session_resource_modify_request request;
  fill_cu_cp_pdu_session_resource_modify_request(request, asn1_request->pdu_session_res_modify_list_mod_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU Session Resource Modify Request
  auto verification_outcome = verify_pdu_session_resource_modify_request(request, asn1_request, ue_logger);

  ASSERT_TRUE(verification_outcome.request.pdu_session_res_modify_items.empty());
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_modify_list.size(), 1U);
}

TEST_F(ngap_validator_test, when_unique_and_duplicate_pdu_session_id_then_pdu_session_modify_partly_fails)
{
  pdu_session_id_t psi           = uint_to_pdu_session_id(1);
  pdu_session_id_t duplicate_psi = uint_to_pdu_session_id(2);
  ue_index_t       ue_index      = uint_to_ue_index(0);
  amf_ue_id_t      amf_ue_id     = uint_to_amf_ue_id(0);
  ran_ue_id_t      ran_ue_id     = uint_to_ran_ue_id(0);

  ngap_message ngap_msg = generate_pdu_session_resource_modify_request_with_one_unique_and_two_duplicate_pdu_sessions(
      amf_ue_id, ran_ue_id, psi, duplicate_psi);

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_modify_request();

  cu_cp_pdu_session_resource_modify_request request;
  fill_cu_cp_pdu_session_resource_modify_request(request, asn1_request->pdu_session_res_modify_list_mod_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU Session Resource Modify Request
  auto verification_outcome = verify_pdu_session_resource_modify_request(request, asn1_request, ue_logger);

  ASSERT_EQ(verification_outcome.request.pdu_session_res_modify_items.size(), 1U);
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_modify_list.size(), 1U);
}
