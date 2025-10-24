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

#include "../cu_cp_test_messages.h"
#include "lib/cu_cp/up_resource_manager/up_resource_manager_impl.h"
#include "srsran/adt/byte_buffer.h"
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
  }

  void setup_initial_pdu_session()
  {
    cu_cp_pdu_session_resource_setup_request msg = generate_pdu_session_resource_setup();
    ASSERT_TRUE(manager.validate_request(msg.pdu_session_res_setup_items));

    // No DRB present
    ASSERT_EQ(manager.get_nof_drbs(), 0);

    // single PDU Session/DRB could be added
    const auto       psi    = uint_to_pdu_session_id(1);
    up_config_update update = manager.calculate_update(msg.pdu_session_res_setup_items);
    ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
    ASSERT_EQ(update.pdu_sessions_to_setup_list.at(psi).drb_to_add.size(), 1);

    // Assume DRB setup was successful.
    up_config_update_result result;
    result.pdu_sessions_added_list.push_back(update.pdu_sessions_to_setup_list.at(psi));
    manager.apply_config_update(result);

    // Verify PDU session and DRB were added
    ASSERT_EQ(manager.get_nof_drbs(), 1);
    ASSERT_EQ(manager.get_nof_pdu_sessions(), 1);
    ASSERT_EQ(manager.get_nof_qos_flows(psi), 1);
    ASSERT_EQ(manager.get_total_nof_qos_flows(), 1);
  }

  // This helper modifies the existing PDU session by adding a new QoS flow that should be mapped on a new DRB.
  void modify_pdu_session()
  {
    // Modify existing session.
    cu_cp_pdu_session_resource_modify_request msg = generate_pdu_session_resource_modification();
    const auto                                psi = uint_to_pdu_session_id(1);

    ASSERT_TRUE(manager.validate_request(msg));
    up_config_update update = manager.calculate_update(msg);

    // Verify calculated update.
    ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 0);
    ASSERT_EQ(update.pdu_sessions_to_modify_list.size(), 1);
    ASSERT_EQ(update.pdu_sessions_to_modify_list.at(psi).drb_to_add.size(), 1);

    // Apply update.
    up_config_update_result result;
    result.pdu_sessions_modified_list.push_back(update.pdu_sessions_to_modify_list.at(psi));
    manager.apply_config_update(result);

    // One PDU session with two QoS flows on two bearers.
    ASSERT_EQ(manager.get_nof_pdu_sessions(), 1);
    ASSERT_EQ(manager.get_total_nof_qos_flows(), 2);
    ASSERT_EQ(manager.get_nof_drbs(), 2);
    ASSERT_EQ(manager.get_nof_qos_flows(psi), 2);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

public:
  pdcp_config             pdcp_cfg{pdcp_rb_type::drb, pdcp_rlc_mode::am};
  up_resource_manager_cfg cfg{{{uint_to_five_qi(9), {pdcp_cfg}}, {uint_to_five_qi(7), {pdcp_cfg}}}, 8};
  up_resource_manager     manager{cfg};
};

TEST_F(up_resource_manager_test, when_initial_pdu_session_is_created_new_drb_is_set_up)
{
  setup_initial_pdu_session();
}

TEST_F(up_resource_manager_test, when_same_pdu_session_is_created_no_new_drb_is_set_up)
{
  cu_cp_pdu_session_resource_setup_request msg = generate_pdu_session_resource_setup();
  ASSERT_TRUE(manager.validate_request(msg.pdu_session_res_setup_items));

  // single DRB should be added
  up_config_update update = manager.calculate_update(msg.pdu_session_res_setup_items);
  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
  ASSERT_EQ(update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drb_to_add.size(), 1);

  // Assume DRB setup was successful.
  up_config_update_result result;
  result.pdu_sessions_added_list.push_back(update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)));
  manager.apply_config_update(result);

  ASSERT_EQ(manager.get_nof_drbs(), 1);

  // if same request is received again, no DRB should be added
  ASSERT_FALSE(manager.validate_request(msg.pdu_session_res_setup_items));
}

TEST_F(up_resource_manager_test, when_drb_is_added_pdcp_config_is_valid)
{
  cu_cp_pdu_session_resource_setup_request msg = generate_pdu_session_resource_setup();
  ASSERT_TRUE(manager.validate_request(msg.pdu_session_res_setup_items));
  up_config_update update = manager.calculate_update(msg.pdu_session_res_setup_items);

  // Verify DRB config
  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
  ASSERT_EQ(update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drb_to_add.size(), 1);
  ASSERT_EQ(
      update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drb_to_add[uint_to_drb_id(1)].pdcp_cfg.rb_type,
      pdcp_rb_type::drb);
}

TEST_F(up_resource_manager_test, when_pdu_session_setup_with_two_qos_flows_both_are_mapped_on_own_drb)
{
  cu_cp_pdu_session_resource_setup_request msg = generate_pdu_session_resource_setup(ue_index_t::min, 1, 2);
  ASSERT_TRUE(manager.validate_request(msg.pdu_session_res_setup_items));
  up_config_update update = manager.calculate_update(msg.pdu_session_res_setup_items);

  // Verify created DRBs.
  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
  ASSERT_EQ(update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drb_to_add.size(), 2);
}

TEST_F(up_resource_manager_test, when_pdu_session_gets_modified_new_drb_is_set_up)
{
  // Preamble.
  setup_initial_pdu_session();

  // The actual test as fixture for later reuse.
  modify_pdu_session();
}

TEST_F(up_resource_manager_test, when_inexiting_qos_flow_gets_removed_removal_fails)
{
  // Preamble.
  setup_initial_pdu_session();

  // Modify existing session by trying to remove invalid/inexisting QoS flow.
  cu_cp_pdu_session_resource_modify_request msg =
      generate_pdu_session_resource_modification_with_qos_flow_removal(uint_to_qos_flow_id(9));

  ASSERT_FALSE(manager.validate_request(msg));
}

/// Note: From TS 38.413 Sec 8.2.3.1 "PDU Session Resource Modify" it's not entirely clear what should happen in
/// the case where all QoS flows of a PDU session are removed in a Resource Modify Request.
/// The current assumption is that we reject the Modify Request as a PDU session resource release would be the
/// expected command.
TEST_F(up_resource_manager_test, when_only_existing_qos_flow_gets_removed_removal_fails)
{
  // Preamble.
  setup_initial_pdu_session();

  // Modify existing session and remove only existing QoS flow.
  cu_cp_pdu_session_resource_modify_request msg =
      generate_pdu_session_resource_modification_with_qos_flow_removal(uint_to_qos_flow_id(1));

  ASSERT_FALSE(manager.validate_request(msg));
}

TEST_F(up_resource_manager_test, when_existing_qos_flow_gets_removed_removal_succeeds_and_associated_drb_is_released)
{
  // Preamble.
  setup_initial_pdu_session();
  modify_pdu_session();

  // Modify existing session and remove existing QoS flow.
  cu_cp_pdu_session_resource_modify_request msg =
      generate_pdu_session_resource_modification_with_qos_flow_removal(uint_to_qos_flow_id(2));
  const auto psi = uint_to_pdu_session_id(1);

  ASSERT_TRUE(manager.validate_request(msg));

  up_config_update update = manager.calculate_update(msg);

  // Verify calculated update.
  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 0);
  ASSERT_EQ(update.pdu_sessions_to_modify_list.size(), 1);
  ASSERT_EQ(update.pdu_sessions_to_modify_list.at(psi).drb_to_add.size(), 0);
  ASSERT_EQ(update.pdu_sessions_to_modify_list.at(psi).drb_to_remove.size(), 1);

  // Apply update.
  up_config_update_result result;
  result.pdu_sessions_modified_list.push_back(update.pdu_sessions_to_modify_list.at(psi));
  manager.apply_config_update(result);

  // One PDU session with one QoS flows using one DRB.
  ASSERT_EQ(manager.get_nof_pdu_sessions(), 1);
  ASSERT_EQ(manager.get_total_nof_qos_flows(), 1);
  ASSERT_EQ(manager.get_nof_drbs(), 1);
  ASSERT_EQ(manager.get_nof_qos_flows(psi), 1);
}

TEST_F(up_resource_manager_test, when_pdu_session_gets_removed_all_resources_are_removed)
{
  // Preamble.
  setup_initial_pdu_session();

  // Modify PDU session and add QoS flow and DRB.
  modify_pdu_session();

  // Attempt to create new session with same PSI fails.
  cu_cp_pdu_session_resource_setup_request setup_msg = generate_pdu_session_resource_setup();
  ASSERT_FALSE(manager.validate_request(setup_msg.pdu_session_res_setup_items));

  // Remove existing session.
  cu_cp_pdu_session_resource_release_command release_msg = generate_pdu_session_resource_release();
  ASSERT_TRUE(manager.validate_request(release_msg));

  // Calculate update
  up_config_update update = manager.calculate_update(release_msg);

  // Verify calculated update.
  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 0);
  ASSERT_EQ(update.pdu_sessions_to_modify_list.size(), 0);
  ASSERT_EQ(update.pdu_sessions_to_remove_list.size(), 1);
  ASSERT_EQ(update.drb_to_remove_list.size(), 2);

  // Apply update.
  up_config_update_result result;
  result.pdu_sessions_removed_list.push_back(update.pdu_sessions_to_remove_list.front());
  manager.apply_config_update(result);

  // All resources are removed.
  ASSERT_EQ(manager.get_nof_pdu_sessions(), 0);
  ASSERT_EQ(manager.get_total_nof_qos_flows(), 0);
  ASSERT_EQ(manager.get_nof_drbs(), 0);

  // Setting up initial PDU session is possible again.
  setup_initial_pdu_session();
}

TEST_F(up_resource_manager_test, when_all_drbs_are_dirty_drb_id_allocation_fails)
{
  // Preamble.
  setup_initial_pdu_session();

  for (unsigned i = 0; i < MAX_NOF_DRBS - 1; i++) {
    // Attempt to create new session with PSI=2.
    pdu_session_id_t                         psi{2};
    cu_cp_pdu_session_resource_setup_request setup_msg =
        generate_pdu_session_resource_setup(ue_index_t::min, psi, qos_flow_id_t::min);
    ASSERT_TRUE(manager.validate_request(setup_msg.pdu_session_res_setup_items));

    // Calculate update
    up_config_update update = manager.calculate_update(setup_msg.pdu_session_res_setup_items);

    // Verify calculated update.
    ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
    ASSERT_EQ(update.pdu_sessions_to_modify_list.size(), 0);
    ASSERT_EQ(update.pdu_sessions_to_remove_list.size(), 0);
    ASSERT_EQ(update.drb_to_remove_list.size(), 0);

    // Apply update.
    up_config_update_result result;
    result.pdu_sessions_added_list.push_back(update.pdu_sessions_to_setup_list.at(psi));
    manager.apply_config_update(result);

    // There are two PDU sessions.
    ASSERT_EQ(manager.get_nof_pdu_sessions(), 2);
    ASSERT_EQ(manager.get_total_nof_qos_flows(), 2);
    ASSERT_EQ(manager.get_nof_drbs(), 2);

    // Remove PDU session so DRB id is marked as dirty.
    cu_cp_pdu_session_resource_release_command release_msg =
        generate_pdu_session_resource_release(ue_index_t::min, psi);
    ASSERT_TRUE(manager.validate_request(release_msg));

    up_config_update rel_update = manager.calculate_update(release_msg);

    // Verify calculated update.
    ASSERT_EQ(rel_update.pdu_sessions_to_setup_list.size(), 0);
    ASSERT_EQ(rel_update.pdu_sessions_to_modify_list.size(), 0);
    ASSERT_EQ(rel_update.pdu_sessions_to_remove_list.size(), 1);
    ASSERT_EQ(rel_update.drb_to_remove_list.size(), 1);

    // Apply update.
    up_config_update_result rel_result;
    rel_result.pdu_sessions_removed_list.push_back(rel_update.pdu_sessions_to_remove_list.front());
    manager.apply_config_update(rel_result);

    // All resources are removed.
    ASSERT_EQ(manager.get_nof_pdu_sessions(), 1);
    ASSERT_EQ(manager.get_total_nof_qos_flows(), 1);
    ASSERT_EQ(manager.get_nof_drbs(), 1);
  }

  // All DRBs were used. There are no more DRB IDs available and allocation will fail.
  {
    pdu_session_id_t                         psi{2};
    cu_cp_pdu_session_resource_setup_request setup_msg =
        generate_pdu_session_resource_setup(ue_index_t::min, psi, qos_flow_id_t::min);
    ASSERT_TRUE(manager.validate_request(setup_msg.pdu_session_res_setup_items));

    // Calculate update
    up_config_update update = manager.calculate_update(setup_msg.pdu_session_res_setup_items);

    // Verify calculated update.
    ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 0);
    ASSERT_EQ(update.pdu_sessions_to_modify_list.size(), 0);
    ASSERT_EQ(update.pdu_sessions_to_remove_list.size(), 0);
    ASSERT_EQ(update.drb_to_remove_list.size(), 0);
  }
  // After marking the keys as changed, we can create DRBs again.
  manager.refresh_drb_id_after_key_change();
  {
    pdu_session_id_t                         psi{2};
    cu_cp_pdu_session_resource_setup_request setup_msg =
        generate_pdu_session_resource_setup(ue_index_t::min, psi, qos_flow_id_t::min);
    ASSERT_TRUE(manager.validate_request(setup_msg.pdu_session_res_setup_items));

    // Calculate update
    up_config_update update = manager.calculate_update(setup_msg.pdu_session_res_setup_items);

    // Verify calculated update.
    ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
    ASSERT_EQ(update.pdu_sessions_to_modify_list.size(), 0);
    ASSERT_EQ(update.pdu_sessions_to_remove_list.size(), 0);
    ASSERT_EQ(update.drb_to_remove_list.size(), 0);
  }
}
