/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_test_helpers.h"
#include "lib/cu_up/pdu_session_manager_impl.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_up;

/// Fixture class for UE manager tests
class pdu_session_manager_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    // create required objects
    ngu_demux = std::make_unique<dummy_ngu>();
    f1u_conn  = std::make_unique<dummy_f1u_connector>();
    // create DUT object
    pdu_session_mng = std::make_unique<pdu_session_manager_impl>(MIN_UE_INDEX, logger, timers, *f1u_conn, *ngu_demux);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  std::unique_ptr<gtpu_demux_ctrl>          ngu_demux;
  std::unique_ptr<f1u_cu_up_connector>      f1u_conn;
  std::unique_ptr<pdu_session_manager_ctrl> pdu_session_mng;
  srslog::basic_logger&                     logger = srslog::fetch_basic_logger("TEST", false);
  timer_manager                             timers;
};

/// PDU session handling tests (creation/deletion)
TEST_F(pdu_session_manager_test, when_valid_pdu_session_setup_item_session_can_be_added)
{
  // no sessions added yet
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 0);

  // prepare request
  asn1::e1ap::pdu_session_res_to_setup_item_s pdu_session_setup_item;
  pdu_session_setup_item.pdu_session_id = 1;
  pdu_session_setup_item.ng_ul_up_tnl_info.set_gtp_tunnel().gtp_teid.from_number(0x12345678);

  // attempt to add session
  pdu_session_setup_result setup_result = pdu_session_mng->setup_pdu_session(pdu_session_setup_item);

  // check successful outcome
  ASSERT_TRUE(setup_result.success);
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);
}

TEST_F(pdu_session_manager_test, when_pdu_session_with_same_id_is_setup_session_cant_be_added)
{
  // no sessions added yet
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 0);

  // prepare request
  asn1::e1ap::pdu_session_res_to_setup_item_s pdu_session_setup_item;
  pdu_session_setup_item.pdu_session_id = 1;
  pdu_session_setup_item.ng_ul_up_tnl_info.set_gtp_tunnel().gtp_teid.from_number(0x12345678);

  // attempt to add session
  pdu_session_setup_result setup_result = pdu_session_mng->setup_pdu_session(pdu_session_setup_item);

  // check successful outcome
  ASSERT_TRUE(setup_result.success);
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);

  // attempt to add the same session again
  setup_result = pdu_session_mng->setup_pdu_session(pdu_session_setup_item);

  // check unsuccessful outcome
  ASSERT_FALSE(setup_result.success);
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);
}
