/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_test_helpers.h"
#include "srsgnb/support/async/async_test_utils.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
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
    return msg_notifier.last_ngc_msg.pdu.successful_outcome().value.type() ==
           asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::init_context_setup_resp;
  }

  bool was_initial_context_setup_failure_sent() const
  {
    return msg_notifier.last_ngc_msg.pdu.unsuccessful_outcome().value.type() ==
           asn1::ngap::ngap_elem_procs_o::unsuccessful_outcome_c::types_opts::init_context_setup_fail;
  }

  bool was_ue_added() const { return ngap->get_nof_ues() == 1; }

  bool was_ue_removed() const { return ngap->get_nof_ues() == 0; }
};

/// Test Initial Context Setup Request
TEST_F(ngap_ue_context_management_procedure_test, when_valid_initial_context_setup_request_received_then_response_send)
{
  // Test preamble
  ue_index_t ue_index = uint_to_ue_index(
      test_rgen::uniform_int<uint32_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max) - 1));
  this->start_procedure(ue_index);

  auto& ue = test_ues.at(ue_index);

  // Inject Initial Context Setup Request
  ngc_message init_context_setup_request =
      generate_valid_initial_context_setup_request_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(init_context_setup_request);

  // Check that AMF notifier was called with right type
  ASSERT_TRUE(was_initial_context_setup_response_sent());

  ASSERT_TRUE(was_ue_added());
}

/// Test Initial Context Setup Request
TEST_F(ngap_ue_context_management_procedure_test, when_invalid_initial_context_setup_request_received_then_failure_sent)
{
  // Test preamble
  ue_index_t ue_index = uint_to_ue_index(
      test_rgen::uniform_int<uint32_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max) - 1));
  this->start_procedure(ue_index);

  auto& ue = test_ues.at(ue_index);

  // Inject Initial Context Setup Request
  ngc_message init_context_setup_request =
      generate_invalid_initial_context_setup_request_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(init_context_setup_request);

  // Check that AMF notifier was called with right type
  ASSERT_TRUE(was_initial_context_setup_failure_sent());

  ASSERT_TRUE(was_initial_context_setup_failure_sent());

  ASSERT_TRUE(was_ue_removed());
}
