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

#include "ngap_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class ngap_ue_context_management_procedure_test : public ngap_test
{
protected:
  void start_procedure(const ue_index_t ue_index)
  {
    ASSERT_EQ(ngap->get_nof_ues(), 0);
    create_ue(ue_index);

    // Inject DL NAS transport message from AMF
    run_dl_nas_transport(ue_index);

    // Inject UL NAS transport message from RRC
    run_ul_nas_transport(ue_index);
  }

  bool was_initial_context_setup_response_sent() const
  {
    return msg_notifier.last_ngap_msg.pdu.successful_outcome().value.type() ==
           asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::init_context_setup_resp;
  }

  bool was_initial_context_setup_failure_sent() const
  {
    return msg_notifier.last_ngap_msg.pdu.unsuccessful_outcome().value.type() ==
           asn1::ngap::ngap_elem_procs_o::unsuccessful_outcome_c::types_opts::init_context_setup_fail;
  }

  bool was_pdu_session_resource_setup_successful() const
  {
    bool setup_present = msg_notifier.last_ngap_msg.pdu.successful_outcome()
                             .value.init_context_setup_resp()
                             ->pdu_session_res_setup_list_cxt_res_present == true;

    bool fail_present = msg_notifier.last_ngap_msg.pdu.successful_outcome()
                            .value.init_context_setup_resp()
                            ->pdu_session_res_failed_to_setup_list_cxt_res_present == false;

    return setup_present && fail_present;
  }

  bool was_pdu_session_resource_setup_unsuccessful() const
  {
    return msg_notifier.last_ngap_msg.pdu.unsuccessful_outcome()
               .value.init_context_setup_fail()
               ->pdu_session_res_failed_to_setup_list_cxt_fail_present == true;
  }

  bool was_ue_context_release_request_sent() const
  {
    return msg_notifier.last_ngap_msg.pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_request;
  }

  bool was_ue_context_release_complete_sent() const
  {
    return msg_notifier.last_ngap_msg.pdu.successful_outcome().value.type() ==
           asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::ue_context_release_complete;
  }

  bool was_ue_added() const { return ngap->get_nof_ues() == 1; }

  bool was_ue_removed() const { return ngap->get_nof_ues() == 0; }
};

/// Test Initial Context Setup Request
TEST_F(ngap_ue_context_management_procedure_test, when_valid_initial_context_setup_request_received_then_response_send)
{
  // Test preamble
  ue_index_t ue_index = uint_to_ue_index(
      test_rgen::uniform_int<uint64_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max)));
  this->start_procedure(ue_index);

  auto& ue = test_ues.at(ue_index);

  // Inject Initial Context Setup Request
  ngap_message init_context_setup_request =
      generate_valid_initial_context_setup_request_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(init_context_setup_request);

  // Check that AMF notifier was called with right type
  ASSERT_TRUE(was_initial_context_setup_response_sent());

  ASSERT_TRUE(was_ue_added());
}

/// Test Initial Context Setup Request with PDUSessionResourceSetupListCxtReq
TEST_F(ngap_ue_context_management_procedure_test,
       when_initial_context_setup_request_with_pdu_session_received_then_response_send)
{
  // Test preamble
  ue_index_t ue_index = uint_to_ue_index(
      test_rgen::uniform_int<uint64_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max)));
  this->start_procedure(ue_index);

  auto& ue = test_ues.at(ue_index);

  // Inject Initial Context Setup Request
  ngap_message init_context_setup_request =
      generate_valid_initial_context_setup_request_message_with_pdu_session(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(init_context_setup_request);

  // Check that AMF notifier was called with right type
  ASSERT_TRUE(was_initial_context_setup_response_sent());

  ASSERT_TRUE(was_ue_added());

  ASSERT_TRUE(was_pdu_session_resource_setup_successful());
}

/// Test Initial Context Setup Request with "updated" AMF UE ID
TEST_F(ngap_ue_context_management_procedure_test,
       when_new_amf_ue_id_is_sent_in_initial_context_setup_request_received_then_id_is_updated)
{
  // Test preamble
  ue_index_t ue_index = uint_to_ue_index(
      test_rgen::uniform_int<uint64_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max)));
  this->start_procedure(ue_index);

  auto& ue = test_ues.at(ue_index);

  // Get "first" AMF UE ID received
  amf_ue_id_t old_id = ue.amf_ue_id.value();

  // Lookup UE in UE manager
  ngap_ue* ngap_ue = ue_mng.find_ngap_ue(ue_index);

  // Check that UE manager has the same AMF UE ID
  ASSERT_EQ(ngap_ue->get_amf_ue_id(), old_id);

  // randomly generate new ID assigned by core
  amf_ue_id_t new_id = old_id;
  while (new_id == old_id) {
    new_id = uint_to_amf_ue_id(
        test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));
  }
  ASSERT_NE(old_id, new_id);

  // Inject Initial Context Setup Request with new ID
  ngap_message init_context_setup_request =
      generate_valid_initial_context_setup_request_message(new_id, ue.ran_ue_id.value());
  ngap->handle_message(init_context_setup_request);

  // Check that AMF notifier was called with right type
  ASSERT_TRUE(was_initial_context_setup_response_sent());

  ASSERT_TRUE(was_ue_added());

  // Check that UE has new AMF UE ID
  ASSERT_EQ(ngap_ue->get_amf_ue_id(), new_id);
}

/// Test invalid Initial Context Setup Request
TEST_F(ngap_ue_context_management_procedure_test, when_invalid_initial_context_setup_request_received_then_failure_sent)
{
  // Test preamble
  ue_index_t ue_index = uint_to_ue_index(
      test_rgen::uniform_int<uint64_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max)));
  this->start_procedure(ue_index);

  auto& ue = test_ues.at(ue_index);

  // Inject Initial Context Setup Request
  ngap_message init_context_setup_request =
      generate_invalid_initial_context_setup_request_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(init_context_setup_request);

  // Check that AMF notifier was called with right type
  ASSERT_TRUE(was_initial_context_setup_failure_sent());

  ASSERT_TRUE(was_ue_removed());
}

/// Test invalid Initial Context Setup Request with PDUSessionResourceSetupListCxtReq
TEST_F(ngap_ue_context_management_procedure_test,
       when_invalid_initial_context_setup_request_with_pdu_session_received_then_failure_sent)
{
  // Test preamble
  ue_index_t ue_index = uint_to_ue_index(
      test_rgen::uniform_int<uint64_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max)));
  this->start_procedure(ue_index);

  auto& ue = test_ues.at(ue_index);

  // Inject Initial Context Setup Request
  ngap_message init_context_setup_request = generate_invalid_initial_context_setup_request_message_with_pdu_session(
      ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(init_context_setup_request);

  // Check that AMF notifier was called with right type
  ASSERT_TRUE(was_initial_context_setup_failure_sent());

  ASSERT_TRUE(was_ue_removed());

  ASSERT_TRUE(was_pdu_session_resource_setup_unsuccessful());
}

/// Test successful UE context release
TEST_F(ngap_ue_context_management_procedure_test,
       when_ue_context_release_command_with_amf_ue_ngap_id_received_then_ue_is_released_and_release_complete_is_sent)
{
  // Test preamble
  ue_index_t ue_index = uint_to_ue_index(
      test_rgen::uniform_int<uint64_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max)));
  this->start_procedure(ue_index);

  auto& ue = test_ues.at(ue_index);

  // Inject Initial Context Setup Request
  ngap_message init_context_setup_request =
      generate_valid_initial_context_setup_request_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(init_context_setup_request);

  ASSERT_TRUE(was_ue_added());

  // Inject UE Context Release Command
  ngap_message ue_context_release_cmd =
      generate_valid_ue_context_release_command_with_amf_ue_ngap_id(ue.amf_ue_id.value());
  ngap->handle_message(ue_context_release_cmd);

  ASSERT_TRUE(was_ue_context_release_complete_sent());
  ASSERT_TRUE(was_ue_removed());
}

/// Test successful UE context release
TEST_F(ngap_ue_context_management_procedure_test,
       when_ue_context_release_command_with_ue_ngap_id_pair_received_then_ue_is_released_and_release_complete_is_sent)
{
  // Test preamble
  ue_index_t ue_index = uint_to_ue_index(
      test_rgen::uniform_int<uint64_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max)));
  this->start_procedure(ue_index);

  auto& ue = test_ues.at(ue_index);

  // Inject Initial Context Setup Request
  ngap_message init_context_setup_request =
      generate_valid_initial_context_setup_request_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(init_context_setup_request);

  ASSERT_TRUE(was_ue_added());

  // Inject UE Context Release Command
  ngap_message ue_context_release_cmd =
      generate_valid_ue_context_release_command_with_ue_ngap_id_pair(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(ue_context_release_cmd);

  ASSERT_TRUE(was_ue_context_release_complete_sent());
  ASSERT_TRUE(was_ue_removed());
}

/// Test UE context release for unknown UE
TEST_F(ngap_ue_context_management_procedure_test,
       when_ue_context_release_command_for_unknown_ue_received_then_ue_is_not_released_and_release_complete_is_not_sent)
{
  // Test preamble
  ue_index_t ue_index = uint_to_ue_index(
      test_rgen::uniform_int<uint64_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max)));
  this->start_procedure(ue_index);

  auto& ue = test_ues.at(ue_index);

  // Inject Initial Context Setup Request
  ngap_message init_context_setup_request =
      generate_valid_initial_context_setup_request_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(init_context_setup_request);

  ASSERT_TRUE(was_ue_added());

  // Inject UE Context Release Command for unknown UE
  amf_ue_id_t unknown_ue_id = uint_to_amf_ue_id(amf_ue_id_to_uint(ue.amf_ue_id.value()) + 1);

  ngap_message ue_context_release_cmd = generate_valid_ue_context_release_command_with_amf_ue_ngap_id(unknown_ue_id);
  ngap->handle_message(ue_context_release_cmd);

  ASSERT_FALSE(was_ue_context_release_complete_sent());
  ASSERT_FALSE(was_ue_removed());
}

/// Test UE context release request for UE that hasn't received an AMF UE ID yet.
TEST_F(ngap_ue_context_management_procedure_test,
       when_ue_context_release_request_is_received_but_no_amf_ue_ngap_id_is_set_then_request_is_ignored)
{
  // Test preamble - Only create UE but do not have DL traffic from the AMF.
  ue_index_t ue_index = uint_to_ue_index(
      test_rgen::uniform_int<uint64_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max)));
  create_ue(ue_index);

  // Trigger UE context release request.
  cu_cp_ue_context_release_request release_request;
  release_request.ue_index = ue_index;
  ngap->handle_ue_context_release_request(release_request);

  ASSERT_FALSE(was_ue_context_release_request_sent());
}