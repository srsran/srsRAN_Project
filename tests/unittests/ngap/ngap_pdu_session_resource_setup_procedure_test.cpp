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

#include "ngap_test_helpers.h"
#include "srsran/asn1/ngap/ngap_ies.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/ran/cu_types.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class ngap_pdu_session_resource_setup_procedure_test : public ngap_test
{
protected:
  ue_index_t start_procedure(bool enable_security = true)
  {
    if (!run_ng_setup()) {
      return ue_index_t::invalid;
    }

    ue_index_t ue_index = create_ue();

    // Inject DL NAS transport message from AMF.
    run_dl_nas_transport(ue_index);

    // Inject UL NAS transport message from RRC.
    run_ul_nas_transport(ue_index);

    // Inject Initial Context Setup Request.
    run_initial_context_setup(ue_index);

    if (enable_security) {
      // Mark security as enabled.
      ue_mng.find_ue(ue_index)->get_security_manager().enable_security();
    }

    return ue_index;
  }

  bool
  was_pdu_session_type_conversion_successful(const asn1::ngap::pdu_session_res_setup_item_su_req_s& asn1_request_item,
                                             const pdu_session_type_t&                              type) const
  {
    // Unpack request transfer.
    asn1::ngap::pdu_session_res_setup_request_transfer_s asn1_setup_req_transfer;
    asn1::cbit_ref bref({asn1_request_item.pdu_session_res_setup_request_transfer.begin(),
                         asn1_request_item.pdu_session_res_setup_request_transfer.end()});

    if (asn1_setup_req_transfer.unpack(bref) != asn1::SRSASN_SUCCESS) {
      test_logger.error("Couldn't unpack PDU Session Resource Setup Request Transfer PDU");
      return false;
    }

    // Check that the conversion from ASN.1 PDU Session Type to PDU Session Type is correct.
    if (asn1_setup_req_transfer->pdu_session_type == asn1::ngap::pdu_session_type_e::ipv4) {
      return type == pdu_session_type_t::ipv4;
    }
    if (asn1_setup_req_transfer->pdu_session_type == asn1::ngap::pdu_session_type_e::ipv6) {
      return type == pdu_session_type_t::ipv6;
    }
    if (asn1_setup_req_transfer->pdu_session_type == asn1::ngap::pdu_session_type_e::ipv4v6) {
      return type == pdu_session_type_t::ipv4v6;
    }
    if (asn1_setup_req_transfer->pdu_session_type == asn1::ngap::pdu_session_type_e::ethernet) {
      return type == pdu_session_type_t::ethernet;
    }
    return false; // Unsupported type.
  }

  bool was_conversion_successful(ngap_message       pdu_session_resource_setup_request,
                                 pdu_session_id_t   pdu_session_id,
                                 pdu_session_type_t pdu_session_type) const
  {
    bool test_1 = pdu_session_resource_setup_request.pdu.init_msg()
                      .value.pdu_session_res_setup_request()
                      ->pdu_session_res_setup_list_su_req.size() ==
                  cu_cp_notifier.last_request.pdu_session_res_setup_items.size();

    // Find the ASN.1 PDU session resource setup item for the given PDU session ID.
    asn1::ngap::pdu_session_res_setup_item_su_req_s asn1_request_item = {};
    for (const auto& item : pdu_session_resource_setup_request.pdu.init_msg()
                                .value.pdu_session_res_setup_request()
                                ->pdu_session_res_setup_list_su_req) {
      if (uint_to_pdu_session_id(item.pdu_session_id) == pdu_session_id) {
        asn1_request_item = item;
        break;
      }
    }

    bool test_2 = was_pdu_session_type_conversion_successful(asn1_request_item, pdu_session_type);

    return test_1 && test_2;
  }

  bool was_pdu_session_resource_setup_request_valid() const
  {
    // Check that AMF notifier was called with right type.
    bool test_1 = n2_gw.last_ngap_msgs.back().pdu.successful_outcome().value.type() ==
                  asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::pdu_session_res_setup_resp;

    // Check that response contains PDU Session Resource Setup List.
    bool test_2 = n2_gw.last_ngap_msgs.back()
                      .pdu.successful_outcome()
                      .value.pdu_session_res_setup_resp()
                      ->pdu_session_res_setup_list_su_res_present;

    return test_1 && test_2;
  }

  bool was_pdu_session_resource_setup_request_invalid() const
  {
    // Check that AMF notifier was called with right type.
    bool test_1 = n2_gw.last_ngap_msgs.back().pdu.successful_outcome().value.type() ==
                  asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::pdu_session_res_setup_resp;

    // Check that response doesn't contain PDU Session Resource Setup List.
    bool test_2 = !n2_gw.last_ngap_msgs.back()
                       .pdu.successful_outcome()
                       .value.pdu_session_res_setup_resp()
                       ->pdu_session_res_setup_list_su_res_present;

    // Check that response contains PDU Session Resource Failed to Setup List.
    bool test_3 = n2_gw.last_ngap_msgs.back()
                      .pdu.successful_outcome()
                      .value.pdu_session_res_setup_resp()
                      ->pdu_session_res_failed_to_setup_list_su_res_present;

    return test_1 && test_2 && test_3;
  }

  bool was_error_indication_sent() const
  {
    return n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::error_ind;
  }

  bool check_metrics_report(const std::string&                                amf_name,
                            const std::map<s_nssai_t, pdu_session_metrics_t>& pdu_session_metrics)
  {
    const ngap_info& metrics_report = ngap->get_metrics_handler().handle_ngap_metrics_report_request();

    // Check that metrics contain the successful PDU session setup.
    if (metrics_report.amf_name != amf_name) {
      test_logger.error("AMF name mismatch: expected {}, got {}", amf_name, metrics_report.amf_name);
      return false;
    }
    for (const auto& [s_nssai, metrics] : pdu_session_metrics) {
      const ngap_metrics& test_metrics = metrics_report.metrics;

      if (test_metrics.pdu_session_metrics.find(s_nssai) == test_metrics.pdu_session_metrics.end()) {
        test_logger.error("PDU session metrics for s_nssai {} not found", s_nssai);
        return false;
      }

      if (test_metrics.pdu_session_metrics.at(s_nssai).nof_pdu_sessions_requested_to_setup !=
          metrics.nof_pdu_sessions_requested_to_setup) {
        test_logger.error("PDU sessions requested for s_nssai {} mismatch: expected {}, got {}",
                          s_nssai,
                          metrics.nof_pdu_sessions_requested_to_setup,
                          test_metrics.pdu_session_metrics.at(s_nssai).nof_pdu_sessions_requested_to_setup);
        return false;
      }
      if (test_metrics.pdu_session_metrics.at(s_nssai).nof_pdu_sessions_successfully_setup !=
          metrics.nof_pdu_sessions_successfully_setup) {
        test_logger.error("PDU sessions successfully setup for s_nssai {} mismatch: expected {}, got {}",
                          s_nssai,
                          metrics.nof_pdu_sessions_successfully_setup,
                          test_metrics.pdu_session_metrics.at(s_nssai).nof_pdu_sessions_successfully_setup);
        return false;
      }

      for (unsigned idx = 0; idx < metrics.nof_pdu_sessions_failed_to_setup.size(); ++idx) {
        unsigned expected_count = metrics.nof_pdu_sessions_failed_to_setup.get_count(idx);
        unsigned test_count =
            test_metrics.pdu_session_metrics.at(s_nssai).nof_pdu_sessions_failed_to_setup.get_count(idx);

        if (expected_count != test_count) {
          test_logger.error(
              "PDU sessions failed to setup for s_nssai {} cause {} mismatch: expected {}, got {}",
              s_nssai,
              test_metrics.pdu_session_metrics.at(s_nssai).nof_pdu_sessions_failed_to_setup.get_cause(idx),
              expected_count,
              test_count);

          return false;
        }
      }
    }
    return true;
  }
};

/// Test missing PDU Session Resource Setup Request.
TEST_F(ngap_pdu_session_resource_setup_procedure_test,
       when_pdu_session_resource_setup_request_is_not_received_then_ue_release_is_requested)
{
  ASSERT_EQ(ngap->get_nof_ues(), 0);

  // Test preamble.
  this->start_procedure();

  // Check that initial context setup request was received to the AMF and that UE object has been created.
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.successful_outcome().value.type(),
            asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::init_context_setup_resp);
  ASSERT_EQ(ngap->get_nof_ues(), 1);

  // Tick timers.
  // Status: NGAP does not receive new PDU Session Resource Setup Request until request_pdu_session_timer has ended.
  for (unsigned msec_elapsed = 0; msec_elapsed < cu_cp_cfg.ue.request_pdu_session_timeout.count() * 1000;
       ++msec_elapsed) {
    this->tick();
  }

  // Check that UE release was requested.
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type(),
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_request);
}

/// Test valid PDU Session Resource Setup Request.
TEST_F(ngap_pdu_session_resource_setup_procedure_test,
       when_valid_pdu_session_resource_setup_request_received_then_pdu_session_setup_succeeds)
{
  // Test preamble.
  ue_index_t ue_index = this->start_procedure();

  // Inject PDU Session Resource Setup Request.
  pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(test_rgen::uniform_int<uint16_t>(
      pdu_session_id_to_uint(pdu_session_id_t::min), pdu_session_id_to_uint(pdu_session_id_t::max)));

  auto& ue = test_ues.at(ue_index);

  ngap_message pdu_session_resource_setup_request = generate_valid_pdu_session_resource_setup_request_message(
      ue.amf_ue_id.value(),
      ue.ran_ue_id.value(),
      {{pdu_session_id, {pdu_session_type_t::ipv4, {{uint_to_qos_flow_id(1), 9}}}}});
  ngap->handle_message(pdu_session_resource_setup_request);

  // Check conversion in adapter.
  ASSERT_TRUE(was_conversion_successful(
      pdu_session_resource_setup_request,
      pdu_session_id,
      cu_cp_notifier.last_request.pdu_session_res_setup_items[pdu_session_id].pdu_session_type));

  // Check that PDU Session Resource Setup Request was valid.
  ASSERT_TRUE(was_pdu_session_resource_setup_request_valid());

  // Check that metrics contain the successful PDU session setup.
  ASSERT_TRUE(check_metrics_report(
      "open5gs-amf0",
      {{{slice_service_type{1}, slice_differentiator::create(0x0027db).value()}, pdu_session_metrics_t{1, 1, {}}}}));
}

/// Test valid PDU Session Resource Setup Request.
TEST_F(ngap_pdu_session_resource_setup_procedure_test,
       when_valid_pdu_session_resource_setup_request_with_ipv4_session_type_received_then_pdu_session_setup_succeeds)
{
  // Test preamble.
  ue_index_t ue_index = this->start_procedure();

  // Inject PDU Session Resource Setup Request.
  pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(test_rgen::uniform_int<uint16_t>(
      pdu_session_id_to_uint(pdu_session_id_t::min), pdu_session_id_to_uint(pdu_session_id_t::max)));

  auto& ue = test_ues.at(ue_index);

  ngap_message pdu_session_resource_setup_request = generate_valid_pdu_session_resource_setup_request_message(
      ue.amf_ue_id.value(),
      ue.ran_ue_id.value(),
      {{pdu_session_id, {pdu_session_type_t::ipv4, {{uint_to_qos_flow_id(1), 9}}}}});
  ngap->handle_message(pdu_session_resource_setup_request);

  // Check conversion in adapter.
  ASSERT_TRUE(was_conversion_successful(
      pdu_session_resource_setup_request,
      pdu_session_id,
      cu_cp_notifier.last_request.pdu_session_res_setup_items[pdu_session_id].pdu_session_type));

  // Check that PDU Session Resource Setup Request was valid.
  ASSERT_TRUE(was_pdu_session_resource_setup_request_valid());

  // Check that metrics contain the successful PDU session setup.
  ASSERT_TRUE(check_metrics_report(
      "open5gs-amf0",
      {{{slice_service_type{1}, slice_differentiator::create(0x0027db).value()}, pdu_session_metrics_t{1, 1, {}}}}));
}

/// Test valid PDU Session Resource Setup Request.
TEST_F(ngap_pdu_session_resource_setup_procedure_test,
       when_valid_pdu_session_resource_setup_request_with_ipv6_session_type_received_then_pdu_session_setup_succeeds)
{
  // Test preamble.
  ue_index_t ue_index = this->start_procedure();

  // Inject PDU Session Resource Setup Request.
  pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(test_rgen::uniform_int<uint16_t>(
      pdu_session_id_to_uint(pdu_session_id_t::min), pdu_session_id_to_uint(pdu_session_id_t::max)));

  auto& ue = test_ues.at(ue_index);

  ngap_message pdu_session_resource_setup_request = generate_valid_pdu_session_resource_setup_request_message(
      ue.amf_ue_id.value(),
      ue.ran_ue_id.value(),
      {{pdu_session_id, {pdu_session_type_t::ipv6, {{uint_to_qos_flow_id(1), 9}}}}});
  ngap->handle_message(pdu_session_resource_setup_request);

  // Check conversion in adapter.
  ASSERT_TRUE(was_conversion_successful(
      pdu_session_resource_setup_request,
      pdu_session_id,
      cu_cp_notifier.last_request.pdu_session_res_setup_items[pdu_session_id].pdu_session_type));

  // Check that PDU Session Resource Setup Request was valid.
  ASSERT_TRUE(was_pdu_session_resource_setup_request_valid());

  // Check that metrics contain the successful PDU session setup.
  ASSERT_TRUE(check_metrics_report(
      "open5gs-amf0",
      {{{slice_service_type{1}, slice_differentiator::create(0x0027db).value()}, pdu_session_metrics_t{1, 1, {}}}}));
}

/// Test valid PDU Session Resource Setup Request.
TEST_F(
    ngap_pdu_session_resource_setup_procedure_test,
    when_valid_pdu_session_resource_setup_request_with_ethernet_session_type_received_then_pdu_session_setup_succeeds)
{
  // Test preamble.
  ue_index_t ue_index = this->start_procedure();

  // Inject PDU Session Resource Setup Request.
  pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(test_rgen::uniform_int<uint16_t>(
      pdu_session_id_to_uint(pdu_session_id_t::min), pdu_session_id_to_uint(pdu_session_id_t::max)));

  auto& ue = test_ues.at(ue_index);

  ngap_message pdu_session_resource_setup_request = generate_valid_pdu_session_resource_setup_request_message(
      ue.amf_ue_id.value(),
      ue.ran_ue_id.value(),
      {{pdu_session_id, {pdu_session_type_t::ethernet, {{uint_to_qos_flow_id(1), 9}}}}});
  ngap->handle_message(pdu_session_resource_setup_request);

  // Check conversion in adapter.
  ASSERT_TRUE(was_conversion_successful(
      pdu_session_resource_setup_request,
      pdu_session_id,
      cu_cp_notifier.last_request.pdu_session_res_setup_items[pdu_session_id].pdu_session_type));

  // Check that PDU Session Resource Setup Request was valid.
  ASSERT_TRUE(was_pdu_session_resource_setup_request_valid());

  // Check that metrics contain the successful PDU session setup.
  ASSERT_TRUE(check_metrics_report(
      "open5gs-amf0",
      {{{slice_service_type{1}, slice_differentiator::create(0x0027db).value()}, pdu_session_metrics_t{1, 1, {}}}}));
}

/// Test valid PDU Session Resource Setup Request.
TEST_F(ngap_pdu_session_resource_setup_procedure_test,
       when_valid_pdu_session_resource_setup_request_with_ipv4v6_session_type_received_then_pdu_session_setup_fails)
{
  // Test preamble.
  ue_index_t ue_index = this->start_procedure();

  // Inject PDU Session Resource Setup Request.
  pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(test_rgen::uniform_int<uint16_t>(
      pdu_session_id_to_uint(pdu_session_id_t::min), pdu_session_id_to_uint(pdu_session_id_t::max)));

  auto& ue = test_ues.at(ue_index);

  ngap_message pdu_session_resource_setup_request = generate_valid_pdu_session_resource_setup_request_message(
      ue.amf_ue_id.value(),
      ue.ran_ue_id.value(),
      {{pdu_session_id, {pdu_session_type_t::ipv4v6, {{uint_to_qos_flow_id(1), 9}}}}});
  ngap->handle_message(pdu_session_resource_setup_request);

  // Check that PDU Session Resource Setup Request was invalid.
  ASSERT_TRUE(was_pdu_session_resource_setup_request_invalid());

  // Check that metrics contain the failed PDU session setup.
  pdu_session_metrics_t expected_pdu_session_metrics = {1, 0, {}};
  expected_pdu_session_metrics.nof_pdu_sessions_failed_to_setup.increase(cause_protocol_t::unspecified);

  ASSERT_TRUE(check_metrics_report(
      "open5gs-amf0",
      {{{slice_service_type{1}, slice_differentiator::create(0x0027db).value()}, expected_pdu_session_metrics}}));
}

/// Test invalid PDU Session Resource Setup Request.
TEST_F(ngap_pdu_session_resource_setup_procedure_test,
       when_invalid_pdu_session_resource_setup_request_received_then_pdu_session_setup_failed)
{
  // Test preamble.
  ue_index_t ue_index = this->start_procedure();

  auto& ue = test_ues.at(ue_index);

  // Inject invalid PDU Session Resource Setup Request.
  ngap_message pdu_session_resource_setup_request =
      generate_invalid_pdu_session_resource_setup_request_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(pdu_session_resource_setup_request);

  // Check that PDU Session Resource Setup Request was invalid.
  ASSERT_TRUE(was_pdu_session_resource_setup_request_invalid());

  // Check that metrics contain the failed PDU session setup.
  pdu_session_metrics_t expected_pdu_session_metrics = {2, 0, {}};
  expected_pdu_session_metrics.nof_pdu_sessions_failed_to_setup.increase(
      ngap_cause_radio_network_t::multiple_pdu_session_id_instances);

  ASSERT_TRUE(check_metrics_report(
      "open5gs-amf0",
      {{{slice_service_type{1}, slice_differentiator::create(0x0027db).value()}, expected_pdu_session_metrics}}));
}

/// Test invalid PDU Session Resource Setup Request.
TEST_F(ngap_pdu_session_resource_setup_procedure_test,
       when_pdu_session_resource_setup_request_with_invalid_tpl_received_then_pdu_session_setup_failed)
{
  // Test preamble.
  ue_index_t ue_index = this->start_procedure();

  auto& ue = test_ues.at(ue_index);

  // Inject invalid PDU Session Resource Setup Request.
  ngap_message pdu_session_resource_setup_request =
      generate_pdu_session_resource_setup_request_with_pdu_session_type_ipv4_and_ipv4v6_transport_layer_address(
          ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(pdu_session_resource_setup_request);

  // Check that Error Indication has been sent to AMF.
  ASSERT_TRUE(was_error_indication_sent());

  // Check that metrics contain the requested PDU session setup.
  ASSERT_TRUE(check_metrics_report(
      "open5gs-amf0",
      {{{slice_service_type{1}, slice_differentiator::create(0x0027db).value()}, pdu_session_metrics_t{1, 0, {}}}}));
}

/// Test invalid PDU Session Resource Setup Request.
TEST_F(ngap_pdu_session_resource_setup_procedure_test, when_security_not_enabled_then_pdu_session_setup_failed)
{
  // Test preamble.
  ue_index_t ue_index = this->start_procedure(false);
  auto&      ue       = test_ues.at(ue_index);

  // Inject PDU Session Resource Setup Request.
  pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(test_rgen::uniform_int<uint16_t>(
      pdu_session_id_to_uint(pdu_session_id_t::min), pdu_session_id_to_uint(pdu_session_id_t::max)));

  ngap_message pdu_session_resource_setup_request = generate_valid_pdu_session_resource_setup_request_message(
      ue.amf_ue_id.value(),
      ue.ran_ue_id.value(),
      {{pdu_session_id, {pdu_session_type_t::ipv4, {{uint_to_qos_flow_id(1), 9}}}}});
  ngap->handle_message(pdu_session_resource_setup_request);

  // Check that Error Indication has been sent to AMF.
  ASSERT_TRUE(was_error_indication_sent());

  // Check that metrics contain the requested PDU session setup.
  ASSERT_TRUE(check_metrics_report(
      "open5gs-amf0",
      {{{slice_service_type{1}, slice_differentiator::create(0x0027db).value()}, pdu_session_metrics_t{1, 0, {}}}}));
}
