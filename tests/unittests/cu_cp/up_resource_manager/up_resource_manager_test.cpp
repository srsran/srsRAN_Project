/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../du_processor_test_messages.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/cu_cp/up_resource_manager.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Fixture class to create UP resource manager object.
class up_resource_manager_test : public ::testing::Test
{
protected:
  static void SetUpTestSuite() { srslog::init(); }

  void SetUp() override
  {
    srslog::basic_logger& rrc_logger = srslog::fetch_basic_logger("RRC", false);
    rrc_logger.set_level(srslog::basic_levels::debug);
    rrc_logger.set_hex_dump_max_size(32);

    up_resource_manager_cfg cfg;
    cfg.five_qi_config[uint_to_five_qi(9)] = {};
    pdcp_config p_cfg;
    p_cfg.rb_type                               = pdcp_rb_type::drb;
    cfg.five_qi_config[uint_to_five_qi(9)].pdcp = p_cfg;
    cfg.five_qi_config[uint_to_five_qi(7)].pdcp = p_cfg;
    manager                                     = create_up_resource_manager(cfg);
  }

  void setup_initial_pdu_session()
  {
    cu_cp_pdu_session_resource_setup_request msg = generate_pdu_session_resource_setup();
    ASSERT_TRUE(manager->validate_request(msg));

    // No DRB present
    ASSERT_EQ(manager->get_nof_drbs(), 0);

    // single PDU Session/DRB could be added
    const auto&      psi    = uint_to_pdu_session_id(1);
    up_config_update update = manager->calculate_update(msg);
    ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
    ASSERT_EQ(update.pdu_sessions_to_setup_list.at(psi).drb_to_add.size(), 1);

    // Assume DRB setup was successful.
    up_config_update_result result;
    result.pdu_sessions_added_list.push_back(update.pdu_sessions_to_setup_list.at(psi));
    manager->apply_config_update(result);

    // Verify PDU session and DRB were added
    ASSERT_EQ(manager->get_nof_drbs(), 1);
    ASSERT_EQ(manager->get_nof_pdu_sessions(), 1);
    ASSERT_EQ(manager->get_nof_qos_flows(psi), 1);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

public:
  std::unique_ptr<up_resource_manager> manager;
};

TEST_F(up_resource_manager_test, when_initial_pdu_session_is_created_new_drb_is_set_up)
{
  setup_initial_pdu_session();
}

TEST_F(up_resource_manager_test, when_same_pdu_session_is_created_no_new_drb_is_set_up)
{
  cu_cp_pdu_session_resource_setup_request msg = generate_pdu_session_resource_setup();
  ASSERT_TRUE(manager->validate_request(msg));

  // single DRB should be added
  up_config_update update = manager->calculate_update(msg);
  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
  ASSERT_EQ(update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drb_to_add.size(), 1);

  // Assume DRB setup was successful.
  up_config_update_result result;
  result.pdu_sessions_added_list.push_back(update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)));
  manager->apply_config_update(result);

  ASSERT_EQ(manager->get_nof_drbs(), 1);

  // if same request is received again, no DRB should be added
  ASSERT_FALSE(manager->validate_request(msg));
}

TEST_F(up_resource_manager_test, when_drb_is_added_pdcp_config_is_valid)
{
  cu_cp_pdu_session_resource_setup_request msg = generate_pdu_session_resource_setup();
  ASSERT_TRUE(manager->validate_request(msg));
  up_config_update update = manager->calculate_update(msg);

  // Verify DRB config
  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
  ASSERT_EQ(update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drb_to_add.size(), 1);
  ASSERT_EQ(
      update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drb_to_add[uint_to_drb_id(1)].pdcp_cfg.rb_type,
      pdcp_rb_type::drb);
}

TEST_F(up_resource_manager_test, when_pdu_session_gets_modified_new_drb_is_set_up)
{
  // Preamble.
  setup_initial_pdu_session();

  // Modify existing session.
  cu_cp_pdu_session_resource_modify_request msg = generate_pdu_session_resource_modification();
  const auto&                               psi = uint_to_pdu_session_id(1);

  ASSERT_TRUE(manager->validate_request(msg));
  up_config_update update = manager->calculate_update(msg);

  // Verify calculated update.
  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 0);
  ASSERT_EQ(update.pdu_sessions_to_modify_list.size(), 1);

  ASSERT_EQ(update.pdu_sessions_to_modify_list.at(psi).drb_to_add.size(), 1);

  // Apply update.
  up_config_update_result result;
  result.pdu_sessions_modified_list.push_back(update.pdu_sessions_to_modify_list.at(psi));
  manager->apply_config_update(result);

  // 1 PDU session with two QoS flows with 2 bearers in one PDU session.
  ASSERT_EQ(manager->get_nof_pdu_sessions(), 1);
  ASSERT_EQ(manager->get_total_nof_qos_flows(), 2);
  ASSERT_EQ(manager->get_nof_drbs(), 2);
  ASSERT_EQ(manager->get_nof_qos_flows(psi), 2);
}