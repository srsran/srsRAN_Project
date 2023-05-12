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
    manager                                     = create_up_resource_manager(cfg);
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
  cu_cp_pdu_session_resource_setup_request msg = generate_pdu_session_resource_setup();

  // No DRB present
  ASSERT_EQ(manager->get_nof_drbs(), 0);

  // single PDU Session/DRB could be added
  up_config_update update = manager->calculate_update(msg);
  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
  ASSERT_EQ(update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drbs.size(), 1);

  // Assume DRB setup was successful.
  up_config_update_result result;
  result.pdu_sessions_added_list.push_back(update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)));
  manager->apply_config_update(result);

  // Verify PDU session and DRB were added
  ASSERT_EQ(manager->get_nof_drbs(), 1);
  ASSERT_EQ(manager->get_pdu_sessions().size(), 1);
}

TEST_F(up_resource_manager_test, when_same_pdu_session_is_created_no_new_drb_is_set_up)
{
  cu_cp_pdu_session_resource_setup_request msg = generate_pdu_session_resource_setup();

  // single DRB should be added
  up_config_update update = manager->calculate_update(msg);
  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
  ASSERT_EQ(update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drbs.size(), 1);

  // Assume DRB setup was successful.
  up_config_update_result result;
  result.pdu_sessions_added_list.push_back(update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)));
  manager->apply_config_update(result);

  ASSERT_EQ(manager->get_nof_drbs(), 1);

  // if same request is received again, no DRB should be added
  ASSERT_EQ(manager->calculate_update(msg).pdu_sessions_to_setup_list.size(), 0);
}

TEST_F(up_resource_manager_test, when_drb_is_added_pdcp_config_is_valid)
{
  cu_cp_pdu_session_resource_setup_request msg    = generate_pdu_session_resource_setup();
  up_config_update                         update = manager->calculate_update(msg);

  // Verify DRB config
  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
  ASSERT_EQ(update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drbs.size(), 1);
  ASSERT_EQ(update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drbs[uint_to_drb_id(1)].pdcp_cfg.rb_type,
            pdcp_rb_type::drb);
}
