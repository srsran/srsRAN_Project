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

class ngap_pdu_session_resource_setup_procedure_test : public ngap_test
{
protected:
  void start_procedure(const cu_cp_ue_id_t cu_cp_ue_id)
  {
    ASSERT_EQ(ngap->get_nof_ues(), 0);
    create_ue(cu_cp_ue_id);

    // Inject DL NAS transport message from AMF
    run_dl_nas_transport(cu_cp_ue_id);

    // Inject UL NAS transport message from RRC
    run_ul_nas_transport(cu_cp_ue_id);

    // Inject Initial Context Setup Request
    run_inital_context_setup(cu_cp_ue_id);
  }

  bool was_conversion_successful(ngc_message pdu_session_resource_setup_request, pdu_session_id_t pdu_session_id) const
  {
    bool test_1 = pdu_session_resource_setup_request.pdu.init_msg()
                      .value.pdu_session_res_setup_request()
                      ->pdu_session_res_setup_list_su_req.value.size() ==
                  du_processor_notifier.last_request.pdu_session_res_setup_items.size();

    bool test_2 =
        du_processor_notifier.last_request.pdu_session_res_setup_items[pdu_session_id].pdu_session_type == "ipv4";

    return test_1 && test_2;
  }

  bool was_pdu_session_resource_setup_request_valid() const
  {
    // Check that AMF notifier was called with right type
    bool test_1 = msg_notifier.last_ngc_msg.pdu.successful_outcome().value.type() ==
                  asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::pdu_session_res_setup_resp;

    // Check that response contains PDU Session Resource Setup List
    bool test_2 = msg_notifier.last_ngc_msg.pdu.successful_outcome()
                      .value.pdu_session_res_setup_resp()
                      ->pdu_session_res_setup_list_su_res_present;

    return test_1 && test_2;
  }

  bool was_pdu_session_resource_setup_request_invalid() const
  {
    // Check that AMF notifier was called with right type
    bool test_1 = msg_notifier.last_ngc_msg.pdu.successful_outcome().value.type() ==
                  asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::pdu_session_res_setup_resp;

    // Check that response contains PDU Session Resource Setup List
    bool test_2 = !msg_notifier.last_ngc_msg.pdu.successful_outcome()
                       .value.pdu_session_res_setup_resp()
                       ->pdu_session_res_setup_list_su_res_present;

    // Check that response contains PDU Session Resource Failed to Setup List
    bool test_3 = msg_notifier.last_ngc_msg.pdu.successful_outcome()
                      .value.pdu_session_res_setup_resp()
                      ->pdu_session_res_failed_to_setup_list_su_res_present;

    return test_1 && test_2 && test_3;
  }
};

/// Test Initial Context Setup Request
TEST_F(ngap_pdu_session_resource_setup_procedure_test,
       when_valid_pdu_session_resource_setup_request_received_then_pdu_session_setup_succeeds)
{
  // Test preamble
  cu_cp_ue_id_t cu_cp_ue_id = uint_to_cu_cp_ue_id(test_rgen::uniform_int<uint32_t>(
      cu_cp_ue_id_to_uint(cu_cp_ue_id_t::min), cu_cp_ue_id_to_uint(cu_cp_ue_id_t::max) - 1));
  this->start_procedure(cu_cp_ue_id);

  // Inject PDU Session Resource Setup Request
  pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(test_rgen::uniform_int<uint32_t>(
      pdu_session_id_to_uint(pdu_session_id_t::min), pdu_session_id_to_uint(pdu_session_id_t::max) - 1));

  auto& ue = test_ues[cu_cp_ue_id];

  ngc_message pdu_session_resource_setup_request = generate_valid_pdu_session_resource_setup_request_message(
      ue.amf_ue_id.value(), ue.ran_ue_id.value(), pdu_session_id);
  ngap->handle_message(pdu_session_resource_setup_request);

  // Check conversion in adapter
  ASSERT_TRUE(was_conversion_successful(pdu_session_resource_setup_request, pdu_session_id));

  // Check that PDU Session Resource Setup Request was valid
  ASSERT_TRUE(was_pdu_session_resource_setup_request_valid());
}

/// Test Initial Context Setup Request
TEST_F(ngap_pdu_session_resource_setup_procedure_test,
       when_invalid_pdu_session_resource_setup_request_received_then_pdu_session_setup_failed)
{
  // Test preamble
  cu_cp_ue_id_t cu_cp_ue_id = uint_to_cu_cp_ue_id(test_rgen::uniform_int<uint32_t>(
      cu_cp_ue_id_to_uint(cu_cp_ue_id_t::min), cu_cp_ue_id_to_uint(cu_cp_ue_id_t::max) - 1));
  this->start_procedure(cu_cp_ue_id);

  auto& ue = test_ues[cu_cp_ue_id];

  // Inject invalid PDU Session Resource Setup Request
  ngc_message pdu_session_resource_setup_request =
      generate_invalid_pdu_session_resource_setup_request_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(pdu_session_resource_setup_request);

  // Check that PDU Session Resource Setup Request was invalid
  ASSERT_TRUE(was_pdu_session_resource_setup_request_invalid());
}
