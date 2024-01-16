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
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class ngap_ue_context_management_procedure_test : public ngap_test
{
protected:
  ue_index_t start_procedure()
  {
    ue_index_t ue_index = create_ue();

    // Inject DL NAS transport message from AMF
    run_dl_nas_transport(ue_index);

    // Inject UL NAS transport message from RRC
    run_ul_nas_transport(ue_index);

    return ue_index;
  }

  bool was_initial_context_setup_response_sent() const
  {
    return msg_notifier.last_ngap_msgs.back().pdu.successful_outcome().value.type() ==
           asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::init_context_setup_resp;
  }

  bool was_initial_context_setup_failure_sent() const
  {
    return msg_notifier.last_ngap_msgs.back().pdu.unsuccessful_outcome().value.type() ==
           asn1::ngap::ngap_elem_procs_o::unsuccessful_outcome_c::types_opts::init_context_setup_fail;
  }

  bool was_pdu_session_resource_setup_successful() const
  {
    bool setup_present = msg_notifier.last_ngap_msgs.back()
                             .pdu.successful_outcome()
                             .value.init_context_setup_resp()
                             ->pdu_session_res_setup_list_cxt_res_present == true;

    bool fail_present = msg_notifier.last_ngap_msgs.back()
                            .pdu.successful_outcome()
                            .value.init_context_setup_resp()
                            ->pdu_session_res_failed_to_setup_list_cxt_res_present == false;

    return setup_present && fail_present;
  }

  bool was_pdu_session_resource_setup_unsuccessful() const
  {
    return msg_notifier.last_ngap_msgs.back()
               .pdu.unsuccessful_outcome()
               .value.init_context_setup_fail()
               ->pdu_session_res_failed_to_setup_list_cxt_fail_present == true;
  }

  bool was_ue_context_release_request_sent() const
  {
    if (msg_notifier.last_ngap_msgs.back().pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::nulltype) {
      return false;
    }
    return msg_notifier.last_ngap_msgs.back().pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_request;
  }

  bool was_ue_context_release_complete_sent() const
  {
    return msg_notifier.last_ngap_msgs.back().pdu.successful_outcome().value.type() ==
           asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::ue_context_release_complete;
  }

  bool was_ue_added() const { return ngap->get_nof_ues() == 1; }

  bool was_ue_removed() const { return ngap->get_nof_ues() == 0; }

  void clear_last_received_msg() { msg_notifier.last_ngap_msgs.back() = {}; }
};

/// Test Initial Context Setup Request
TEST_F(ngap_ue_context_management_procedure_test, when_valid_initial_context_setup_request_received_then_response_send)
{
  // Test preamble
  ue_index_t ue_index = this->start_procedure();

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
  ue_index_t ue_index = this->start_procedure();

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
  ue_index_t ue_index = this->start_procedure();

  auto& ue = test_ues.at(ue_index);

  // Get "first" AMF UE ID received
  amf_ue_id_t old_id = ue.amf_ue_id.value();

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
}

/// Test invalid Initial Context Setup Request
TEST_F(ngap_ue_context_management_procedure_test, when_invalid_initial_context_setup_request_received_then_failure_sent)
{
  // Test preamble
  ue_index_t ue_index = this->start_procedure();

  auto& ue = test_ues.at(ue_index);

  // Inject Initial Context Setup Request
  ngap_message init_context_setup_request =
      generate_invalid_initial_context_setup_request_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(init_context_setup_request);

  // Check that AMF notifier was called with right type
  ASSERT_TRUE(was_initial_context_setup_failure_sent());
}

/// Test invalid Initial Context Setup Request with PDUSessionResourceSetupListCxtReq
TEST_F(ngap_ue_context_management_procedure_test,
       when_invalid_initial_context_setup_request_with_pdu_session_received_then_failure_sent)
{
  // Test preamble
  ue_index_t ue_index = this->start_procedure();

  auto& ue = test_ues.at(ue_index);

  // Inject Initial Context Setup Request
  ngap_message init_context_setup_request = generate_invalid_initial_context_setup_request_message_with_pdu_session(
      ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(init_context_setup_request);

  // Check that AMF notifier was called with right type
  ASSERT_TRUE(was_initial_context_setup_failure_sent());

  ASSERT_TRUE(was_pdu_session_resource_setup_unsuccessful());
}

/// Test successful UE context release
TEST_F(ngap_ue_context_management_procedure_test,
       when_ue_context_release_command_with_amf_ue_ngap_id_received_then_ue_is_released_and_release_complete_is_sent)
{
  // Test preamble
  ue_index_t ue_index = this->start_procedure();

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

/// Initial UE message tests
TEST_F(ngap_ue_context_management_procedure_test,
       when_release_command_after_initial_ue_message_is_received_then_ue_is_released)
{
  ASSERT_EQ(ngap->get_nof_ues(), 0);

  // Test preamble
  ue_index_t ue_index = create_ue();

  auto& ue = test_ues.at(ue_index);

  // Inject DL NAS transport message from AMF
  run_dl_nas_transport(ue_index);

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
  ue_index_t ue_index = this->start_procedure();

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
  ue_index_t ue_index = this->start_procedure();

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
  ue_index_t ue_index = create_ue();

  // Trigger UE context release request.
  cu_cp_ue_context_release_request release_request;
  release_request.ue_index = ue_index;
  ngap->handle_ue_context_release_request(release_request);

  ASSERT_FALSE(was_ue_context_release_request_sent());
}

/// Test UE context release request is not sent multiple times for same UE.
TEST_F(ngap_ue_context_management_procedure_test,
       when_ue_context_release_request_is_received_multiple_times_ngap_message_is_not_sent_more_than_once)
{
  // Test preamble
  ue_index_t ue_index = this->start_procedure();

  auto& ue = test_ues.at(ue_index);

  // Inject Initial Context Setup Request
  ngap_message init_context_setup_request =
      generate_valid_initial_context_setup_request_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(init_context_setup_request);

  ASSERT_TRUE(was_ue_added());

  // Trigger UE context release request.
  cu_cp_ue_context_release_request release_request;
  release_request.ue_index = ue_index;
  ngap->handle_ue_context_release_request(release_request);

  ASSERT_TRUE(was_ue_context_release_request_sent());

  // Trigger 2nd UE context release request.
  clear_last_received_msg();
  ngap->handle_ue_context_release_request(release_request);
  ASSERT_FALSE(was_ue_context_release_request_sent());
}

/// Test DL NAS transport after transfering UE IDs/context.
TEST_F(ngap_ue_context_management_procedure_test, when_ue_context_is_tranfered_amf_ue_id_is_updated)
{
  // Normal test preamble to get UE created.
  ue_index_t ue_index = this->start_procedure();
  auto&      ue       = test_ues.at(ue_index);
  ASSERT_NE(ue.ue_index, ue_index_t::invalid);

  // Get AMF UE ID
  amf_ue_id_t amf_id = ue.amf_ue_id.value();
  ASSERT_NE(amf_id, amf_ue_id_t::invalid);

  ran_ue_id_t ran_id = ue.ran_ue_id.value();
  ASSERT_NE(ran_id, ran_ue_id_t::invalid);

  // Clear NAS PDU.
  ue.rrc_ue_notifier.last_nas_pdu.clear();
  ASSERT_TRUE(ue.rrc_ue_notifier.last_nas_pdu.empty());

  // Inject new DL NAS transport from core.
  ngap_message dl_nas_transport = generate_downlink_nas_transport_message(amf_id, ue.ran_ue_id.value());
  ngap->handle_message(dl_nas_transport);

  // Check NAS PDU has been passed to RRC.
  ASSERT_FALSE(ue.rrc_ue_notifier.last_nas_pdu.empty());

  // Clear PDU again.
  ue.rrc_ue_notifier.last_nas_pdu.clear();

  // Create new UE object (with own RRC UE notifier).
  ue_index_t target_ue_index = create_ue(rnti_t::MAX_CRNTI);
  ASSERT_NE(target_ue_index, ue_index_t::invalid);
  auto& target_ue = test_ues.at(target_ue_index);
  ASSERT_TRUE(target_ue.rrc_ue_notifier.last_nas_pdu.empty());

  // Transfer NGAP UE context to new target UE.
  ngap->update_ue_index(target_ue_index, ue_index);

  // Inject NAS message again.
  ngap->handle_message(dl_nas_transport);

  // Check that RRC notifier of initial UE has not been called.
  ASSERT_TRUE(ue.rrc_ue_notifier.last_nas_pdu.empty());

  // Verify that RRC notifier of target UE has indeed benn called.
  ASSERT_FALSE(target_ue.rrc_ue_notifier.last_nas_pdu.empty());
}
