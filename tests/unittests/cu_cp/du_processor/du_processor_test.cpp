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

#include "../du_processor_test_messages.h"
#include "du_processor_test_helpers.h"
#include "lib/cu_cp/du_processor/du_processor.h"
#include "tests/unittests/f1ap/common/f1ap_cu_test_messages.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::f1ap;

//////////////////////////////////////////////////////////////////////////////////////
/* F1 setup                                                                         */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the successful f1 setup procedure
TEST_F(du_processor_test, when_valid_f1setup_received_then_f1_setup_response_sent)
{
  // Pass F1 Setup Request to DU processor
  f1ap_message f1_setup_req = test_helpers::generate_f1_setup_request();
  du_processor_obj->get_f1ap_interface().get_f1ap_handler().get_f1ap_message_handler().handle_message(f1_setup_req);

  // Check response is F1SetupResponse
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.type(), f1ap_pdu_c::types_opts::options::successful_outcome);
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.successful_outcome().value.type(),
            f1ap_elem_procs_o::successful_outcome_c::types_opts::options::f1_setup_resp);
}

TEST_F(du_processor_test, when_du_served_cells_list_missing_then_f1setup_rejected)
{
  // Generate F1SetupRequest with missing du served cells list
  f1ap_message f1_setup_req = test_helpers::generate_f1_setup_request();
  f1_setup_req.pdu.init_msg().value.f1_setup_request()->gnb_du_served_cells_list_present = false;
  f1_setup_req.pdu.init_msg().value.f1_setup_request()->gnb_du_served_cells_list.clear();

  // Pass message to DU processor
  du_processor_obj->get_f1ap_interface().get_f1ap_handler().get_f1ap_message_handler().handle_message(f1_setup_req);

  // Check the generated PDU is indeed the F1 Setup failure
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.type(), f1ap_pdu_c::types_opts::options::unsuccessful_outcome);
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.unsuccessful_outcome().value.type(),
            f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail);
}

TEST_F(du_processor_test, when_gnb_du_sys_info_missing_then_f1setup_rejected)
{
  // Generate F1SetupRequest with missing gnb du sys info
  f1ap_message f1_setup_req = test_helpers::generate_f1_setup_request();
  f1_setup_req.pdu.init_msg()
      .value.f1_setup_request()
      ->gnb_du_served_cells_list[0]
      .value()
      .gnb_du_served_cells_item()
      .gnb_du_sys_info_present = false;

  // Pass message to DU processor
  du_processor_obj->get_f1ap_interface().get_f1ap_handler().get_f1ap_message_handler().handle_message(f1_setup_req);

  // Check the generated PDU is indeed the F1 Setup failure
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.type(), f1ap_pdu_c::types_opts::options::unsuccessful_outcome);
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.unsuccessful_outcome().value.type(),
            f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail);
}

TEST_F(du_processor_test, when_max_nof_du_cells_exeeded_then_f1setup_rejected)
{
  // Generate F1SetupRequest with too many cells
  f1ap_message f1ap_msg = create_f1_setup_request_with_too_many_cells();

  // Pass message to DU processor
  du_processor_obj->get_f1ap_interface().get_f1ap_handler().get_f1ap_message_handler().handle_message(f1ap_msg);

  // Check the generated PDU is indeed the F1 Setup failure
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.type(), f1ap_pdu_c::types_opts::options::unsuccessful_outcome);
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.unsuccessful_outcome().value.type(),
            f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail);
}

//////////////////////////////////////////////////////////////////////////////////////
/* UE creation                                                                      */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(du_processor_test, when_ue_creation_msg_valid_then_ue_added)
{
  // Pass message to DU processor
  du_processor_obj->get_f1ap_interface().get_f1ap_handler().get_f1ap_message_handler().handle_message(
      test_helpers::generate_f1_setup_request());

  // Generate ue_creation message
  ue_rrc_context_creation_request req = generate_ue_rrc_context_creation_request(
      ue_index_t::invalid, rnti_t::MIN_CRNTI, nr_cell_identity::create(gnb_id_t{411, 22}, 0).value());

  // Pass message to DU processor
  ue_rrc_context_creation_outcome outcome =
      du_processor_obj->get_f1ap_interface().handle_ue_rrc_context_creation_request(req);
  ASSERT_TRUE(outcome.has_value());

  ASSERT_EQ(du_processor_obj->get_statistics_handler().get_nof_ues(), 1);
}

TEST_F(du_processor_test, when_cell_id_invalid_then_ue_creation_fails)
{
  // Generate valid F1SetupRequest and pass it to DU processor
  du_processor_obj->get_f1ap_interface().get_f1ap_handler().get_f1ap_message_handler().handle_message(
      test_helpers::generate_f1_setup_request());

  // Generate ue_creation message
  ue_rrc_context_creation_request req = generate_ue_rrc_context_creation_request(
      ue_index_t::invalid, rnti_t::MIN_CRNTI, nr_cell_identity::create(1).value());

  // Pass message to DU processor
  ue_rrc_context_creation_outcome outcome =
      du_processor_obj->get_f1ap_interface().handle_ue_rrc_context_creation_request(req);
  ASSERT_TRUE(not outcome.has_value());
}

TEST_F(du_processor_test, when_ue_rrc_context_exists_then_new_ue_rrc_context_not_added)
{
  // Generate valid F1SetupRequest
  du_setup_request f1_setup_request;
  generate_valid_f1_setup_request(f1_setup_request);

  // Pass message to DU processor
  du_processor_obj->get_f1ap_interface().get_f1ap_handler().get_f1ap_message_handler().handle_message(
      test_helpers::generate_f1_setup_request());

  // Generate ue_creation message
  ue_rrc_context_creation_request req = generate_ue_rrc_context_creation_request(
      ue_index_t::invalid, rnti_t::MIN_CRNTI, nr_cell_identity::create(gnb_id_t{411, 22}, 0).value());

  // Pass message to DU processor
  ue_rrc_context_creation_outcome outcome =
      du_processor_obj->get_f1ap_interface().handle_ue_rrc_context_creation_request(req);
  ASSERT_TRUE(outcome.has_value());

  ASSERT_EQ(du_processor_obj->get_statistics_handler().get_nof_ues(), 1);

  // Pass same message to DU processor again
  req.ue_index = outcome->ue_index;
  ue_rrc_context_creation_outcome outcome2 =
      du_processor_obj->get_f1ap_interface().handle_ue_rrc_context_creation_request(req);
  ASSERT_TRUE(not outcome2.has_value());

  ASSERT_EQ(du_processor_obj->get_statistics_handler().get_nof_ues(), 1);
}

TEST_F(du_processor_test, when_max_nof_ues_exceeded_then_ue_not_added)
{
  // Generate valid F1SetupRequest and pass it to DU processor
  du_processor_obj->get_f1ap_interface().get_f1ap_handler().get_f1ap_message_handler().handle_message(
      test_helpers::generate_f1_setup_request());

  // Reduce logger loglevel to warning to reduce console output
  srslog::fetch_basic_logger("CU-CP").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("RRC").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("CU-UEMNG").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::warning);

  // Add the maximum number of UEs
  for (unsigned it = 0; it < ue_mng.get_ue_config().max_nof_supported_ues; it++) {
    // Generate ue_creation message
    rnti_t                          c_rnti = to_rnti(it + 1); // 0 is not a valid RNTI
    ue_rrc_context_creation_request req    = generate_ue_rrc_context_creation_request(
        ue_index_t::invalid, c_rnti, nr_cell_identity::create(gnb_id_t{411, 22}, 0).value());

    // Pass message to DU processor
    ue_rrc_context_creation_outcome outcome =
        du_processor_obj->get_f1ap_interface().handle_ue_rrc_context_creation_request(req);
    ASSERT_TRUE(outcome.has_value());
  }

  // Reset logger loglevel
  srslog::fetch_basic_logger("CU-CP").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("RRC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("CU-UEMNG").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);

  ASSERT_EQ(du_processor_obj->get_statistics_handler().get_nof_ues(), ue_mng.get_ue_config().max_nof_supported_ues);

  // Try to add additional UE
  rnti_t                          c_rnti = to_rnti(ue_mng.get_ue_config().max_nof_supported_ues + 1);
  ue_rrc_context_creation_request req    = generate_ue_rrc_context_creation_request(
      ue_index_t::invalid, c_rnti, nr_cell_identity::create(gnb_id_t{411, 22}, 0).value());
  ue_rrc_context_creation_outcome outcome =
      du_processor_obj->get_f1ap_interface().handle_ue_rrc_context_creation_request(req);
  ASSERT_TRUE(not outcome.has_value());

  ASSERT_EQ(du_processor_obj->get_statistics_handler().get_nof_ues(), ue_mng.get_ue_config().max_nof_supported_ues);
}
