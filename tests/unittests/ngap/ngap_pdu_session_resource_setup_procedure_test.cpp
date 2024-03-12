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

#include "ngap_test_helpers.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class ngap_pdu_session_resource_setup_procedure_test : public ngap_test
{
protected:
  ue_index_t start_procedure()
  {
    ue_index_t ue_index = create_ue();

    // Inject DL NAS transport message from AMF
    run_dl_nas_transport(ue_index);

    // Inject UL NAS transport message from RRC
    run_ul_nas_transport(ue_index);

    // Inject Initial Context Setup Request
    run_initial_context_setup(ue_index);

    return ue_index;
  }

  bool was_conversion_successful(ngap_message pdu_session_resource_setup_request, pdu_session_id_t pdu_session_id) const
  {
    bool test_1 = pdu_session_resource_setup_request.pdu.init_msg()
                      .value.pdu_session_res_setup_request()
                      ->pdu_session_res_setup_list_su_req.size() ==
                  du_processor_notifier->last_request.pdu_session_res_setup_items.size();

    bool test_2 =
        du_processor_notifier->last_request.pdu_session_res_setup_items[pdu_session_id].pdu_session_type == "ipv4";

    return test_1 && test_2;
  }

  bool was_pdu_session_resource_setup_request_valid() const
  {
    // Check that AMF notifier was called with right type
    bool test_1 = msg_notifier.last_ngap_msgs.back().pdu.successful_outcome().value.type() ==
                  asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::pdu_session_res_setup_resp;

    // Check that response contains PDU Session Resource Setup List
    bool test_2 = msg_notifier.last_ngap_msgs.back()
                      .pdu.successful_outcome()
                      .value.pdu_session_res_setup_resp()
                      ->pdu_session_res_setup_list_su_res_present;

    return test_1 && test_2;
  }

  bool was_pdu_session_resource_setup_request_invalid() const
  {
    // Check that a UE release was requested from the AMF
    bool test_1 = msg_notifier.last_ngap_msgs.back().pdu.init_msg().value.type() ==
                  asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_request;

    // Check that AMF notifier was called with right type
    bool test_2 = msg_notifier.last_ngap_msgs.end()[-2].pdu.successful_outcome().value.type() ==
                  asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::pdu_session_res_setup_resp;

    // Check that response doesn't contain PDU Session Resource Setup List
    bool test_3 = !msg_notifier.last_ngap_msgs.end()[-2]
                       .pdu.successful_outcome()
                       .value.pdu_session_res_setup_resp()
                       ->pdu_session_res_setup_list_su_res_present;

    // Check that response contains PDU Session Resource Failed to Setup List
    bool test_4 = msg_notifier.last_ngap_msgs.end()[-2]
                      .pdu.successful_outcome()
                      .value.pdu_session_res_setup_resp()
                      ->pdu_session_res_failed_to_setup_list_su_res_present;

    return test_1 && test_2 && test_3 && test_4;
  }

  bool was_error_indication_sent() const
  {
    return msg_notifier.last_ngap_msgs.back().pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::error_ind;
  }
};

/// Test missing PDU Session Resource Setup Request
TEST_F(ngap_pdu_session_resource_setup_procedure_test,
       when_pdu_session_resource_setup_request_is_not_received_then_ue_release_is_requested)
{
  ASSERT_EQ(ngap->get_nof_ues(), 0);

  // Test preamble
  this->start_procedure();

  // check that initial context setup request was received to the AMF and that UE object has been created
  ASSERT_EQ(msg_notifier.last_ngap_msgs.back().pdu.type().value,
            asn1::ngap::ngap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(msg_notifier.last_ngap_msgs.back().pdu.successful_outcome().value.type(),
            asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::init_context_setup_resp);
  ASSERT_EQ(ngap->get_nof_ues(), 1);

  // tick timers
  // Status: NGAP does not receive new PDU Session Resource Setup Request until pdu_session_setup_timer has ended.
  for (unsigned msec_elapsed = 0; msec_elapsed < cfg.pdu_session_setup_timeout.count() * 1000; ++msec_elapsed) {
    this->tick();
  }

  // check that UE release was requested
  ASSERT_EQ(msg_notifier.last_ngap_msgs.back().pdu.init_msg().value.type(),
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_request);
}

/// Test valid PDU Session Resource Setup Request
TEST_F(ngap_pdu_session_resource_setup_procedure_test,
       when_valid_pdu_session_resource_setup_request_received_then_pdu_session_setup_succeeds)
{
  // Test preamble
  ue_index_t ue_index = this->start_procedure();

  // Inject PDU Session Resource Setup Request
  pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(test_rgen::uniform_int<uint16_t>(
      pdu_session_id_to_uint(pdu_session_id_t::min), pdu_session_id_to_uint(pdu_session_id_t::max)));

  auto& ue = test_ues.at(ue_index);

  ngap_message pdu_session_resource_setup_request = generate_valid_pdu_session_resource_setup_request_message(
      ue.amf_ue_id.value(), ue.ran_ue_id.value(), pdu_session_id);
  ngap->handle_message(pdu_session_resource_setup_request);

  // Check conversion in adapter
  ASSERT_TRUE(was_conversion_successful(pdu_session_resource_setup_request, pdu_session_id));

  // Check that PDU Session Resource Setup Request was valid
  ASSERT_TRUE(was_pdu_session_resource_setup_request_valid());
}

/// Test invalid PDU Session Resource Setup Request
TEST_F(ngap_pdu_session_resource_setup_procedure_test,
       when_invalid_pdu_session_resource_setup_request_received_then_pdu_session_setup_failed)
{
  // Test preamble
  ue_index_t ue_index = this->start_procedure();

  auto& ue = test_ues.at(ue_index);

  // Inject invalid PDU Session Resource Setup Request
  ngap_message pdu_session_resource_setup_request =
      generate_invalid_pdu_session_resource_setup_request_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(pdu_session_resource_setup_request);

  // Check that PDU Session Resource Setup Request was invalid
  ASSERT_TRUE(was_pdu_session_resource_setup_request_invalid());
}

/// Test invalid PDU Session Resource Setup Request
TEST_F(ngap_pdu_session_resource_setup_procedure_test, when_security_not_enabled_then_pdu_session_setup_failed)
{
  // Test preamble
  ue_index_t ue_index = this->start_procedure();
  auto&      ue       = test_ues.at(ue_index);

  ue.rrc_ue_notifier.set_security_enabled(false);

  // Inject PDU Session Resource Setup Request
  pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(test_rgen::uniform_int<uint16_t>(
      pdu_session_id_to_uint(pdu_session_id_t::min), pdu_session_id_to_uint(pdu_session_id_t::max)));

  ngap_message pdu_session_resource_setup_request = generate_valid_pdu_session_resource_setup_request_message(
      ue.amf_ue_id.value(), ue.ran_ue_id.value(), pdu_session_id);
  ngap->handle_message(pdu_session_resource_setup_request);

  // Check that Error Indication has been sent to AMF
  ASSERT_TRUE(was_error_indication_sent());
}
