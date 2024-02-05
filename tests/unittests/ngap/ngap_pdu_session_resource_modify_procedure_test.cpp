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
#include "srsran/ran/cu_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class ngap_pdu_session_resource_modify_procedure_test : public ngap_test
{
protected:
  ue_index_t start_procedure(pdu_session_id_t pdu_session_id)
  {
    ue_index_t ue_index = create_ue();

    // Inject DL NAS transport message from AMF
    run_dl_nas_transport(ue_index);

    // Inject UL NAS transport message from RRC
    run_ul_nas_transport(ue_index);

    // Inject Initial Context Setup Request
    run_initial_context_setup(ue_index);

    // Inject PDU Session Resource Setup Request
    run_pdu_session_resource_setup(ue_index, pdu_session_id);
    add_pdu_session_to_up_manager(ue_index, pdu_session_id, uint_to_drb_id(1), uint_to_qos_flow_id(1));

    return ue_index;
  }

  bool was_conversion_successful(ngap_message     pdu_session_resource_modify_request,
                                 pdu_session_id_t pdu_session_id) const
  {
    bool test_1 = pdu_session_resource_modify_request.pdu.init_msg()
                      .value.pdu_session_res_modify_request()
                      ->pdu_session_res_modify_list_mod_req.size() ==
                  du_processor_notifier->last_modify_request.pdu_session_res_modify_items.size();

    bool test_2 =
        du_processor_notifier->last_modify_request.pdu_session_res_modify_items[pdu_session_id].pdu_session_id ==
        pdu_session_id;

    return test_1 && test_2;
  }

  bool was_pdu_session_resource_modify_request_valid() const
  {
    // Check that AMF notifier was called with right type
    bool test_1 = msg_notifier.last_ngap_msgs.back().pdu.successful_outcome().value.type() ==
                  asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::pdu_session_res_modify_resp;

    // Check that response contains PDU Session Resource Modify List
    bool test_2 = msg_notifier.last_ngap_msgs.back()
                      .pdu.successful_outcome()
                      .value.pdu_session_res_modify_resp()
                      ->pdu_session_res_modify_list_mod_res_present;

    return test_1 && test_2;
  }

  bool was_pdu_session_resource_modify_request_invalid() const
  {
    // Check that a UE release was requested from the AMF
    bool test_1 = msg_notifier.last_ngap_msgs.back().pdu.init_msg().value.type() ==
                  asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_request;

    // Check that AMF notifier was called with right type
    bool test_2 = msg_notifier.last_ngap_msgs.end()[-2].pdu.successful_outcome().value.type() ==
                  asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::pdu_session_res_modify_resp;

    // Check that response doesn't contain PDU Session Resource Modify List
    bool test_3 = !msg_notifier.last_ngap_msgs.end()[-2]
                       .pdu.successful_outcome()
                       .value.pdu_session_res_modify_resp()
                       ->pdu_session_res_modify_list_mod_res_present;

    // Check that response contains PDU Session Resource Failed to Modify List
    bool test_4 = msg_notifier.last_ngap_msgs.end()[-2]
                      .pdu.successful_outcome()
                      .value.pdu_session_res_modify_resp()
                      ->pdu_session_res_failed_to_modify_list_mod_res_present;

    return test_1 && test_2 && test_3 && test_4;
  }

  bool was_error_indication_sent() const
  {
    return msg_notifier.last_ngap_msgs.back().pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::error_ind;
  }
};

/// Test valid PDU Session Resource Modify Request
TEST_F(ngap_pdu_session_resource_modify_procedure_test,
       when_valid_pdu_session_resource_modify_request_received_then_pdu_session_modification_succeeds)
{
  // Test preamble
  pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(test_rgen::uniform_int<uint16_t>(
      pdu_session_id_to_uint(pdu_session_id_t::min), pdu_session_id_to_uint(pdu_session_id_t::max)));
  ue_index_t       ue_index       = this->start_procedure(pdu_session_id);
  auto&            ue             = test_ues.at(ue_index);

  // Inject PDU Session Resource Modify Request
  ngap_message pdu_session_resource_modify_request = generate_valid_pdu_session_resource_modify_request_message(
      ue.amf_ue_id.value(), ue.ran_ue_id.value(), pdu_session_id);
  ngap->handle_message(pdu_session_resource_modify_request);

  // Check conversion in adapter
  ASSERT_TRUE(was_conversion_successful(pdu_session_resource_modify_request, pdu_session_id));

  // Check that PDU Session Resource Modify Request was valid
  ASSERT_TRUE(was_pdu_session_resource_modify_request_valid());
}

/// Test invalid PDU Session Resource Modify Request
TEST_F(ngap_pdu_session_resource_modify_procedure_test,
       when_invalid_pdu_session_resource_modify_request_received_then_pdu_session_modification_failed)
{
  // Test preamble
  pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(test_rgen::uniform_int<uint16_t>(
      pdu_session_id_to_uint(pdu_session_id_t::min), pdu_session_id_to_uint(pdu_session_id_t::max)));
  ue_index_t       ue_index       = this->start_procedure(pdu_session_id);
  auto&            ue             = test_ues.at(ue_index);

  // Inject invalid PDU Session Resource Modify Request
  ngap_message pdu_session_resource_modify_request = generate_invalid_pdu_session_resource_modify_request_message(
      ue.amf_ue_id.value(), ue.ran_ue_id.value(), pdu_session_id);
  ngap->handle_message(pdu_session_resource_modify_request);

  // Check that PDU Session Resource Modify Request was invalid
  ASSERT_TRUE(was_pdu_session_resource_modify_request_invalid());
}

/// Test valid PDU Session Resource Modify Request
TEST_F(ngap_pdu_session_resource_modify_procedure_test,
       when_valid_pdu_session_resource_modify_request_received_twice_then_error_indication_is_send)
{
  // Test preamble
  pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(test_rgen::uniform_int<uint16_t>(
      pdu_session_id_to_uint(pdu_session_id_t::min), pdu_session_id_to_uint(pdu_session_id_t::max)));
  ue_index_t       ue_index       = this->start_procedure(pdu_session_id);
  auto&            ue             = test_ues.at(ue_index);

  // Inject PDU Session Resource Modify Request
  ngap_message pdu_session_resource_modify_request = generate_valid_pdu_session_resource_modify_request_message(
      ue.amf_ue_id.value(), ue.ran_ue_id.value(), pdu_session_id);
  ngap->handle_message(pdu_session_resource_modify_request);

  // Check conversion in adapter
  ASSERT_TRUE(was_conversion_successful(pdu_session_resource_modify_request, pdu_session_id));

  // Check that PDU Session Resource Modify Request was valid
  ASSERT_TRUE(was_pdu_session_resource_modify_request_valid());

  // Inject same PDU Session Resource Modify Request again
  ngap->handle_message(pdu_session_resource_modify_request);

  ASSERT_TRUE(was_error_indication_sent());
}
