/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/adt/byte_buffer.h"
#include "srsran/rrc/rrc_du_factory.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Fixture class RRC Setup tests preparation
class rrc_ue_setup : public rrc_ue_test_helper, public ::testing::Test
{
protected:
  static void SetUpTestSuite() { srslog::init(); }

  void SetUp() override { init(); }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
};

/// Test the RRC setup with disconnected AMF
TEST_F(rrc_ue_setup, when_amf_disconnected_then_rrc_reject_sent)
{
  receive_setup_request();

  // check if the RRC reject message was generated
  ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_reject);
}

/// Test the RRC setup with connected AMF
TEST_F(rrc_ue_setup, when_amf_connected_then_rrc_setup_sent)
{
  connect_amf();
  receive_setup_request();

  // check if the RRC setup message was generated
  ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

  // check if SRB1 was created
  check_srb1_exists();

  receive_setup_complete();
}

/// Test the correct handling of missing RRC setup complete message
TEST_F(rrc_ue_setup, when_setup_complete_timeout_then_ue_deleted)
{
  connect_amf();
  receive_setup_request();

  // check that UE has been created and was not requested to be released
  check_ue_release_not_requested();

  // tick timer until RRC setup complete timer fires
  tick_timer();

  // verify that RRC requested UE context release
  check_ue_release_requested();
}

TEST_F(rrc_ue_setup, when_setup_complete_received_initial_ue_message_sent)
{
  connect_amf();
  receive_setup_request();

  // check if the RRC setup message was generated
  ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

  // check if SRB1 was created
  check_srb1_exists();

  receive_setup_complete();

  check_initial_ue_message_sent();
}
