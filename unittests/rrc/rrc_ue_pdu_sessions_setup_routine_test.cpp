/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdu_session_setup_helpers.h"
#include "rrc_ue_test_helpers.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/support/async/async_task_loop.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;

/// Fixture class RRC Reconfiguration tests preparation (to bring UE in RRC connected state)
class rrc_ue_pdu_session_resource_setup : public rrc_ue_test_helper, public ::testing::Test
{
protected:
  static void SetUpTestSuite() { srslog::init(); }

  void SetUp() override
  {
    init();

    srslog::basic_logger& rrc_logger = srslog::fetch_basic_logger("RRC", false);
    rrc_logger.set_level(srslog::basic_levels::debug);
    rrc_logger.set_hex_dump_max_size(32);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  void attach_ue()
  {
    connect_amf();
    receive_setup_request();

    // check if the RRC setup message was generated
    ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

    // check if SRB1 was created
    check_srb1_exists();

    receive_setup_complete();
  }
};

/// Test handling of PDU session setup request when UE is in RRC idle state.
/// Once paging is implemented this test needs to be updated.
TEST_F(rrc_ue_pdu_session_resource_setup, when_ue_not_connected_setup_fails)
{
  // Prepare args
  cu_cp_pdu_session_resource_setup_message msg = fill_res_setup_request();

  // Trigger PDU session setup
  async_task<cu_cp_pdu_session_resource_setup_response_message> t =
      get_rrc_ue_pdu_session_resource_handler()->handle_new_pdu_session_resource_setup_request(msg);
  lazy_task_launcher<cu_cp_pdu_session_resource_setup_response_message> t_launcher(t);

  ASSERT_TRUE(t.ready());

  // nothing has been set up
  ASSERT_EQ(t.get().pdu_session_res_setup_response_items.size(), 0);

  // requested session failed to setup
  ASSERT_EQ(t.get().pdu_session_res_failed_to_setup_items.size(), 1);
  ASSERT_EQ(t.get().pdu_session_res_failed_to_setup_items.at(0).pdu_session_id, 1);
}

/// Test handling of PDU session setup request when UE is in RRC connected state.
TEST_F(rrc_ue_pdu_session_resource_setup, when_ue_connected_setup_succeeds)
{
  attach_ue();

  // Prepare args
  cu_cp_pdu_session_resource_setup_message msg = fill_res_setup_request();

  // Trigger PDU session setup
  async_task<cu_cp_pdu_session_resource_setup_response_message> t =
      get_rrc_ue_pdu_session_resource_handler()->handle_new_pdu_session_resource_setup_request(msg);
  lazy_task_launcher<cu_cp_pdu_session_resource_setup_response_message> t_launcher(t);

  ASSERT_FALSE(t.ready());

  // Receive Reconfig complete
  receive_reconfig_complete();

  ASSERT_TRUE(t.ready());

  // PDU session has been set up
  ASSERT_EQ(t.get().pdu_session_res_setup_response_items.size(), 1);
  ASSERT_EQ(t.get().pdu_session_res_setup_response_items.at(0).pdu_session_id, 1);

  // nothing has failed to setup
  ASSERT_EQ(t.get().pdu_session_res_failed_to_setup_items.size(), 0);
}
