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

#include "du_processor_test_helpers.h"
#include "lib/cu_cp/du_processor/du_processor_impl_interface.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
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
  f1ap_message f1_setup_req = generate_f1_setup_request();
  du_processor_obj->get_du_processor_f1ap_interface().get_f1ap_message_handler().handle_message(f1_setup_req);

  // Check response is F1SetupResponse
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.type(), f1ap_pdu_c::types_opts::options::successful_outcome);
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.successful_outcome().value.type(),
            f1ap_elem_procs_o::successful_outcome_c::types_opts::options::f1_setup_resp);
}

TEST_F(du_processor_test, when_du_served_cells_list_missing_then_f1setup_rejected)
{
  // Generate F1SetupRequest with missing du served cells list
  f1ap_message f1_setup_req                                                              = generate_f1_setup_request();
  f1_setup_req.pdu.init_msg().value.f1_setup_request()->gnb_du_served_cells_list_present = false;
  f1_setup_req.pdu.init_msg().value.f1_setup_request()->gnb_du_served_cells_list.clear();

  // Pass message to DU processor
  du_processor_obj->get_du_processor_f1ap_interface().get_f1ap_message_handler().handle_message(f1_setup_req);

  // Check the generated PDU is indeed the F1 Setup failure
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.type(), f1ap_pdu_c::types_opts::options::unsuccessful_outcome);
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.unsuccessful_outcome().value.type(),
            f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail);
}

TEST_F(du_processor_test, when_gnb_du_sys_info_missing_then_f1setup_rejected)
{
  // Generate F1SetupRequest with missing gnb du sys info
  f1ap_message f1_setup_req = generate_f1_setup_request();
  f1_setup_req.pdu.init_msg()
      .value.f1_setup_request()
      ->gnb_du_served_cells_list[0]
      .value()
      .gnb_du_served_cells_item()
      .gnb_du_sys_info_present = false;

  // Pass message to DU processor
  du_processor_obj->get_du_processor_f1ap_interface().get_f1ap_message_handler().handle_message(f1_setup_req);

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
  du_processor_obj->get_du_processor_f1ap_interface().get_f1ap_message_handler().handle_message(f1ap_msg);

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
  du_processor_obj->get_du_processor_f1ap_interface().get_f1ap_message_handler().handle_message(
      generate_f1_setup_request());

  // Generate ue_creation message
  ue_index_t ue_index                 = du_processor_obj->get_du_processor_f1ap_interface().allocate_new_ue_index();
  ue_rrc_context_creation_request req = generate_ue_rrc_context_creation_request(ue_index, rnti_t::MIN_CRNTI, 6576);

  // Pass message to DU processor
  ue_rrc_context_creation_response resp = du_processor_obj->handle_ue_rrc_context_creation_request(req);
  ASSERT_NE(resp.f1ap_rrc_notifier, nullptr);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), 1);
}

TEST_F(du_processor_test, when_cell_id_invalid_then_ue_creation_fails)
{
  // Generate valid F1SetupRequest and pass it to DU processor
  du_processor_obj->get_du_processor_f1ap_interface().get_f1ap_message_handler().handle_message(
      generate_f1_setup_request());

  // Generate ue_creation message
  ue_index_t ue_index                 = du_processor_obj->get_du_processor_f1ap_interface().allocate_new_ue_index();
  ue_rrc_context_creation_request req = generate_ue_rrc_context_creation_request(ue_index, rnti_t::MIN_CRNTI, 1);

  // Pass message to DU processor
  ue_rrc_context_creation_response resp = du_processor_obj->handle_ue_rrc_context_creation_request(req);
  ASSERT_EQ(resp.f1ap_rrc_notifier, nullptr);
}

TEST_F(du_processor_test, when_ue_rrc_context_exists_then_new_ue_rrc_context_not_added)
{
  // Generate valid F1SetupRequest
  du_setup_request f1_setup_request;
  generate_valid_f1_setup_request(f1_setup_request);

  // Pass message to DU processor
  du_processor_obj->get_du_processor_f1ap_interface().get_f1ap_message_handler().handle_message(
      generate_f1_setup_request());

  // Generate ue_creation message
  ue_index_t ue_index                 = du_processor_obj->get_du_processor_f1ap_interface().allocate_new_ue_index();
  ue_rrc_context_creation_request req = generate_ue_rrc_context_creation_request(ue_index, rnti_t::MIN_CRNTI, 6576);

  // Pass message to DU processor
  ue_rrc_context_creation_response resp = du_processor_obj->handle_ue_rrc_context_creation_request(req);
  ASSERT_NE(resp.f1ap_rrc_notifier, nullptr);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), 1);

  // Pass same message to DU processor again
  resp = du_processor_obj->handle_ue_rrc_context_creation_request(req);
  ASSERT_EQ(resp.f1ap_rrc_notifier, nullptr);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), 1);
}

TEST_F(du_processor_test, when_max_nof_ues_exceeded_then_ue_not_added)
{
  // Generate valid F1SetupRequest and pass it to DU processor
  du_processor_obj->get_du_processor_f1ap_interface().get_f1ap_message_handler().handle_message(
      generate_f1_setup_request());

  // Reduce logger loglevel to warning to reduce console output
  srslog::fetch_basic_logger("CU-CP").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("CU-UEMNG").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::warning);

  // Add the maximum number of UEs
  for (unsigned it = 0; it < MAX_NOF_UES_PER_DU; it++) {
    // Generate ue_creation message
    rnti_t     c_rnti                   = to_rnti(it + 1); // 0 is not a valid RNTI
    ue_index_t ue_index                 = du_processor_obj->get_du_processor_f1ap_interface().allocate_new_ue_index();
    ue_rrc_context_creation_request req = generate_ue_rrc_context_creation_request(ue_index, c_rnti, 6576);

    // Pass message to DU processor
    ue_rrc_context_creation_response resp = du_processor_obj->handle_ue_rrc_context_creation_request(req);
    ASSERT_NE(resp.f1ap_rrc_notifier, nullptr);
  }

  // Reset logger loglevel
  srslog::fetch_basic_logger("CU-CP").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("CU-UEMNG").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), MAX_NOF_UES_PER_DU);

  // Try to allocate additional UE index
  ue_index_t ue_index = du_processor_obj->get_du_processor_f1ap_interface().allocate_new_ue_index();
  ASSERT_EQ(ue_index, ue_index_t::invalid);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), MAX_NOF_UES_PER_DU);
}

//////////////////////////////////////////////////////////////////////////////////////
/* UE context release                                                               */
//////////////////////////////////////////////////////////////////////////////////////
TEST_F(du_processor_test, when_ue_context_release_command_received_then_ue_deleted)
{
  // Generate valid F1SetupRequest and pass it to DU processor
  du_processor_obj->get_du_processor_f1ap_interface().get_f1ap_message_handler().handle_message(
      generate_f1_setup_request());

  // Generate ue_creation message
  ue_index_t ue_index                 = du_processor_obj->get_du_processor_f1ap_interface().allocate_new_ue_index();
  ue_rrc_context_creation_request req = generate_ue_rrc_context_creation_request(ue_index, rnti_t::MIN_CRNTI, 6576);

  // Pass message to DU processor
  ue_rrc_context_creation_response resp = du_processor_obj->handle_ue_rrc_context_creation_request(req);
  ASSERT_NE(resp.f1ap_rrc_notifier, nullptr);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), 1);

  // Generate UE context release command message
  cu_cp_ue_context_release_command ue_context_release_command = generate_ue_context_release_command(ue_index);

  // Pass message to DU processor
  t = du_processor_obj->get_du_processor_rrc_ue_interface().handle_ue_context_release_command(
      ue_context_release_command);
  t_launcher.emplace(t);

  ASSERT_TRUE(t.ready());

  ASSERT_EQ(du_processor_obj->get_nof_ues(), 0);
  ASSERT_EQ(ue_task_sched->get_nof_pending_tasks(), 0);
}

TEST_F(du_processor_test, when_valid_ue_creation_request_received_after_ue_was_removed_from_full_ue_db_then_ue_added)
{
  // Generate valid F1SetupRequest and pass it to DU processor
  du_processor_obj->get_du_processor_f1ap_interface().get_f1ap_message_handler().handle_message(
      generate_f1_setup_request());

  // Reduce logger loglevel to warning to reduce console output
  srslog::fetch_basic_logger("CU-CP").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("CU-UEMNG").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("RRC").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("PDCP").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::warning);

  // Add the maximum number of UEs
  for (unsigned it = 0; it < MAX_NOF_UES_PER_DU; it++) {
    // Generate ue_creation message
    rnti_t     c_rnti                   = to_rnti(it + 1); // 0 is not a valid RNTI
    ue_index_t ue_index                 = du_processor_obj->get_du_processor_f1ap_interface().allocate_new_ue_index();
    ue_rrc_context_creation_request req = generate_ue_rrc_context_creation_request(ue_index, c_rnti, 6576);

    // Pass message to DU processor
    ue_rrc_context_creation_response resp = du_processor_obj->handle_ue_rrc_context_creation_request(req);
    ASSERT_NE(resp.f1ap_rrc_notifier, nullptr);

    // create SRB1
    srb_creation_message srb1_msg{};
    srb1_msg.ue_index = ue_index;
    srb1_msg.srb_id   = srb_id_t::srb1;
    srb1_msg.pdcp_cfg = {};
    ue_mng.find_du_ue(ue_index)->get_rrc_ue_srb_notifier().create_srb(srb1_msg);
  }

  // Reset logger loglevel
  srslog::fetch_basic_logger("CU-CP").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("CU-UEMNG").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("RRC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("PDCP").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), MAX_NOF_UES_PER_DU);

  // Generate UE context release command message
  cu_cp_ue_context_release_command ue_context_release_command = generate_ue_context_release_command(ue_index_t::min);

  // Pass message to DU processor
  t = du_processor_obj->get_du_processor_rrc_ue_interface().handle_ue_context_release_command(
      ue_context_release_command);
  t_launcher.emplace(t);

  ASSERT_TRUE(t.ready());
  ASSERT_EQ(du_processor_obj->get_nof_ues(), MAX_NOF_UES_PER_DU - 1);
  ASSERT_EQ(ue_task_sched->get_nof_pending_tasks(), 0);

  // Add one more UE to DU processor
  // Generate ue_creation message
  rnti_t     c_rnti                   = to_rnti(MAX_NOF_UES_PER_DU + 1);
  ue_index_t ue_index                 = du_processor_obj->get_du_processor_f1ap_interface().allocate_new_ue_index();
  ue_rrc_context_creation_request req = generate_ue_rrc_context_creation_request(ue_index, c_rnti, 6576);

  // Pass message to DU processor
  ue_rrc_context_creation_response resp = du_processor_obj->handle_ue_rrc_context_creation_request(req);
  ASSERT_NE(resp.f1ap_rrc_notifier, nullptr);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), MAX_NOF_UES_PER_DU);
}
