/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_processor_test_helpers.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;
using namespace asn1::f1ap;

//////////////////////////////////////////////////////////////////////////////////////
/* F1 setup                                                                         */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the successful f1 setup procedure
TEST_F(du_processor_test, when_valid_f1setup_received_then_f1_setup_response_sent)
{
  // Generate valid F1SetupRequest
  f1_setup_request_message f1_setup_request_msg = generate_valid_f1_setup_request();

  // Pass message to DU processor
  du_processor_obj->handle_f1_setup_request(f1_setup_request_msg);

  // Check response is F1SetupResponse
  ASSERT_EQ(f1c_pdu_notifier.last_f1c_msg.pdu.type(), f1ap_pdu_c::types_opts::options::successful_outcome);
  ASSERT_EQ(f1c_pdu_notifier.last_f1c_msg.pdu.successful_outcome().value.type(),
            f1ap_elem_procs_o::successful_outcome_c::types_opts::options::f1_setup_resp);
}

TEST_F(du_processor_test, when_du_served_cells_list_missing_then_f1setup_rejected)
{
  // Generate F1SetupRequest with missing du served cells list
  f1_setup_request_message f1_setup_request_msg                  = generate_f1_setup_request_base();
  f1_setup_request_msg.request->gnb_du_served_cells_list_present = false;

  // Pass message to DU processor
  du_processor_obj->handle_f1_setup_request(f1_setup_request_msg);

  // Check the generated PDU is indeed the F1 Setup failure
  ASSERT_EQ(f1c_pdu_notifier.last_f1c_msg.pdu.type(), f1ap_pdu_c::types_opts::options::unsuccessful_outcome);
  ASSERT_EQ(f1c_pdu_notifier.last_f1c_msg.pdu.unsuccessful_outcome().value.type(),
            f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail);
}

TEST_F(du_processor_test, when_gnb_du_sys_info_missing_then_f1setup_rejected)
{
  // Generate F1SetupRequest with missing gnb du sys info
  f1_setup_request_message f1_setup_request_msg = generate_valid_f1_setup_request();
  f1_setup_request_msg.request->gnb_du_served_cells_list.value[0]->gnb_du_served_cells_item().gnb_du_sys_info_present =
      false;

  // Pass message to DU processor
  du_processor_obj->handle_f1_setup_request(f1_setup_request_msg);

  // Check the generated PDU is indeed the F1 Setup failure
  ASSERT_EQ(f1c_pdu_notifier.last_f1c_msg.pdu.type(), f1ap_pdu_c::types_opts::options::unsuccessful_outcome);
  ASSERT_EQ(f1c_pdu_notifier.last_f1c_msg.pdu.unsuccessful_outcome().value.type(),
            f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail);
}

TEST_F(du_processor_test, when_max_nof_du_cells_exeeded_then_f1setup_rejected)
{
  // Generate F1SetupRequest with too many cells
  f1_setup_request_message f1_setup_request_msg                  = generate_f1_setup_request_base();
  f1_setup_request_msg.request->gnb_du_served_cells_list_present = true;
  f1_setup_request_msg.request->gnb_du_served_cells_list.id      = ASN1_F1AP_ID_GNB_DU_SERVED_CELLS_LIST;
  f1_setup_request_msg.request->gnb_du_served_cells_list.crit    = asn1::crit_opts::reject;

  for (int du_cell_idx_int = MIN_DU_CELL_INDEX; du_cell_idx_int < MAX_NOF_DU_CELLS + 1; du_cell_idx_int++) {
    f1_setup_request_msg.request->gnb_du_served_cells_list.value.push_back({});
    f1_setup_request_msg.request->gnb_du_served_cells_list.value.back().load_info_obj(
        ASN1_F1AP_ID_GNB_DU_SERVED_CELLS_ITEM);
    f1_setup_request_msg.request->gnb_du_served_cells_list.value.back()->gnb_du_served_cells_item() =
        generate_served_cells_item(du_cell_idx_int, du_cell_idx_int);
  }

  // Pass message to DU processor
  du_processor_obj->handle_f1_setup_request(f1_setup_request_msg);

  // Check the generated PDU is indeed the F1 Setup failure
  ASSERT_EQ(f1c_pdu_notifier.last_f1c_msg.pdu.type(), f1ap_pdu_c::types_opts::options::unsuccessful_outcome);
  ASSERT_EQ(f1c_pdu_notifier.last_f1c_msg.pdu.unsuccessful_outcome().value.type(),
            f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail);
}

//////////////////////////////////////////////////////////////////////////////////////
/* UE creation                                                                      */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(du_processor_test, when_ue_creation_msg_valid_then_ue_added)
{
  // Generate valid F1SetupRequest
  f1_setup_request_message f1_setup_request_msg = generate_valid_f1_setup_request();

  // Pass message to DU processor
  du_processor_obj->handle_f1_setup_request(f1_setup_request_msg);

  // Generate ue_creation message
  ue_creation_message ue_creation_msg = generate_ue_creation_message(MIN_CRNTI, 12345678);

  // Pass message to DU processor
  ue_creation_complete_message ue_creation_complete_msg = du_processor_obj->handle_ue_creation_request(ue_creation_msg);
  ASSERT_NE(ue_creation_complete_msg.ue_index, ue_index_t::invalid);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), 1);
}

TEST_F(du_processor_test, when_cell_id_invalid_then_ue_not_added)
{
  // Generate valid F1SetupRequest
  f1_setup_request_message f1_setup_request_msg = generate_valid_f1_setup_request();

  // Pass message to DU processor
  du_processor_obj->handle_f1_setup_request(f1_setup_request_msg);

  // Generate ue_creation message
  ue_creation_message ue_creation_msg = generate_ue_creation_message(MIN_CRNTI, 1);

  // Pass message to DU processor
  ue_creation_complete_message ue_creation_complete_msg = du_processor_obj->handle_ue_creation_request(ue_creation_msg);
  ASSERT_EQ(ue_creation_complete_msg.ue_index, ue_index_t::invalid);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), 0);
}

TEST_F(du_processor_test, when_rnti_invalid_then_ue_not_added)
{
  // Generate valid F1SetupRequest
  f1_setup_request_message f1_setup_request_msg = generate_valid_f1_setup_request();

  // Pass message to DU processor
  du_processor_obj->handle_f1_setup_request(f1_setup_request_msg);

  // Generate ue_creation message
  ue_creation_message ue_creation_msg = generate_ue_creation_message(INVALID_RNTI, 12345678);

  // Pass message to DU processor
  ue_creation_complete_message ue_creation_complete_msg = du_processor_obj->handle_ue_creation_request(ue_creation_msg);
  ASSERT_EQ(ue_creation_complete_msg.ue_index, ue_index_t::invalid);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), 0);
}

TEST_F(du_processor_test, when_ue_exists_then_ue_not_added)
{
  // Generate valid F1SetupRequest
  f1_setup_request_message f1_setup_request_msg = generate_valid_f1_setup_request();

  // Pass message to DU processor
  du_processor_obj->handle_f1_setup_request(f1_setup_request_msg);

  // Generate ue_creation message
  ue_creation_message ue_creation_msg = generate_ue_creation_message(MIN_CRNTI, 12345678);

  // Pass message to DU processor
  ue_creation_complete_message ue_creation_complete_msg = du_processor_obj->handle_ue_creation_request(ue_creation_msg);
  ASSERT_NE(ue_creation_complete_msg.ue_index, ue_index_t::invalid);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), 1);

  // Pass same message to DU processor again
  ue_creation_complete_msg = du_processor_obj->handle_ue_creation_request(ue_creation_msg);
  ASSERT_EQ(ue_creation_complete_msg.ue_index, ue_index_t::invalid);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), 1);
}

TEST_F(du_processor_test, when_max_nof_ues_exceeded_then_ue_not_added)
{
  // Generate valid F1SetupRequest
  f1_setup_request_message f1_setup_request_msg = generate_valid_f1_setup_request();

  // Pass message to DU processor
  du_processor_obj->handle_f1_setup_request(f1_setup_request_msg);

  // Reduce logger loglevel to warning to reduce console output
  srslog::fetch_basic_logger("CU-CP").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("UE-MNG").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::warning);

  // Add the maximum number of UEs
  for (unsigned ue_index = 0; ue_index < MAX_NOF_UES_PER_DU; ue_index++) {
    // Generate ue_creation message
    rnti_t              c_rnti          = to_rnti(ue_index + 1); // 0 is not a valid RNTI
    ue_creation_message ue_creation_msg = generate_ue_creation_message(c_rnti, 12345678);

    // Pass message to DU processor
    ue_creation_complete_message ue_creation_complete_msg =
        du_processor_obj->handle_ue_creation_request(ue_creation_msg);
    ASSERT_NE(ue_creation_complete_msg.ue_index, ue_index_t::invalid);
  }

  // Reset logger loglevel
  srslog::fetch_basic_logger("CU-CP").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("UE-MNG").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), MAX_NOF_UES_PER_DU);

  // Add one more UE to DU processor
  // Generate ue_creation message
  rnti_t              c_rnti          = to_rnti(MAX_NOF_UES_PER_DU + 1);
  ue_creation_message ue_creation_msg = generate_ue_creation_message(c_rnti, 12345678);

  // Pass message to DU processor
  ue_creation_complete_message ue_creation_complete_msg = du_processor_obj->handle_ue_creation_request(ue_creation_msg);
  ASSERT_EQ(ue_creation_complete_msg.ue_index, ue_index_t::invalid);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), MAX_NOF_UES_PER_DU);
}

//////////////////////////////////////////////////////////////////////////////////////
/* UE context release                                                               */
//////////////////////////////////////////////////////////////////////////////////////
TEST_F(du_processor_test, when_ue_context_release_command_received_then_ue_deleted)
{
  // Generate valid F1SetupRequest
  f1_setup_request_message f1_setup_request_msg = generate_valid_f1_setup_request();

  // Pass message to DU processor
  du_processor_obj->handle_f1_setup_request(f1_setup_request_msg);

  // Generate ue_creation message
  ue_creation_message ue_creation_msg = generate_ue_creation_message(MIN_CRNTI, 12345678);

  // Pass message to DU processor
  ue_creation_complete_message ue_creation_complete_msg = du_processor_obj->handle_ue_creation_request(ue_creation_msg);
  ASSERT_NE(ue_creation_complete_msg.ue_index, ue_index_t::invalid);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), 1);

  // Generate UE context release command message
  cu_cp_ue_context_release_command ue_context_release_command =
      generate_ue_context_release_command(uint_to_ue_index(0));

  // Pass message to DU processor
  du_processor_obj->handle_ue_context_release_command(ue_context_release_command);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), 0);
}

TEST_F(du_processor_test, when_valid_ue_creation_request_received_after_ue_was_removed_from_full_ue_db_then_ue_added)
{
  // Generate valid F1SetupRequest
  f1_setup_request_message f1_setup_request_msg = generate_valid_f1_setup_request();

  // Pass message to DU processor
  du_processor_obj->handle_f1_setup_request(f1_setup_request_msg);

  // Reduce logger loglevel to warning to reduce console output
  srslog::fetch_basic_logger("CU-CP").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("UE-MNG").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::warning);

  // Add the maximum number of UEs
  for (unsigned ue_index = 0; ue_index < MAX_NOF_UES_PER_DU; ue_index++) {
    // Generate ue_creation message
    rnti_t              c_rnti          = to_rnti(ue_index + 1); // 0 is not a valid RNTI
    ue_creation_message ue_creation_msg = generate_ue_creation_message(c_rnti, 12345678);

    // Pass message to DU processor
    ue_creation_complete_message ue_creation_complete_msg =
        du_processor_obj->handle_ue_creation_request(ue_creation_msg);
    ASSERT_NE(ue_creation_complete_msg.ue_index, ue_index_t::invalid);
  }

  // Reset logger loglevel
  srslog::fetch_basic_logger("CU-CP").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("UE-MNG").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), MAX_NOF_UES_PER_DU);

  // Generate UE context release command message
  cu_cp_ue_context_release_command ue_context_release_command =
      generate_ue_context_release_command(uint_to_ue_index(0));

  // Pass message to DU processor
  du_processor_obj->handle_ue_context_release_command(ue_context_release_command);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), MAX_NOF_UES_PER_DU - 1);

  // Add one more UE to DU processor
  // Generate ue_creation message
  rnti_t              c_rnti          = to_rnti(MAX_NOF_UES_PER_DU + 1);
  ue_creation_message ue_creation_msg = generate_ue_creation_message(c_rnti, 12345678);

  // Pass message to DU processor
  ue_creation_complete_message ue_creation_complete_msg = du_processor_obj->handle_ue_creation_request(ue_creation_msg);
  ASSERT_NE(ue_creation_complete_msg.ue_index, ue_index_t::invalid);

  ASSERT_EQ(du_processor_obj->get_nof_ues(), MAX_NOF_UES_PER_DU);
}
