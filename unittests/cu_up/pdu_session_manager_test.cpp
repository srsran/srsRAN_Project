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

  timer_manager                                        timers;
  std::unique_ptr<dummy_gtpu_demux_ctrl>               gtpu_rx_demux;
  std::unique_ptr<gtpu_tunnel_tx_upper_layer_notifier> gtpu_tx_notifier;
  dummy_inner_f1u_bearer                               f1u_bearer;
  std::unique_ptr<dummy_f1u_gateway>                   f1u_gw;
  std::unique_ptr<pdu_session_manager_ctrl>            pdu_session_mng;
  network_interface_config                             net_config;
  srslog::basic_logger&                                logger = srslog::fetch_basic_logger("TEST", false);
};

/// PDU session handling tests (creation/deletion)
TEST_F(pdu_session_manager_test, when_valid_pdu_session_setup_item_session_can_be_added)
{
  // no sessions added yet
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 0);

  // prepare request
  e1ap_pdu_session_res_to_setup_item pdu_session_setup_item;
  pdu_session_setup_item.pdu_session_id = uint_to_pdu_session_id(1);
  pdu_session_setup_item.ng_ul_up_tnl_info.tp_address.from_bitstring("01111111000000000000000000000001");
  pdu_session_setup_item.ng_ul_up_tnl_info.gtp_teid = int_to_gtp_teid(0x12345678);

  // attempt to add session
  pdu_session_setup_result setup_result = pdu_session_mng->setup_pdu_session(pdu_session_setup_item);

  // check successful outcome
  ASSERT_TRUE(setup_result.success);
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);

  // attempt to remove non-existing session
  pdu_session_mng->remove_pdu_session(uint_to_pdu_session_id(2));

  // check successful outcome (unchanged)
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);

  // attempt to remove existing session
  pdu_session_mng->remove_pdu_session(uint_to_pdu_session_id(1));

  // check successful outcome (unchanged)
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 0);
}

TEST_F(pdu_session_manager_test, when_pdu_session_with_same_id_is_setup_session_cant_be_added)
{
  // no sessions added yet
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 0);

  // prepare request
  e1ap_pdu_session_res_to_setup_item pdu_session_setup_item;
  pdu_session_setup_item.pdu_session_id = uint_to_pdu_session_id(1);
  pdu_session_setup_item.ng_ul_up_tnl_info.tp_address.from_bitstring("01111111000000000000000000000001");
  pdu_session_setup_item.ng_ul_up_tnl_info.gtp_teid = int_to_gtp_teid(0x12345678);

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

  // prepare setup request (to create bearer)
  e1ap_pdu_session_res_to_setup_item pdu_session_setup_item;
  pdu_session_setup_item.pdu_session_id = uint_to_pdu_session_id(0x0d);
  pdu_session_setup_item.ng_ul_up_tnl_info.tp_address.from_bitstring("01111111000000000000000000000001");
  pdu_session_setup_item.ng_ul_up_tnl_info.gtp_teid = int_to_gtp_teid(0x12345678);

  e1ap_drb_to_setup_item_ng_ran drb_to_setup;
  drb_to_setup.drb_id                      = uint_to_drb_id(0x0b);
  drb_to_setup.pdcp_cfg.rlc_mod            = rlc_mode::um_bidir;
  drb_to_setup.pdcp_cfg.pdcp_sn_size_dl    = pdcp_sn_size::size18bits;
  drb_to_setup.pdcp_cfg.pdcp_sn_size_ul    = pdcp_sn_size::size18bits;
  drb_to_setup.pdcp_cfg.t_reordering_timer = 50;

  e1ap_qos_flow_qos_param_item qos_to_setup;
  qos_to_setup.qos_flow_id = uint_to_qos_flow_id(0xee);

  drb_to_setup.qos_flow_info_to_be_setup.emplace(uint_to_qos_flow_id(0xee), qos_to_setup);
  pdu_session_setup_item.drb_to_setup_list_ng_ran.emplace(uint_to_drb_id(0x0b), drb_to_setup);

  // attempt to add session
  pdu_session_setup_result setup_result = pdu_session_mng->setup_pdu_session(pdu_session_setup_item);

  // check successful outcome
  ASSERT_TRUE(setup_result.success);
  ASSERT_EQ(setup_result.pdu_session_id, uint_to_pdu_session_id(0x0d));
  ASSERT_EQ(setup_result.drb_setup_results.size(), 1);
  ASSERT_EQ(setup_result.drb_setup_results.begin()->drb_id, uint_to_drb_id(0x0b));
  ASSERT_EQ(setup_result.drb_setup_results.begin()->qos_flow_results.size(), 1);
  ASSERT_EQ(setup_result.drb_setup_results.begin()->qos_flow_results.begin()->qos_flow_id, uint_to_qos_flow_id(0xee));

  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);
  ASSERT_FALSE(gtpu_rx_demux->created_teid_list.empty());
  gtpu_rx_demux->created_teid_list.pop_front();
  ASSERT_TRUE(gtpu_rx_demux->created_teid_list.empty());

  ASSERT_FALSE(f1u_gw->created_ul_teid_list.empty());
  uint32_t ul_teid = f1u_gw->created_ul_teid_list.front();
  f1u_gw->created_ul_teid_list.pop_front();
  ASSERT_TRUE(f1u_gw->created_ul_teid_list.empty());

  // prepare modification request (to remove bearers)
  asn1::e1ap::pdu_session_res_to_modify_item_s pdu_session_modify_item;
  pdu_session_modify_item.pdu_session_id = 0x0d;

  asn1::e1ap::drb_to_rem_item_ng_ran_s invalid_drb_to_remove;
  invalid_drb_to_remove.drb_id = 0x0f;

  asn1::e1ap::drb_to_rem_item_ng_ran_s valid_drb_to_remove;
  valid_drb_to_remove.drb_id = 0x0b;

  pdu_session_modify_item.drb_to_rem_list_ng_ran.push_back(invalid_drb_to_remove);
  pdu_session_modify_item.drb_to_rem_list_ng_ran.push_back(valid_drb_to_remove);

  // attempt to remove bearers
  pdu_session_modification_result modification_result = pdu_session_mng->modify_pdu_session(pdu_session_modify_item);
  // check successful outcome
  ASSERT_TRUE(setup_result.success);

  // validate pdu session is not disconnected from GTP-U gateway
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);
  ASSERT_TRUE(gtpu_rx_demux->removed_teid_list.empty());

  // validate bearer is disconnected from F1-U gateway
  ASSERT_FALSE(f1u_gw->removed_ul_teid_list.empty());
  ASSERT_EQ(f1u_gw->removed_ul_teid_list.front(), ul_teid);
  f1u_gw->removed_ul_teid_list.pop_front();
  ASSERT_TRUE(f1u_gw->removed_ul_teid_list.empty());
}

TEST_F(pdu_session_manager_test, dtor_rm_all_sessions_and_bearers)
{
  // no sessions added yet
  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 0);

  // prepare setup request (to create bearer)
  e1ap_pdu_session_res_to_setup_item pdu_session_setup_item;
  pdu_session_setup_item.pdu_session_id = uint_to_pdu_session_id(0x0d);
  pdu_session_setup_item.ng_ul_up_tnl_info.tp_address.from_bitstring("01111111000000000000000000000001");
  pdu_session_setup_item.ng_ul_up_tnl_info.gtp_teid = int_to_gtp_teid(0x12345678);

  e1ap_drb_to_setup_item_ng_ran drb_to_setup;
  drb_to_setup.drb_id                      = uint_to_drb_id(0x0b);
  drb_to_setup.pdcp_cfg.rlc_mod            = rlc_mode::um_bidir;
  drb_to_setup.pdcp_cfg.pdcp_sn_size_dl    = pdcp_sn_size::size18bits;
  drb_to_setup.pdcp_cfg.pdcp_sn_size_ul    = pdcp_sn_size::size18bits;
  drb_to_setup.pdcp_cfg.t_reordering_timer = 50;

  e1ap_qos_flow_qos_param_item qos_to_setup;
  qos_to_setup.qos_flow_id = uint_to_qos_flow_id(0xee);

  drb_to_setup.qos_flow_info_to_be_setup.emplace(uint_to_qos_flow_id(0xee), qos_to_setup);
  pdu_session_setup_item.drb_to_setup_list_ng_ran.emplace(uint_to_drb_id(0x0b), drb_to_setup);

  // attempt to add session
  pdu_session_setup_result setup_result = pdu_session_mng->setup_pdu_session(pdu_session_setup_item);

  // check successful outcome
  ASSERT_TRUE(setup_result.success);
  ASSERT_EQ(setup_result.pdu_session_id, uint_to_pdu_session_id(0x0d));
  ASSERT_EQ(setup_result.drb_setup_results.size(), 1);
  ASSERT_EQ(setup_result.drb_setup_results.begin()->drb_id, uint_to_drb_id(0x0b));
  ASSERT_EQ(setup_result.drb_setup_results.begin()->qos_flow_results.size(), 1);
  ASSERT_EQ(setup_result.drb_setup_results.begin()->qos_flow_results.begin()->qos_flow_id, uint_to_qos_flow_id(0xee));

  ASSERT_EQ(pdu_session_mng->get_nof_pdu_sessions(), 1);
  ASSERT_FALSE(gtpu_rx_demux->created_teid_list.empty());
  uint32_t teid = gtpu_rx_demux->created_teid_list.front();
  gtpu_rx_demux->created_teid_list.pop_front();
  ASSERT_TRUE(gtpu_rx_demux->created_teid_list.empty());

  ASSERT_FALSE(f1u_gw->created_ul_teid_list.empty());
  uint32_t ul_teid = f1u_gw->created_ul_teid_list.front();
  f1u_gw->created_ul_teid_list.pop_front();
  ASSERT_TRUE(f1u_gw->created_ul_teid_list.empty());

  // delete pdu_session_mng, all remaining sessions and bearers shall be removed and detached from all gateways
  pdu_session_mng.reset();

  // validate pdu session is disconnected from GTP-U gateway
  ASSERT_FALSE(gtpu_rx_demux->removed_teid_list.empty());
  ASSERT_EQ(gtpu_rx_demux->removed_teid_list.front(), teid);
  gtpu_rx_demux->removed_teid_list.pop_front();
  ASSERT_TRUE(gtpu_rx_demux->removed_teid_list.empty());

  // validate bearer is disconnected from F1-U gateway
  ASSERT_FALSE(f1u_gw->removed_ul_teid_list.empty());
  ASSERT_EQ(f1u_gw->removed_ul_teid_list.front(), ul_teid);
  f1u_gw->removed_ul_teid_list.pop_front();
  ASSERT_TRUE(f1u_gw->removed_ul_teid_list.empty());
}
