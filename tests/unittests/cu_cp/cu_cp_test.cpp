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
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/ngap/ngap_types.h"
#include "srsran/ran/cu_types.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::f1ap;

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
  f1ap_message_notifier* f1c_rx_pdu_notif = this->f1c_gw.request_new_du_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = test_helpers::generate_f1_setup_request();
  // Pass message to CU-CP
  f1c_rx_pdu_notif->on_new_message(f1setup_msg);

  // Generate Paging
  ngap_message paging_msg = generate_valid_minimal_paging_message();

  cu_cp_obj->get_ngap_message_handler().handle_message(paging_msg);

  ASSERT_TRUE(check_minimal_paging_result());
}

/// Test the handling of a valid Paging message for multiple DUs with only mandatory values set
TEST_F(cu_cp_test, when_valid_paging_message_received_then_paging_is_only_sent_to_du_with_matching_tac)
{
  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure)
  f1ap_message_notifier* f1c_rx_pdu_notif1 = this->f1c_gw.request_new_du_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = test_helpers::generate_f1_setup_request();
  // Pass message to CU-CP
  f1c_rx_pdu_notif1->on_new_message(f1setup_msg);

  // Connect second DU
  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure)
  f1ap_message_notifier* f1c_rx_pdu_notif2 = this->f1c_gw.request_new_du_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg2 =
      test_helpers::generate_f1_setup_request(int_to_gnb_du_id(0x12), nr_cell_identity::create(6577).value(), 1, 8);

  // Pass message to CU-CP
  f1c_rx_pdu_notif2->on_new_message(f1setup_msg2);

  // Generate Paging
  ngap_message paging_msg = generate_valid_minimal_paging_message();

  cu_cp_obj->get_ngap_message_handler().handle_message(paging_msg);

  ASSERT_TRUE(check_minimal_paging_result());
}

/// Test the handling of a valid Paging message for multiple DUs with only mandatory values set
TEST_F(cu_cp_test, when_valid_paging_message_received_then_paging_is_only_sent_to_du_with_matching_nci)
{
  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure)
  f1ap_message_notifier* f1c_rx_pdu_notif1 = this->f1c_gw.request_new_du_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = test_helpers::generate_f1_setup_request();
  // Pass message to CU-CP
  f1c_rx_pdu_notif1->on_new_message(f1setup_msg);

  // Connect second DU
  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure)
  f1ap_message_notifier* f1c_rx_pdu_notif2 = this->f1c_gw.request_new_du_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg2 =
      test_helpers::generate_f1_setup_request(int_to_gnb_du_id(0x12), nr_cell_identity::create(6577).value(), 1, 7);

  // Pass message to CU-CP
  f1c_rx_pdu_notif2->on_new_message(f1setup_msg2);

  // Generate Paging
  ngap_message paging_msg = generate_valid_minimal_paging_message();

  cu_cp_obj->get_ngap_message_handler().handle_message(paging_msg);

  ASSERT_TRUE(check_minimal_paging_result());
}

/// Test the handling of a valid Paging message with optional values set
TEST_F(cu_cp_test, when_valid_paging_message_with_optional_values_received_then_paging_is_sent_to_du)
{
  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure)
  f1ap_message_notifier* f1c_rx_pdu_notif = this->f1c_gw.request_new_du_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = test_helpers::generate_f1_setup_request();
  // Pass message to CU-CP
  f1c_rx_pdu_notif->on_new_message(f1setup_msg);

  // Generate Paging
  ngap_message paging_msg = generate_valid_paging_message();

  cu_cp_obj->get_ngap_message_handler().handle_message(paging_msg);

  ASSERT_TRUE(check_paging_result());
}

/// Test the handling of an invalid Paging message
TEST_F(cu_cp_test, when_no_du_for_tac_exists_then_paging_is_not_sent_to_du)
{
  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure)
  f1ap_message_notifier* f1c_rx_pdu_notif = this->f1c_gw.request_new_du_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = test_helpers::generate_f1_setup_request();
  // Pass message to CU-CP
  f1c_rx_pdu_notif->on_new_message(f1setup_msg);

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
  f1ap_message_notifier* f1c_rx_pdu_notif = this->f1c_gw.request_new_du_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = test_helpers::generate_f1_setup_request();
  // Pass message to CU-CP
  f1c_rx_pdu_notif->on_new_message(f1setup_msg);

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
  f1ap_message_notifier* f1c_rx_pdu_notif = this->f1c_gw.request_new_du_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = test_helpers::generate_f1_setup_request();
  // Pass message to CU-CP
  f1c_rx_pdu_notif->on_new_message(f1setup_msg);

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
  // Connect AMF, DU, CU-UP
  test_preamble_all_connected(du_index, pci);
  // Attach UE
  test_preamble_ue_creation(du_index, du_ue_id, cu_ue_id, crnti, amf_ue_id, ran_ue_id);

  cu_cp_inactivity_notification inactivity_notification;
  inactivity_notification.ue_index    = uint_to_ue_index(0);
  inactivity_notification.ue_inactive = true;

  cu_cp_obj->handle_bearer_context_inactivity_notification(inactivity_notification);

  // check that the UE Context Release Request was sent to the AMF
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.type(), asn1::ngap::ngap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_request);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.ue_context_release_request()->cause.type(),
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
  // Connect AMF, DU, CU-UP
  test_preamble_all_connected(du_index, pci);
  // Attach UE
  test_preamble_ue_creation(du_index, du_ue_id, cu_ue_id, crnti, amf_ue_id, ran_ue_id);

  cu_cp_inactivity_notification inactivity_notification;
  inactivity_notification.ue_index    = uint_to_ue_index(0);
  inactivity_notification.ue_inactive = false;

  cu_cp_obj->handle_bearer_context_inactivity_notification(inactivity_notification);

  // check that the UE Context Release Request was not sent to the AMF
  ASSERT_NE(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_request);
}

//////////////////////////////////////////////////////////////////////////////////////
/* AMF initiated PDU Session Release                                                */
//////////////////////////////////////////////////////////////////////////////////////

// When multiple PDU sessions exist and not all of them are released, a BearerContextModification is sent to the CU-UP
TEST_F(cu_cp_test, when_pdu_session_resource_release_command_received_then_modification_request_is_sent)
{
  // Test preamble
  du_index_t          du_index  = uint_to_du_index(0);
  gnb_cu_ue_f1ap_id_t cu_ue_id  = int_to_gnb_cu_ue_f1ap_id(0);
  gnb_du_ue_f1ap_id_t du_ue_id  = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti     = to_rnti(0x4601);
  pci_t               pci       = 1;
  amf_ue_id_t         amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));
  ran_ue_id_t                   ran_ue_id        = uint_to_ran_ue_id(0);
  std::vector<pdu_session_id_t> psis             = {uint_to_pdu_session_id(1), uint_to_pdu_session_id(2)};
  gnb_cu_cp_ue_e1ap_id_t        cu_cp_ue_e1ap_id = int_to_gnb_cu_cp_ue_e1ap_id(0);
  gnb_cu_up_ue_e1ap_id_t        cu_up_ue_e1ap_id = int_to_gnb_cu_up_ue_e1ap_id(0);

  // Connect AMF, DU, CU-UP
  test_preamble_all_connected(du_index, pci);
  // Attach UE
  test_preamble_ue_full_attach(
      du_index, du_ue_id, cu_ue_id, crnti, amf_ue_id, ran_ue_id, psis, cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);

  // Inject PduSessionResourceReleaseCommand
  cu_cp_obj->get_ngap_message_handler().handle_message(
      generate_valid_pdu_session_resource_release_command(amf_ue_id, ran_ue_id, uint_to_pdu_session_id(1)));

  // Check that the Bearer Context Modification was sent to the CU-UP
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_mod_request);
}

// When all active PDU sessions are released a BearerContextReleaseCommand is sent to the CU-UP
TEST_F(cu_cp_test, when_all_pdu_sessions_get_released_then_bearer_context_release_command_is_sent)
{
  // Test preamble
  du_index_t          du_index  = uint_to_du_index(0);
  gnb_cu_ue_f1ap_id_t cu_ue_id  = int_to_gnb_cu_ue_f1ap_id(0);
  gnb_du_ue_f1ap_id_t du_ue_id  = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti     = to_rnti(0x4601);
  pci_t               pci       = 1;
  amf_ue_id_t         amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));
  ran_ue_id_t                   ran_ue_id        = uint_to_ran_ue_id(0);
  std::vector<pdu_session_id_t> psis             = {uint_to_pdu_session_id(1)};
  gnb_cu_cp_ue_e1ap_id_t        cu_cp_ue_e1ap_id = int_to_gnb_cu_cp_ue_e1ap_id(0);
  gnb_cu_up_ue_e1ap_id_t        cu_up_ue_e1ap_id = int_to_gnb_cu_up_ue_e1ap_id(0);

  // Connect AMF, DU, CU-UP
  test_preamble_all_connected(du_index, pci);
  // Attach UE
  test_preamble_ue_full_attach(
      du_index, du_ue_id, cu_ue_id, crnti, amf_ue_id, ran_ue_id, psis, cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);

  // Inject PduSessionResourceReleaseCommand
  cu_cp_obj->get_ngap_message_handler().handle_message(
      generate_valid_pdu_session_resource_release_command(amf_ue_id, ran_ue_id, uint_to_pdu_session_id(1)));

  // Check that the Bearer Context Release was sent to the CU-UP
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_release_cmd);

  // Check that no UE Context Release Request was sent to the AMF
  ASSERT_NE(n2_gw.last_ngap_msgs.back().pdu.type(), asn1::ngap::ngap_pdu_c::types_opts::options::init_msg);
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
  // Connect AMF, DU, CU-UP
  test_preamble_all_connected(du_index, pci);
  // Attach UE
  test_preamble_ue_creation(du_index, du_ue_id, cu_ue_id, crnti, amf_ue_id, ran_ue_id);

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
  // Connect AMF, DU, CU-UP
  test_preamble_all_connected(du_index, pci);
  // Attach UE
  test_preamble_ue_creation(du_index, du_ue_id, cu_ue_id, crnti, amf_ue_id, ran_ue_id);

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
  f1c_gw.get_du(uint_to_du_index(0))
      .on_new_message(test_helpers::generate_ue_context_release_complete(cu_ue_id, du_ue_id));

  // check that the ErrorIndication was sent to the AMF
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type().value,
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
  // Connect AMF, DU, CU-UP
  test_preamble_all_connected(du_index, pci);
  // Attach UE
  test_preamble_ue_creation(du_index, du_ue_id, cu_ue_id, crnti, amf_ue_id, ran_ue_id);

  // Inject UE Context Release Request
  f1c_gw.get_du(du_index).on_new_message(test_helpers::generate_ue_context_release_request(cu_ue_id, du_ue_id));

  // Check that the UE Context Release Request was sent to the AMF
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.type(), asn1::ngap::ngap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_request);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.ue_context_release_request()->cause.type(),
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
  ASSERT_EQ(cu_cp_obj->get_metrics_handler().request_metrics_report().ues.size(), 1);

  // Inject UE Context Release Request
  f1c_gw.get_du(uint_to_du_index(0))
      .on_new_message(test_helpers::generate_ue_context_release_request(cu_ue_id, du_ue_id));

  // Check that the UE Context Release Request was not sent to the AMF
  ASSERT_NE(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_request);

  // Check that the Ue Context Release Command was sent to the DU
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_release_cmd);

  // Inject UE Context Release Complete
  f1c_gw.get_du(uint_to_du_index(0)).on_new_message(generate_ue_context_release_complete(cu_ue_id, du_ue_id));

  // Check that the UE was removed
  ASSERT_EQ(cu_cp_obj->get_metrics_handler().request_metrics_report().ues.size(), 0);
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
  e1ap_gw.get_cu_up(0).on_new_message(bearer_ctxt_setup_resp);

  // Check that the UE Context Setup Request Message was sent to the DU
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_setup_request);
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.init_msg().value.ue_context_setup_request()->gnb_cu_ue_f1ap_id, 0);

  // Inject F1AP UE Context Setup Response
  f1ap_message ue_ctxt_setup_resp =
      generate_ue_context_setup_response(int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(0), to_rnti(0x4601));
  f1c_gw.get_du(du_index).on_new_message(ue_ctxt_setup_resp);

  // Check that the Bearer Context Modification Request Message was sent to the CU-UP
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_mod_request);
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.bearer_context_mod_request()->gnb_cu_cp_ue_e1ap_id, 0);

  // Inject E1AP Bearer Context Modification Response
  e1ap_message bearer_ctxt_mod_resp =
      generate_bearer_context_modification_response(int_to_gnb_cu_cp_ue_e1ap_id(0), int_to_gnb_cu_up_ue_e1ap_id(0));
  e1ap_gw.get_cu_up(0).on_new_message(bearer_ctxt_mod_resp);

  // Check that the Handover Request Ack was sent to the AMF
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.type(), asn1::ngap::ngap_pdu_c::types_opts::options::successful_outcome);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.successful_outcome().value.type().value,
            asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::ho_request_ack);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.successful_outcome().value.ho_request_ack()->amf_ue_ngap_id,
            amf_ue_id_to_uint(amf_ue_id));

  // Inject RRC Reconfiguration Complete with transaction_id=0
  f1ap_message rrc_recfg_complete = generate_ul_rrc_message_transfer(int_to_gnb_cu_ue_f1ap_id(0),
                                                                     int_to_gnb_du_ue_f1ap_id(0),
                                                                     srb_id_t::srb1,
                                                                     make_byte_buffer("800008004e17dae3").value());
  f1c_gw.get_du(du_index).on_new_message(rrc_recfg_complete);

  // Check that the Handover Notify was sent to the AMF
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.type(), asn1::ngap::ngap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ho_notify);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.ho_notify()->amf_ue_ngap_id, amf_ue_id_to_uint(amf_ue_id));
}
