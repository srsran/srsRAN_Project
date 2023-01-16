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
    gtpu_rx_demux    = std::make_unique<dummy_gtpu_demux_ctrl>();
    gtpu_tx_notifier = std::make_unique<dummy_gtpu_network_gateway_adapter>();
    f1u_gw           = std::make_unique<dummy_f1u_gateway>(f1u_bearer);

    // create DUT object
    pdu_session_mng = std::make_unique<pdu_session_manager_impl>(
        MIN_UE_INDEX, net_config, logger, timers, *f1u_gw, *gtpu_tx_notifier, *gtpu_rx_demux);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  std::unique_ptr<gtpu_demux_ctrl>                     gtpu_rx_demux;
  std::unique_ptr<gtpu_tunnel_tx_upper_layer_notifier> gtpu_tx_notifier;
  dummy_f1u_bearer                                     f1u_bearer;
  std::unique_ptr<f1u_cu_up_gateway>                   f1u_gw;
  std::unique_ptr<pdu_session_manager_ctrl>            pdu_session_mng;
  network_interface_config                             net_config;
  srslog::basic_logger&                                logger = srslog::fetch_basic_logger("TEST", false);
  timer_manager                                        timers;
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

  // attempt to remove non-existing session
  pdu_session_mng->remove_pdu_session(2);

  // check successful outcome (unchanged)
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);

  // attempt to remove existing session
  pdu_session_mng->remove_pdu_session(1);

  // check successful outcome (unchanged)
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 0);
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

/// PDU session handling tests (creation/deletion)
TEST_F(pdu_session_manager_test, drb_create_modify_remove)
{
  // no sessions added yet
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 0);

  // prepare request
  asn1::e1ap::pdu_session_res_to_setup_item_s pdu_session_setup_item;
  pdu_session_setup_item.pdu_session_id = 0x0d;
  pdu_session_setup_item.ng_ul_up_tnl_info.set_gtp_tunnel().gtp_teid.from_number(0x12345678);

  asn1::e1ap::drb_to_setup_item_ng_ran_s drb_to_setup;
  drb_to_setup.drb_id                                   = 0x0b;
  drb_to_setup.pdcp_cfg.rlc_mode                        = asn1::e1ap::rlc_mode_opts::options::rlc_um_bidirectional;
  drb_to_setup.pdcp_cfg.pdcp_sn_size_dl                 = asn1::e1ap::pdcp_sn_size_opts::options::s_neg18;
  drb_to_setup.pdcp_cfg.pdcp_sn_size_ul                 = asn1::e1ap::pdcp_sn_size_opts::options::s_neg18;
  drb_to_setup.pdcp_cfg.discard_timer_present           = false;
  drb_to_setup.pdcp_cfg.t_reordering_timer_present      = true;
  drb_to_setup.pdcp_cfg.t_reordering_timer.t_reordering = asn1::e1ap::t_reordering_e::ms50;

  asn1::e1ap::qos_flow_qos_param_item_s qos_to_setup;
  qos_to_setup.qos_flow_id = 0xee;

  drb_to_setup.qos_flow_info_to_be_setup.push_back(qos_to_setup);
  pdu_session_setup_item.drb_to_setup_list_ng_ran.push_back(drb_to_setup);

  // attempt to add session
  pdu_session_setup_result setup_result = pdu_session_mng->setup_pdu_session(pdu_session_setup_item);

  // check successful outcome
  ASSERT_TRUE(setup_result.success);
  ASSERT_EQ(setup_result.pdu_session_id, 0x0d);
  ASSERT_EQ(setup_result.drb_setup_results.size(), 1);
  ASSERT_EQ(setup_result.drb_setup_results.begin()->drb_id, 0x0b);
  ASSERT_EQ(setup_result.drb_setup_results.begin()->qos_flow_results.size(), 1);
  ASSERT_EQ(setup_result.drb_setup_results.begin()->qos_flow_results.begin()->qos_flow_id, 0xee);

  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);
}
