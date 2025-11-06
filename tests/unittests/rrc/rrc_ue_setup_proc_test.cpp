/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "rrc_ue_test_helpers.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Fixture class RRC Setup tests preparation.
class rrc_ue_setup : public rrc_ue_test_helper, public ::testing::Test
{
protected:
  static void SetUpTestSuite() { srslog::init(); }

  void SetUp() override { init(); }

  void TearDown() override
  {
    // Flush logger after each test.
    srslog::flush();
  }
};

/// Test the RRC setup with disconnected AMF.
TEST_F(rrc_ue_setup, when_amf_disconnected_then_rrc_reject_sent)
{
  this->rrc_ue_cu_cp_notifier.next_ue_setup_response = false;

  receive_setup_request();

  check_ue_release_requested();
}

/// Test the RRC setup with connected AMF.
TEST_F(rrc_ue_setup, when_amf_connected_then_rrc_setup_sent)
{
  receive_setup_request();

  // Check if the RRC setup message was generated.
  ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

  // Check if SRB1 was created.
  check_srb1_exists();

  receive_setup_complete();
}

/// Test the RRC setup with disconnected AMF.
TEST_F(rrc_ue_setup, when_rrc_setup_invalid_then_rrc_reject_sent)
{
  receive_invalid_setup_request();

  check_ue_release_requested();
}

/// Test the correct handling of missing RRC setup complete message.
TEST_F(rrc_ue_setup, when_setup_complete_timeout_then_ue_deleted)
{
  receive_setup_request();

  // Check that UE has been created and was not requested to be released.
  check_ue_release_not_requested();

  // Tick timer until RRC setup complete timer fires.
  tick_timer();

  // Verify that RRC requested UE context release.
  check_ue_release_requested();
}

TEST_F(rrc_ue_setup, when_setup_complete_received_initial_ue_message_sent)
{
  receive_setup_request();

  // Check if the RRC setup message was generated.
  ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

  // Check if SRB1 was created.
  check_srb1_exists();

  receive_setup_complete();

  check_initial_ue_message_sent();
}

/// Test the correct handling of corrupted RRC setup complete message.
TEST_F(rrc_ue_setup, when_integrity_failure_detected_then_ue_deleted)
{
  receive_setup_request();

  // Check if the RRC setup message was generated.
  ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

  // Check if SRB1 was created.
  check_srb1_exists();

  receive_corrupted_setup_complete();

  // Tick timer until RRC setup complete timer fires.
  tick_timer();

  // Verify that RRC requested UE context release.
  check_ue_release_requested();
}

/// Test the correct handling of corrupted RRC setup complete message.
TEST_F(rrc_ue_setup, when_malformed_setup_complete_received_then_ue_deleted)
{
  receive_setup_request();

  // Check if the RRC setup message was generated.
  ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

  // Check if SRB1 was created.
  check_srb1_exists();

  // UL-DCCH with malformed RRC setup complete message.
  std::array<uint8_t, 28> malformed_rrc_setup_complete_pdu = {
      0x00, 0x00, 0x18, 0x00, 0x05, 0xdf, 0x80, 0x10, 0x5e, 0x40, 0x03, 0x40, 0x40, 0x3c,
      0x44, 0x00, 0x00, 0x00, 0x00, 0x04, 0x0c, 0x95, 0x1d, 0x82, 0x0b, 0x80, 0xbc, 0x1c};

  // Inject malformed RRC setup complete.
  rrc_ue->get_ul_pdu_handler().handle_ul_dcch_pdu(srb_id_t::srb1,
                                                  byte_buffer::create(malformed_rrc_setup_complete_pdu).value());
  // Tick timer until RRC setup complete timer fires.
  tick_timer();

  // Verify that RRC requested UE context release.
  check_ue_release_requested();
}
