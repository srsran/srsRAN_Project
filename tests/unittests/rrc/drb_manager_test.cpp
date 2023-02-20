/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../cu_cp/common/du_processor_test_messages.h"
#include "../lib/rrc/ue/drb_manager_impl.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/rrc/drb_manager.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Fixture class RRC Reconfiguration tests preparation (to bring UE in RRC connected state)
class drb_manager_test : public ::testing::Test
{
protected:
  static void SetUpTestSuite() { srslog::init(); }

  void SetUp() override
  {
    srslog::basic_logger& rrc_logger = srslog::fetch_basic_logger("RRC", false);
    rrc_logger.set_level(srslog::basic_levels::debug);
    rrc_logger.set_hex_dump_max_size(32);

    drb_manager_cfg cfg;
    manager = std::make_unique<drb_manager_impl>(cfg);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

public:
  std::unique_ptr<drb_manager> manager;
};

TEST_F(drb_manager_test, when_initial_pdu_session_is_created_new_drb_is_set_up)
{
  cu_cp_pdu_session_resource_setup_request msg = generate_pdu_session_resource_setup();

  // No DRB present
  ASSERT_EQ(manager->get_nof_drbs(), 0);

  // single DRB could be added
  std::vector<drb_id_t> drbs_to_add = manager->calculate_drb_to_add_list(msg);
  ASSERT_EQ(drbs_to_add.size(), 1);

  // Verify DRB was added
  ASSERT_EQ(manager->get_nof_drbs(), 1);
}

TEST_F(drb_manager_test, when_same_pdu_session_is_created_no_new_drb_is_set_up)
{
  cu_cp_pdu_session_resource_setup_request msg = generate_pdu_session_resource_setup();

  // single DRB should be added
  ASSERT_EQ(manager->calculate_drb_to_add_list(msg).size(), 1);

  // if same request is received again, no DRB should be added
  ASSERT_EQ(manager->calculate_drb_to_add_list(msg).size(), 0);

  ASSERT_EQ(manager->get_nof_drbs(), 1);
}

TEST_F(drb_manager_test, when_drb_is_added_pdcp_config_is_valid)
{
  cu_cp_pdu_session_resource_setup_request msg         = generate_pdu_session_resource_setup();
  std::vector<drb_id_t>                    drbs_to_add = manager->calculate_drb_to_add_list(msg);

  // Verify DRB config
  const auto pdcp_cfg = manager->get_pdcp_config(drbs_to_add.at(0));
  ASSERT_TRUE(pdcp_cfg.drb.has_value());
}