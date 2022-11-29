/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_ue_test_helpers.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/rrc/rrc_du_factory.h"
#include "srsgnb/support/async/async_task_loop.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;

/// Fixture class RRC Setup tests preparation
class rrc_setup : public rrc_ue_test_helper, public ::testing::Test
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
TEST_F(rrc_setup, when_amf_disconnected_then_rrc_reject_sent)
{
  receive_setup_request();

  // check if the RRC reject message was generated
  ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_reject);
}

/// Test the RRC setup with connected AMF
TEST_F(rrc_setup, when_amf_connected_then_rrc_setup_sent)
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
TEST_F(rrc_setup, when_setup_complete_timeout_then_ue_deleted)
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

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
