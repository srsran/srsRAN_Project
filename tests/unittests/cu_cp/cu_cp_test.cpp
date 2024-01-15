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

#include "../rrc/rrc_ue_test_messages.h"
#include "cu_cp_test_helpers.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/ngap/ngap_types.h"
#include "srsran/ran/cu_types.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::f1ap;

//////////////////////////////////////////////////////////////////////////////////////
/* Initial CU-CP routine manager with connected CU-UPs                              */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the initial cu-cp routine
TEST_F(cu_cp_test, when_new_cu_ups_connected_then_cu_up_e1_setup_request_received)
{
  // Connect CU-UP (note that this creates a CU-UP processor, but the CU-UP is only connected after the E1Setup
  // procedure)
  e1ap_gw.request_new_cu_up_connection();

  // // Inject E1SetupRequest
  e1ap_message e1_setup_request = generate_valid_cu_up_e1_setup_request();
  cu_cp_obj->get_cu_cp_cu_up_connection_interface()
      .get_e1ap_message_handler(uint_to_cu_up_index(0))
      .handle_message(e1_setup_request);

  // check that CU-UP has been added
  ASSERT_EQ(cu_cp_obj->get_nof_cu_ups(), 1U);

  // check that the E1 Setup Response was sent
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome);
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.successful_outcome().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::gnb_cu_up_e1_setup_resp);
}

//////////////////////////////////////////////////////////////////////////////////////
/* DU connection handling                                                           */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the DU connection
TEST_F(cu_cp_test, when_new_du_connection_then_du_added)
{
  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure)
  this->f1c_gw.request_new_du_connection();

  // check that DU has been added
  ASSERT_EQ(cu_cp_obj->get_connected_dus().get_nof_dus(), 1U);
}

/// Test the DU removal
TEST_F(cu_cp_test, when_du_remove_request_received_then_du_removed)
{
  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure)
  this->f1c_gw.request_new_du_connection();

  // Remove DU
  this->f1c_gw.remove_du_connection(0);

  // Check that DU has been removed
  ASSERT_EQ(cu_cp_obj->get_connected_dus().get_nof_dus(), 0U);
}

/// Test exeeding the maximum number of connected DUs
TEST_F(cu_cp_test, when_max_nof_dus_connected_then_reject_new_connection)
{
  for (int it = 0; it < MAX_NOF_DUS; it++) {
    this->f1c_gw.request_new_du_connection();
  }

  // Check that MAX_NOF_DUS are connected
  ASSERT_EQ(cu_cp_obj->get_connected_dus().get_nof_dus(), MAX_NOF_DUS);

  // Attempting connection beyond CU-CP MAX_NOF_DUS limit.
  this->f1c_gw.request_new_du_connection();

  // Check that MAX_NOF_DUS are connected
  ASSERT_EQ(cu_cp_obj->get_connected_dus().get_nof_dus(), MAX_NOF_DUS);
}

//////////////////////////////////////////////////////////////////////////////////////
/* CU-UP connection handling                                                        */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the CU-UP connection
TEST_F(cu_cp_test, when_new_cu_up_connection_then_cu_up_added)
{
  // Connect CU-UP
  this->e1ap_gw.request_new_cu_up_connection();

  // check that CU-UP has been added
  ASSERT_EQ(cu_cp_obj->get_nof_cu_ups(), 1U);
}

/// Test the CU-UP removal
TEST_F(cu_cp_test, when_cu_up_remove_request_received_then_cu_up_removed)
{
  // Connect CU-UP
  this->e1ap_gw.request_new_cu_up_connection();

  // Check that CU-UP has been added
  ASSERT_EQ(cu_cp_obj->get_nof_cu_ups(), 1U);

  // Remove CU-UP
  this->e1ap_gw.remove_cu_up_connection(0);

  // Check that CU-UP has been removed
  ASSERT_EQ(cu_cp_obj->get_nof_cu_ups(), 0U);
}

/// Test exeeding the maximum number of connected CU-UPs
TEST_F(cu_cp_test, when_max_nof_cu_ups_connected_then_reject_new_connection)
{
  for (int it = cu_up_index_to_uint(cu_up_index_t::min); it < MAX_NOF_CU_UPS; it++) {
    this->e1ap_gw.request_new_cu_up_connection();
  }

  // Check that MAX_NOF_CU_UPS are connected
  ASSERT_EQ(cu_cp_obj->get_nof_cu_ups(), MAX_NOF_CU_UPS);

  this->e1ap_gw.request_new_cu_up_connection();

  // Check that MAX_NOF_CU_UPS are connected
  ASSERT_EQ(cu_cp_obj->get_nof_cu_ups(), MAX_NOF_CU_UPS);
}

//////////////////////////////////////////////////////////////////////////////////////
/* AMF connection handling                                                          */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(cu_cp_test, when_ng_setup_response_received_then_amf_connected)
{
  // Connect AMF by injecting a ng_setup_response
  ngap_message ngap_msg = generate_ng_setup_response();
  cu_cp_obj->get_ngap_message_handler().handle_message(ngap_msg);

  ASSERT_TRUE(cu_cp_obj->amf_is_connected());
}

TEST_F(cu_cp_test, when_amf_connected_then_ue_added)
{
  // Connect AMF by injecting a ng_setup_response
  ngap_message ngap_msg = generate_ng_setup_response();
  cu_cp_obj->get_ngap_message_handler().handle_message(ngap_msg);

  ASSERT_TRUE(cu_cp_obj->amf_is_connected());

  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure)
  this->f1c_gw.request_new_du_connection();
  // Connect CU-UP
  this->e1ap_gw.request_new_cu_up_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = generate_f1_setup_request();

  du_index_t du_index = uint_to_du_index(0);

  // Pass message to CU-CP
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(f1setup_msg);

  // Attach UE
  {
    gnb_cu_ue_f1ap_id_t cu_ue_id = int_to_gnb_cu_ue_f1ap_id(0);
    gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(41255);
    rnti_t              crnti    = to_rnti(0x4601);

    attach_ue(du_ue_id, cu_ue_id, crnti, du_index);
  }

  // check that UE has been added
  ASSERT_EQ(cu_cp_obj->get_connected_dus().get_nof_ues(), 1U);

  // check that the Initial UE Message was sent to the AMF
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.type(), asn1::ngap::ngap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::init_ue_msg);
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.init_msg().value.init_ue_msg()->ran_ue_ngap_id, 0);
}

TEST_F(cu_cp_test, when_amf_not_connected_then_ue_rejected)
{
  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure)
  this->f1c_gw.request_new_du_connection();
  // Connect CU-UP
  this->e1ap_gw.request_new_cu_up_connection();
  // Disconnect CU-CP from AMF.
  cu_cp_obj->handle_amf_connection_drop();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = generate_f1_setup_request();

  du_index_t du_index = uint_to_du_index(0);

  // Pass message to CU-CP
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(f1setup_msg);

  gnb_cu_ue_f1ap_id_t cu_ue_id = int_to_gnb_cu_ue_f1ap_id(0);
  gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(41255);
  rnti_t              crnti    = to_rnti(0x4601);

  // Inject Initial UL RRC message
  f1ap_message init_ul_rrc_msg = generate_init_ul_rrc_message_transfer(du_ue_id, crnti);
  test_logger.info("Injecting Initial UL RRC message");
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(init_ul_rrc_msg);

  // Inject UE Context Release Complete message
  f1ap_message ue_context_release_complete_msg = generate_ue_context_release_complete(cu_ue_id, du_ue_id);
  test_logger.info("Injecting UE Context Release Complete message");
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(
      ue_context_release_complete_msg);

  // check that UE has not been added
  ASSERT_EQ(cu_cp_obj->get_connected_dus().get_nof_ues(), 0U);

  // check that the Initial UE Message was not send to the AMF
  ASSERT_NE(ngap_amf_notifier.last_ngap_msgs.back().pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::init_ue_msg);
}

/// Test the f1 initial UL RRC message transfer procedure
TEST_F(cu_cp_test, when_amf_connection_drop_then_reject_ue)
{
  // Connect AMF by injecting a ng_setup_response
  ngap_message ngap_msg = generate_ng_setup_response();
  cu_cp_obj->get_ngap_message_handler().handle_message(ngap_msg);

  ASSERT_TRUE(cu_cp_obj->amf_is_connected());

  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure)
  this->f1c_gw.request_new_du_connection();
  // Connect CU-UP
  this->e1ap_gw.request_new_cu_up_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = generate_f1_setup_request();

  du_index_t du_index = uint_to_du_index(0);

  // Pass message to CU-CP
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(f1setup_msg);

  // Attach first UE (successful)
  {
    gnb_cu_ue_f1ap_id_t first_ue_cu_ue_id = int_to_gnb_cu_ue_f1ap_id(0);
    gnb_du_ue_f1ap_id_t first_ue_du_ue_id = int_to_gnb_du_ue_f1ap_id(41255);
    rnti_t              crnti             = to_rnti(0x4601);

    attach_ue(first_ue_du_ue_id, first_ue_cu_ue_id, crnti, du_index);
  }

  // check that UE has been added
  ASSERT_EQ(cu_cp_obj->get_connected_dus().get_nof_ues(), 1U);

  // Disconnect AMF
  cu_cp_obj->handle_amf_connection_drop();

  ASSERT_FALSE(cu_cp_obj->amf_is_connected());

  // Attach second UE (failure)
  {
    gnb_cu_ue_f1ap_id_t second_ue_cu_ue_id = int_to_gnb_cu_ue_f1ap_id(1);
    gnb_du_ue_f1ap_id_t second_ue_du_ue_id = int_to_gnb_du_ue_f1ap_id(41256);
    rnti_t              crnti              = to_rnti(0x4602);

    // Inject Initial UL RRC message
    f1ap_message init_ul_rrc_msg = generate_init_ul_rrc_message_transfer(second_ue_du_ue_id, crnti);
    test_logger.info("Injecting Initial UL RRC message");
    cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(init_ul_rrc_msg);

    // Inject UE Context Release Complete message
    f1ap_message ue_context_release_complete_msg =
        generate_ue_context_release_complete(second_ue_cu_ue_id, second_ue_du_ue_id);
    test_logger.info("Injecting UE Context Release Complete message");
    cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(
        ue_context_release_complete_msg);
  }

  // The UE should not exists in the CU-CP
  ASSERT_EQ(cu_cp_obj->get_connected_dus().get_nof_ues(), 1U);

  // Check that UE has also been removed from F1AP
  ASSERT_EQ(cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_statistics_handler().get_nof_ues(), 1);
}

//////////////////////////////////////////////////////////////////////////////////////
/* Paging handling                                                                  */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the handling of an paging message when a DU is not connected
TEST_F(cu_cp_test, when_du_connection_not_finished_then_paging_is_not_sent_to_du)
{
  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure)
  this->f1c_gw.request_new_du_connection();

  // Generate Paging
  ngap_message paging_msg = generate_valid_minimal_paging_message();

  cu_cp_obj->get_ngap_message_handler().handle_message(paging_msg);

  ASSERT_FALSE(check_minimal_paging_result());
}

/// Test the handling of a valid Paging message with only mandatory values set
TEST_F(cu_cp_test, when_valid_paging_message_received_then_paging_is_sent_to_du)
{
  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure)
  this->f1c_gw.request_new_du_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = generate_f1_setup_request();
  // Pass message to CU-CP
  cu_cp_obj->get_connected_dus().get_du(uint_to_du_index(0)).get_f1ap_message_handler().handle_message(f1setup_msg);

  // Generate Paging
  ngap_message paging_msg = generate_valid_minimal_paging_message();

  cu_cp_obj->get_ngap_message_handler().handle_message(paging_msg);

  ASSERT_TRUE(check_minimal_paging_result());
}

/// Test the handling of a valid Paging message with optional values set
TEST_F(cu_cp_test, when_valid_paging_message_with_optional_values_received_then_paging_is_sent_to_du)
{
  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure)
  this->f1c_gw.request_new_du_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = generate_f1_setup_request();
  // Pass message to CU-CP
  cu_cp_obj->get_connected_dus().get_du(uint_to_du_index(0)).get_f1ap_message_handler().handle_message(f1setup_msg);

  // Generate Paging
  ngap_message paging_msg = generate_valid_paging_message();

  cu_cp_obj->get_ngap_message_handler().handle_message(paging_msg);

  ASSERT_TRUE(check_paging_result());
}

/// Test the handling of an invalid Paging message
TEST_F(cu_cp_test, when_no_du_for_tac_exists_then_paging_is_not_sent_to_du)
{
  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure)
  this->f1c_gw.request_new_du_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = generate_f1_setup_request();
  // Pass message to CU-CP
  cu_cp_obj->get_connected_dus().get_du(uint_to_du_index(0)).get_f1ap_message_handler().handle_message(f1setup_msg);

  // Generate Paging with unknown tac
  ngap_message paging_msg = generate_valid_minimal_paging_message();
  paging_msg.pdu.init_msg().value.paging()->tai_list_for_paging[0].tai.tac.from_number(8);

  cu_cp_obj->get_ngap_message_handler().handle_message(paging_msg);

  ASSERT_FALSE(check_minimal_paging_result());
}

/// Test the handling of an invalid Paging message
TEST_F(cu_cp_test, when_assist_data_for_paging_for_unknown_tac_is_included_then_paging_is_not_sent_to_du)
{
  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure)
  this->f1c_gw.request_new_du_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = generate_f1_setup_request();
  // Pass message to CU-CP
  cu_cp_obj->get_connected_dus().get_du(uint_to_du_index(0)).get_f1ap_message_handler().handle_message(f1setup_msg);

  // Generate Paging with unknown tac but assist data for paging
  ngap_message paging_msg = generate_valid_paging_message();
  paging_msg.pdu.init_msg().value.paging()->tai_list_for_paging[0].tai.tac.from_number(8);

  cu_cp_obj->get_ngap_message_handler().handle_message(paging_msg);

  ASSERT_FALSE(check_paging_result());
}

/// Test the handling of an invalid Paging message
TEST_F(cu_cp_test, when_invalid_paging_message_received_then_paging_is_not_sent_to_du)
{
  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure)
  this->f1c_gw.request_new_du_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = generate_f1_setup_request();
  // Pass message to CU-CP
  cu_cp_obj->get_connected_dus().get_du(uint_to_du_index(0)).get_f1ap_message_handler().handle_message(f1setup_msg);

  // Generate Paging
  ngap_message paging_msg = generate_invalid_paging_message();

  cu_cp_obj->get_ngap_message_handler().handle_message(paging_msg);

  ASSERT_FALSE(check_paging_result());
}

//////////////////////////////////////////////////////////////////////////////////////
/* Inactivity Notification                                                          */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the handling of a ue level inactivity notification
TEST_F(cu_cp_test, when_ue_level_inactivity_message_received_then_ue_context_release_request_is_sent)
{
  // Test preamble
  du_index_t          du_index  = uint_to_du_index(0);
  gnb_cu_ue_f1ap_id_t cu_ue_id  = int_to_gnb_cu_ue_f1ap_id(0);
  gnb_du_ue_f1ap_id_t du_ue_id  = int_to_gnb_du_ue_f1ap_id(0);
  pci_t               pci       = 0;
  rnti_t              crnti     = to_rnti(0x4601);
  amf_ue_id_t         amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));
  ran_ue_id_t ran_ue_id = uint_to_ran_ue_id(0);
  test_preamble_ue_creation(du_index, du_ue_id, cu_ue_id, pci, crnti, amf_ue_id, ran_ue_id);

  cu_cp_inactivity_notification inactivity_notification;
  inactivity_notification.ue_index    = uint_to_ue_index(0);
  inactivity_notification.ue_inactive = true;

  cu_cp_obj->handle_bearer_context_inactivity_notification(inactivity_notification);

  // check that the UE Context Release Request was sent to the AMF
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.type(), asn1::ngap::ngap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_request);
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.init_msg().value.ue_context_release_request()->cause.type(),
            asn1::ngap::cause_c::types::radio_network);
}

/// Test the handling of an inactivity notification with unsupported activity level
TEST_F(cu_cp_test, when_unsupported_inactivity_message_received_then_ue_context_release_request_is_not_sent)
{
  // Test preamble
  du_index_t          du_index  = uint_to_du_index(0);
  gnb_cu_ue_f1ap_id_t cu_ue_id  = int_to_gnb_cu_ue_f1ap_id(0);
  gnb_du_ue_f1ap_id_t du_ue_id  = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti     = to_rnti(0x4601);
  pci_t               pci       = 0;
  amf_ue_id_t         amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));
  ran_ue_id_t ran_ue_id = uint_to_ran_ue_id(0);
  test_preamble_ue_creation(du_index, du_ue_id, cu_ue_id, pci, crnti, amf_ue_id, ran_ue_id);

  cu_cp_inactivity_notification inactivity_notification;
  inactivity_notification.ue_index    = uint_to_ue_index(0);
  inactivity_notification.ue_inactive = false;

  cu_cp_obj->handle_bearer_context_inactivity_notification(inactivity_notification);

  // check that the UE Context Release Request was not sent to the AMF
  ASSERT_NE(ngap_amf_notifier.last_ngap_msgs.back().pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_request);
}

//////////////////////////////////////////////////////////////////////////////////////
/* AMF initiated PDU Session Release                                                */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(cu_cp_test, when_pdu_session_resource_release_command_received_then_release_command_is_sent_to_cu_up_first)
{
  // Test preamble
  du_index_t          du_index  = uint_to_du_index(0);
  gnb_cu_ue_f1ap_id_t cu_ue_id  = int_to_gnb_cu_ue_f1ap_id(0);
  gnb_du_ue_f1ap_id_t du_ue_id  = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti     = to_rnti(0x4601);
  pci_t               pci       = 1;
  amf_ue_id_t         amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));
  ran_ue_id_t            ran_ue_id        = uint_to_ran_ue_id(0);
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = int_to_gnb_cu_cp_ue_e1ap_id(0);
  gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id = int_to_gnb_cu_up_ue_e1ap_id(0);

  test_preamble_ue_full_attach(
      du_index, du_ue_id, cu_ue_id, pci, crnti, amf_ue_id, ran_ue_id, cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);

  // Inject PduSessionResourceReleaseCommand
  cu_cp_obj->get_ngap_message_handler().handle_message(
      generate_valid_pdu_session_resource_release_command(amf_ue_id, ran_ue_id, uint_to_pdu_session_id(1)));

  // Check that the Bearer Context Release Command was sent to the CU-UP first
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_release_cmd);
}

//////////////////////////////////////////////////////////////////////////////////////
/* AMF initiated UE Context Release                                                 */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(cu_cp_test, when_release_command_received_then_release_command_is_sent_to_du)
{
  // Test preamble
  du_index_t          du_index  = uint_to_du_index(0);
  gnb_cu_ue_f1ap_id_t cu_ue_id  = int_to_gnb_cu_ue_f1ap_id(0);
  gnb_du_ue_f1ap_id_t du_ue_id  = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti     = to_rnti(0x4601);
  pci_t               pci       = 0;
  amf_ue_id_t         amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));
  ran_ue_id_t ran_ue_id = uint_to_ran_ue_id(0);
  test_preamble_ue_creation(du_index, du_ue_id, cu_ue_id, pci, crnti, amf_ue_id, ran_ue_id);

  // Inject UE Context Release Command
  cu_cp_obj->get_ngap_message_handler().handle_message(
      generate_valid_ue_context_release_command_with_amf_ue_ngap_id(amf_ue_id));

  // check that the UE Context Release Command with RRC Container was sent to the DU
  span<const f1ap_message> last_f1ap_msgs = f1c_gw.last_tx_pdus(0);
  ASSERT_FALSE(last_f1ap_msgs.empty());
  ASSERT_EQ(last_f1ap_msgs.back().pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(last_f1ap_msgs.back().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_release_cmd);
  ASSERT_TRUE(last_f1ap_msgs.back().pdu.init_msg().value.ue_context_release_cmd()->rrc_container_present);
  // check that the SRB ID is set if the RRC Container is included
  ASSERT_TRUE(last_f1ap_msgs.back().pdu.init_msg().value.ue_context_release_cmd()->srb_id_present);
}

TEST_F(cu_cp_test,
       when_when_pdu_session_resource_setup_request_is_received_during_release_then_error_indication_is_sent)
{
  // Test preamble
  du_index_t          du_index  = uint_to_du_index(0);
  gnb_cu_ue_f1ap_id_t cu_ue_id  = int_to_gnb_cu_ue_f1ap_id(0);
  gnb_du_ue_f1ap_id_t du_ue_id  = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti     = to_rnti(0x4601);
  pci_t               pci       = 0;
  amf_ue_id_t         amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));
  ran_ue_id_t ran_ue_id = uint_to_ran_ue_id(0);
  test_preamble_ue_creation(du_index, du_ue_id, cu_ue_id, pci, crnti, amf_ue_id, ran_ue_id);

  // Inject UE Context Release Command
  cu_cp_obj->get_ngap_message_handler().handle_message(
      generate_valid_ue_context_release_command_with_amf_ue_ngap_id(amf_ue_id));

  // check that the UE Context Release Command with RRC Container was sent to the DU
  span<const f1ap_message> last_f1ap_msgs = f1c_gw.last_tx_pdus(0);
  ASSERT_FALSE(last_f1ap_msgs.empty());
  ASSERT_EQ(last_f1ap_msgs.back().pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(last_f1ap_msgs.back().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_release_cmd);
  ASSERT_TRUE(last_f1ap_msgs.back().pdu.init_msg().value.ue_context_release_cmd()->rrc_container_present);
  // check that the SRB ID is set if the RRC Container is included
  ASSERT_TRUE(last_f1ap_msgs.back().pdu.init_msg().value.ue_context_release_cmd()->srb_id_present);

  // Inject PDU Session Resource Setup Request
  cu_cp_obj->get_ngap_message_handler().handle_message(
      generate_valid_pdu_session_resource_setup_request_message(amf_ue_id, ran_ue_id, uint_to_pdu_session_id(1)));

  // Inject F1AP UE Context Release Complete
  cu_cp_obj->get_connected_dus()
      .get_du(uint_to_du_index(0))
      .get_f1ap_message_handler()
      .handle_message(generate_ue_context_release_complete(cu_ue_id, du_ue_id));

  // check that the ErrorIndication was sent to the AMF
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::error_ind);
}

//////////////////////////////////////////////////////////////////////////////////////
/* DU Initiated UE Context Release                                                  */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the handling of a DU initiated release request
TEST_F(cu_cp_test, when_du_initiated_ue_context_release_received_then_ue_context_release_request_is_sent)
{
  // Test preamble
  du_index_t          du_index  = uint_to_du_index(0);
  gnb_cu_ue_f1ap_id_t cu_ue_id  = int_to_gnb_cu_ue_f1ap_id(0);
  gnb_du_ue_f1ap_id_t du_ue_id  = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti     = to_rnti(0x4601);
  pci_t               pci       = 0;
  amf_ue_id_t         amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));
  ran_ue_id_t ran_ue_id = uint_to_ran_ue_id(0);
  test_preamble_ue_creation(du_index, du_ue_id, cu_ue_id, pci, crnti, amf_ue_id, ran_ue_id);

  // Inject UE Context Release Request
  cu_cp_obj->get_connected_dus()
      .get_du(uint_to_du_index(0))
      .get_f1ap_message_handler()
      .handle_message(generate_ue_context_release_request(cu_ue_id, du_ue_id));

  // Check that the UE Context Release Request was sent to the AMF
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.type(), asn1::ngap::ngap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_request);
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.init_msg().value.ue_context_release_request()->cause.type(),
            asn1::ngap::cause_c::types_opts::options::radio_network);
}

/// Test the handling of a DU initiated release request
TEST_F(
    cu_cp_test,
    when_du_initiated_ue_context_release_received_and_ue_unknown_to_amf_then_ue_is_released_without_release_request_to_amf)
{
  // Test preamble
  du_index_t          du_index = uint_to_du_index(0);
  gnb_cu_ue_f1ap_id_t cu_ue_id = int_to_gnb_cu_ue_f1ap_id(0);
  gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti    = to_rnti(0x4601);
  pci_t               pci      = 0;

  // Connect AMF, DU, CU-UP
  test_preamble_all_connected(du_index, pci);

  // Attach UE
  attach_ue(du_ue_id, cu_ue_id, crnti, du_index);
  ASSERT_EQ(cu_cp_obj->get_connected_dus().get_nof_ues(), 1U);

  // Inject UE Context Release Request
  cu_cp_obj->get_connected_dus()
      .get_du(uint_to_du_index(0))
      .get_f1ap_message_handler()
      .handle_message(generate_ue_context_release_request(cu_ue_id, du_ue_id));

  // Check that the UE Context Release Request was not sent to the AMF
  ASSERT_NE(ngap_amf_notifier.last_ngap_msgs.back().pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_request);

  // Check that the Ue Context Release Command was sent to the DU
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_release_cmd);

  // Inject UE Context Release Complete
  cu_cp_obj->get_connected_dus()
      .get_du(uint_to_du_index(0))
      .get_f1ap_message_handler()
      .handle_message(generate_ue_context_release_complete(cu_ue_id, du_ue_id));

  // Check that the UE was removed
  ASSERT_EQ(cu_cp_obj->get_connected_dus().get_nof_ues(), 0);
}

//////////////////////////////////////////////////////////////////////////////////////
/* Reestablishment handling                                                         */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(cu_cp_test, when_reestablishment_fails_then_ue_released)
{
  // Test preamble
  du_index_t          du_index  = uint_to_du_index(0);
  gnb_cu_ue_f1ap_id_t cu_ue_id  = int_to_gnb_cu_ue_f1ap_id(0);
  gnb_du_ue_f1ap_id_t du_ue_id  = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti     = to_rnti(0x4601);
  pci_t               pci       = 0;
  amf_ue_id_t         amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));
  ran_ue_id_t ran_ue_id = uint_to_ran_ue_id(0);
  test_preamble_ue_creation(du_index, du_ue_id, cu_ue_id, pci, crnti, amf_ue_id, ran_ue_id);

  // Attach second UE with RRC Reestablishment Request
  {
    gnb_cu_ue_f1ap_id_t cu_ue_id_2 = int_to_gnb_cu_ue_f1ap_id(1);
    gnb_du_ue_f1ap_id_t du_ue_id_2 = int_to_gnb_du_ue_f1ap_id(1);
    rnti_t              crnti_2    = to_rnti(0x4602);

    // Create Initial UL RRC message
    f1ap_message init_ul_rrc_msg = generate_init_ul_rrc_message_transfer(du_ue_id_2, crnti_2);

    // Add invalid RRC Reestablishment Request to Initial UL RRC message
    init_ul_rrc_msg.pdu.init_msg().value.init_ul_rrc_msg_transfer()->rrc_container =
        generate_invalid_rrc_reestablishment_request_pdu(pci, crnti);

    // Inject Initial UL RRC message
    test_logger.info("Injecting Initial UL RRC message");
    cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(init_ul_rrc_msg);

    // Inject UL RRC message containing RRC Setup Complete
    f1ap_message ul_rrc_msg =
        generate_ul_rrc_message_transfer(cu_ue_id_2, du_ue_id_2, srb_id_t::srb1, generate_rrc_setup_complete());
    test_logger.info("Injecting UL RRC message (RRC Setup Complete)");
    cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(ul_rrc_msg);

    // check that the UE Context Release Request was sent to the AMF
    ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.type(),
              asn1::ngap::ngap_pdu_c::types_opts::options::init_msg);
    ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.init_msg().value.type().value,
              asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_request);
    ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.init_msg().value.ue_context_release_request()->cause.type(),
              asn1::ngap::cause_c::types_opts::options::radio_network);
  }

  // check that UE has been added as new UE (old ue is not released, this is covered by ngap unittests)
  ASSERT_EQ(cu_cp_obj->get_connected_dus().get_nof_ues(), 2U);
}

TEST_F(cu_cp_test, when_old_ue_not_fully_attached_then_reestablishment_rejected)
{
  // Test preamble
  du_index_t          du_index  = uint_to_du_index(0);
  gnb_cu_ue_f1ap_id_t cu_ue_id  = int_to_gnb_cu_ue_f1ap_id(0);
  gnb_du_ue_f1ap_id_t du_ue_id  = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti     = to_rnti(0x4601);
  pci_t               pci       = 1;
  amf_ue_id_t         amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));
  ran_ue_id_t ran_ue_id = uint_to_ran_ue_id(0);
  test_preamble_ue_creation(du_index, du_ue_id, cu_ue_id, pci, crnti, amf_ue_id, ran_ue_id);

  // Attach second UE with RRC Reestablishment Request
  {
    gnb_cu_ue_f1ap_id_t cu_ue_id_2 = int_to_gnb_cu_ue_f1ap_id(1);
    gnb_du_ue_f1ap_id_t du_ue_id_2 = int_to_gnb_du_ue_f1ap_id(1);
    rnti_t              crnti_2    = to_rnti(0x4602);

    // Create Initial UL RRC message
    f1ap_message init_ul_rrc_msg = generate_init_ul_rrc_message_transfer(du_ue_id_2, crnti_2);

    init_ul_rrc_msg.pdu.init_msg().value.init_ul_rrc_msg_transfer()->rrc_container =
        generate_valid_rrc_reestablishment_request_pdu(pci, crnti, "1100011101010100");

    // Inject Initial UL RRC message
    test_logger.info("Injecting Initial UL RRC message (RRC Reestablishment Request)");
    cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(init_ul_rrc_msg);

    // Inject UL RRC message containing RRC Setup Complete
    f1ap_message ul_rrc_msg =
        generate_ul_rrc_message_transfer(cu_ue_id_2, du_ue_id_2, srb_id_t::srb1, generate_rrc_setup_complete());
    test_logger.info("Injecting UL RRC message (RRC Setup Complete)");
    cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(ul_rrc_msg);

    // check that the UE Context Release Request was sent to the AMF
    ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.type(),
              asn1::ngap::ngap_pdu_c::types_opts::options::init_msg);
    ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.init_msg().value.type().value,
              asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_request);
    ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.init_msg().value.ue_context_release_request()->cause.type(),
              asn1::ngap::cause_c::types_opts::options::radio_network);
  }

  // check that UE has been added as new UE (old ue is not released, this is covered by ngap unittests)
  ASSERT_EQ(cu_cp_obj->get_connected_dus().get_nof_ues(), 2U);
}

TEST_F(cu_cp_test, when_reestablishment_successful_then_ue_attached)
{
  // Test preamble
  du_index_t          du_index  = uint_to_du_index(0);
  gnb_cu_ue_f1ap_id_t cu_ue_id  = int_to_gnb_cu_ue_f1ap_id(0);
  gnb_du_ue_f1ap_id_t du_ue_id  = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti     = to_rnti(0x4601);
  pci_t               pci       = 1;
  amf_ue_id_t         amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));
  ran_ue_id_t            ran_ue_id        = uint_to_ran_ue_id(0);
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = int_to_gnb_cu_cp_ue_e1ap_id(0);
  gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id = int_to_gnb_cu_up_ue_e1ap_id(0);

  test_preamble_ue_full_attach(
      du_index, du_ue_id, cu_ue_id, pci, crnti, amf_ue_id, ran_ue_id, cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);

  // Attach second UE with RRC Reestablishment Request
  {
    gnb_cu_ue_f1ap_id_t cu_ue_id_2 = int_to_gnb_cu_ue_f1ap_id(1);
    gnb_du_ue_f1ap_id_t du_ue_id_2 = int_to_gnb_du_ue_f1ap_id(1);
    rnti_t              crnti_2    = to_rnti(0x4602);

    // Create Initial UL RRC message
    f1ap_message init_ul_rrc_msg = generate_init_ul_rrc_message_transfer(du_ue_id_2, crnti_2);

    init_ul_rrc_msg.pdu.init_msg().value.init_ul_rrc_msg_transfer()->rrc_container =
        generate_valid_rrc_reestablishment_request_pdu(pci, crnti, "1100011101010100");

    // Inject Initial UL RRC message
    test_logger.info("Injecting Initial UL RRC message (RRC Reestablishment Request)");
    cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(init_ul_rrc_msg);

    // Inject UL RRC message containing RRC Reestablishment Complete
    f1ap_message ul_rrc_msg =
        generate_ul_rrc_message_transfer(cu_ue_id_2, du_ue_id_2, srb_id_t::srb1, make_byte_buffer("00001800df0061cd"));
    test_logger.info("Injecting UL RRC message (RRC Reestablishment Complete)");
    cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(ul_rrc_msg);

    // check that the Bearer Context Modification Request Message was sent to the CU-UP
    ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
    ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
              asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_mod_request);

    // Inject Bearer Context Modification Response
    e1ap_message bearer_context_mod_resp =
        generate_bearer_context_modification_response(cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);
    cu_cp_obj->get_cu_cp_cu_up_connection_interface()
        .get_e1ap_message_handler(uint_to_cu_up_index(0))
        .handle_message(bearer_context_mod_resp);

    // check that the UE Context Modification Request Message was sent to the DU
    ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::options::init_msg);
    ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
              asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_mod_request);

    // Inject UE Context Modification Response
    f1ap_message ue_context_mod_resp = generate_ue_context_modification_response(cu_ue_id_2, du_ue_id_2);
    cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(ue_context_mod_resp);

    // check that the Bearer Context Modification was sent to the CU-UP
    ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
    ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
              asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_mod_request);

    // Inject Bearer Context Modification Response
    cu_cp_obj->get_cu_cp_cu_up_connection_interface()
        .get_e1ap_message_handler(uint_to_cu_up_index(0))
        .handle_message(bearer_context_mod_resp);

    // check that the RRC Reconfiguration was sent to the DU
    ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::options::init_msg);
    ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
              asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::dl_rrc_msg_transfer);
  }
}

//////////////////////////////////////////////////////////////////////////////////////
/* Handover Request handling                                                        */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(cu_cp_test, when_handover_request_received_then_handover_notify_is_sent)
{
  // Test preamble
  du_index_t du_index = uint_to_du_index(0);
  pci_t      pci      = 0;
  test_preamble_all_connected(du_index, pci);

  amf_ue_id_t amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));

  // Inject Handover Request
  cu_cp_obj->get_ngap_message_handler().handle_message(generate_valid_handover_request(amf_ue_id));

  // Check that the Bearer Context Setup Request Message was sent to the CU-UP
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_setup_request);
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.bearer_context_setup_request()->gnb_cu_cp_ue_e1ap_id,
            0);

  // Inject E1AP Bearer Context Setup Response
  e1ap_message bearer_ctxt_setup_resp =
      generate_bearer_context_setup_response(int_to_gnb_cu_cp_ue_e1ap_id(0), int_to_gnb_cu_up_ue_e1ap_id(0));
  cu_cp_obj->get_e1ap_message_handler(uint_to_cu_up_index(0)).handle_message(bearer_ctxt_setup_resp);

  // Check that the UE Context Setup Request Message was sent to the DU
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_setup_request);
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.init_msg().value.ue_context_setup_request()->gnb_cu_ue_f1ap_id, 0);

  // Inject F1AP UE Context Setup Response
  f1ap_message ue_ctxt_setup_resp =
      generate_ue_context_setup_response(int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(0));
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(ue_ctxt_setup_resp);

  // Check that the Bearer Context Modification Request Message was sent to the CU-UP
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_mod_request);
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.bearer_context_mod_request()->gnb_cu_cp_ue_e1ap_id, 0);

  // Inject E1AP Bearer Context Modification Response
  e1ap_message bearer_ctxt_mod_resp =
      generate_bearer_context_modification_response(int_to_gnb_cu_cp_ue_e1ap_id(0), int_to_gnb_cu_up_ue_e1ap_id(0));
  cu_cp_obj->get_e1ap_message_handler(uint_to_cu_up_index(0)).handle_message(bearer_ctxt_mod_resp);

  // Check that the Handover Request Ack was sent to the AMF
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.type(),
            asn1::ngap::ngap_pdu_c::types_opts::options::successful_outcome);
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.successful_outcome().value.type().value,
            asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::ho_request_ack);
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.successful_outcome().value.ho_request_ack()->amf_ue_ngap_id,
            amf_ue_id_to_uint(amf_ue_id));

  // Inject RRC Reconfiguration Complete with transaction_id=0
  f1ap_message rrc_recfg_complete = generate_ul_rrc_message_transfer(
      int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(0), srb_id_t::srb1, make_byte_buffer("800008004e17dae3"));
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(rrc_recfg_complete);

  // Check that the Handover Notify was sent to the AMF
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.type(), asn1::ngap::ngap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ho_notify);
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.init_msg().value.ho_notify()->amf_ue_ngap_id,
            amf_ue_id_to_uint(amf_ue_id));
}
