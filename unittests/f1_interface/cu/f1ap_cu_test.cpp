/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/f1_interface/cu/f1ap_cu_factory.h"
#include "srsgnb/f1_interface/cu/f1c_cu.h"
#include "srsgnb/support/test_utils.h"
#include "unittests/f1_interface/common/f1_cu_test_helpers.h"
#include "unittests/f1_interface/common/test_helpers.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;
using namespace asn1::f1ap;

//////////////////////////////////////////////////////////////////////////////////////
/* Handling of unsupported messages                                                 */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(f1ap_cu_test, when_unsupported_f1ap_pdu_received_then_message_ignored)
{
  // Set last message of PDU notifier to init_msg
  f1c_pdu_notifier->last_f1c_msg.pdu.set_init_msg();

  // Generate unupported F1AP PDU
  f1c_message unsupported_msg = {};
  unsupported_msg.pdu.set_choice_ext();

  f1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded (last PDU is still init_msg)
  EXPECT_EQ(f1c_pdu_notifier->last_f1c_msg.pdu.type(), asn1::f1ap::f1_ap_pdu_c::types_opts::options::init_msg);
}

TEST_F(f1ap_cu_test, when_unsupported_init_msg_received_then_message_ignored)
{
  // Set last message of PDU notifier to successful outcome
  f1c_pdu_notifier->last_f1c_msg.pdu.set_successful_outcome();

  // Generate unupported F1AP PDU
  f1c_message unsupported_msg = {};
  unsupported_msg.pdu.set_init_msg();

  f1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded (last PDU is still successful_outcome)
  EXPECT_EQ(f1c_pdu_notifier->last_f1c_msg.pdu.type(),
            asn1::f1ap::f1_ap_pdu_c::types_opts::options::successful_outcome);
}

TEST_F(f1ap_cu_test, when_unsupported_successful_outcome_received_then_message_ignored)
{
  // Set last message of PDU notifier to init_msg
  f1c_pdu_notifier->last_f1c_msg.pdu.set_init_msg();

  // Generate unupported F1AP PDU
  f1c_message unsupported_msg = {};
  unsupported_msg.pdu.set_successful_outcome();

  f1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded (last PDU is still init_msg)
  EXPECT_EQ(f1c_pdu_notifier->last_f1c_msg.pdu.type(), asn1::f1ap::f1_ap_pdu_c::types_opts::options::init_msg);
}

TEST_F(f1ap_cu_test, when_unsupported_unsuccessful_outcome_received_then_message_ignored)
{
  // Set last message of PDU notifier to init_msg
  f1c_pdu_notifier->last_f1c_msg.pdu.set_init_msg();

  // Generate unupported F1AP PDU
  f1c_message unsupported_msg = {};
  unsupported_msg.pdu.set_unsuccessful_outcome();

  f1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded (last PDU is still init_msg)
  EXPECT_EQ(f1c_pdu_notifier->last_f1c_msg.pdu.type(), asn1::f1ap::f1_ap_pdu_c::types_opts::options::init_msg);
}

//////////////////////////////////////////////////////////////////////////////////////
/* Initial UL RRC Message handling                                                  */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(f1ap_cu_test, when_duto_currc_container_missing_then_ue_not_added)
{
  // Generate F1 Initial UL RRC Message without DU to CU RRC Container
  f1c_message init_ul_rrc_msg = generate_f1_init_ul_rrc_msg_base(41255);

  // Pass message to F1AP
  f1ap->handle_message(init_ul_rrc_msg);

  EXPECT_EQ(f1ap->get_nof_ues(), 0);
}

TEST_F(f1ap_cu_test, when_max_nof_ues_exceeded_then_ue_not_added)
{
  // Reduce F1AP and TEST logger loglevel to warning to reduce console output
  srslog::fetch_basic_logger("CU-F1AP").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::warning);

  // Add the maximum number of UEs
  for (int ue_index = MIN_UE_INDEX; ue_index < MAX_NOF_UES; ue_index++) {
    // Generate ue_creation message
    rnti_t      c_rnti          = to_rnti(ue_index + 1); // 0 is not a valid RNTI
    f1c_message init_ul_rrc_msg = generate_valid_f1_init_ul_rrc_msg(c_rnti);

    // Pass message to F1AP
    f1ap->handle_message(init_ul_rrc_msg);
  }

  // Reset F1AP and TEST logger loglevel
  srslog::fetch_basic_logger("CU-F1AP").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);

  EXPECT_EQ(f1ap->get_nof_ues(), MAX_NOF_UES);

  // Add one more UE to F1AP
  // Generate ue_creation message
  rnti_t      c_rnti          = to_rnti(MAX_NOF_UES + 1);
  f1c_message init_ul_rrc_msg = generate_valid_f1_init_ul_rrc_msg(c_rnti);

  // Pass message to F1AP
  f1ap->handle_message(init_ul_rrc_msg);

  EXPECT_EQ(f1ap->get_nof_ues(), MAX_NOF_UES);
}

TEST_F(f1ap_cu_test, when_ue_creation_fails_then_ue_not_added)
{
  // Add maximum number of UEs to dummy DU processor
  du_processor_notifier->set_ue_index(MAX_NOF_UES);

  // Add one more UE to F1AP
  // Generate F1 Initial UL RRC Message
  f1c_message init_ul_rrc_msg = generate_valid_f1_init_ul_rrc_msg(41255);

  // Pass message to F1AP
  f1ap->handle_message(init_ul_rrc_msg);

  EXPECT_EQ(f1ap->get_nof_ues(), 0);
}

TEST_F(f1ap_cu_test, when_rrc_setup_complete_present_then_forward_over_srb1)
{
  // Generate F1 Initial UL RRC Message with RRC Setup Complete present
  f1c_message init_ul_rrc_msg = generate_valid_f1_init_ul_rrc_msg(41255);
  auto&       init_ul_rrc     = init_ul_rrc_msg.pdu.init_msg().value.init_ulrrc_msg_transfer();
  init_ul_rrc->rrc_container_rrc_setup_complete_present = true;

  // Pass message to F1AP
  f1ap->handle_message(init_ul_rrc_msg);

  EXPECT_EQ(du_processor_notifier->rx_notifier->last_rrc_container.to_string(),
            init_ul_rrc->rrc_container_rrc_setup_complete.value.to_string());
}

//////////////////////////////////////////////////////////////////////////////////////
/* F1 Removal Request handling                                                      */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(f1ap_cu_test, when_f1_removal_request_received_then_du_deleted)
{
  // Generate F1 Removal Request Message
  f1c_message removal_request = {};
  removal_request.pdu.set_init_msg();
  removal_request.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_F1_REMOVAL);

  // Pass message to F1AP
  f1ap->handle_message(removal_request);

  EXPECT_EQ(f1c_du_mgmt_notifier->last_du_idx, 0);
}
