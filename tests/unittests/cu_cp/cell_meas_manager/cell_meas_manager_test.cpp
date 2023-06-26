/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/adt/byte_buffer.h"
#include "srsran/cu_cp/cell_meas_manager.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Fixture class to create cell meas manager object.
class cell_meas_manager_test : public ::testing::Test
{
protected:
  static void SetUpTestSuite() { srslog::init(); }

  void SetUp() override
  {
    srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-CP", false);
    logger.set_level(srslog::basic_levels::debug);
    logger.set_hex_dump_max_size(32);
  }

  void create_empty_manager()
  {
    cell_meas_manager_cfg cfg = {};
    // Don't add neighbor cells but a

    manager = create_cell_meas_manager(cfg);
    ASSERT_NE(manager, nullptr);
  }

  void create_manager_with_one_cell_and_one_neighbor_cell()
  {
    cell_meas_manager_cfg cfg;

    // Cell id 0 with single neighbor with cell id 1
    std::vector<nr_cell_id_t> neighbor_list;
    neighbor_list.push_back(1);
    cfg.neighbor_cell_list.insert({0, neighbor_list});

    // Add A3 event.
    cfg.a3_event_config.emplace();
    cfg.a3_event_config.value().a3_offset.rsrp.emplace();
    cfg.a3_event_config.value().a3_offset.rsrp.value() = 6;

    ASSERT_NO_FATAL_FAILURE(is_valid_configuration(cfg));

    manager = create_cell_meas_manager(cfg);
    ASSERT_NE(manager, nullptr);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

public:
  std::unique_ptr<cell_meas_manager> manager;
};

TEST_F(cell_meas_manager_test, when_empty_config_is_used_validation_succeeds)
{
  cell_meas_manager_cfg cfg = {};
  ASSERT_TRUE(is_valid_configuration(cfg));
}

TEST_F(cell_meas_manager_test, when_empty_neighbor_is_defined_but_no_event_configured_validation_fails)
{
  cell_meas_manager_cfg     cfg = {};
  std::vector<nr_cell_id_t> neighbor_list;
  neighbor_list.push_back(1);
  cfg.neighbor_cell_list.insert({0, neighbor_list});

  ASSERT_FALSE(is_valid_configuration(cfg));
}

TEST_F(cell_meas_manager_test, when_empty_config_is_used_then_no_neighbor_cells_are_availble)
{
  create_empty_manager();

  nr_cell_id_t   cid      = 0;
  cu_cp_meas_cfg meas_cfg = manager->get_measurement_config(cid);

  // Make sure meas_cfg is empty.
  ASSERT_EQ(meas_cfg.meas_obj_to_add_mod_list.size(), 0);
  ASSERT_EQ(meas_cfg.report_cfg_to_add_mod_list.size(), 0);
  ASSERT_EQ(meas_cfg.meas_id_to_add_mod_list.size(), 0);
  ASSERT_FALSE(meas_cfg.s_measure_cfg.has_value());
  ASSERT_FALSE(meas_cfg.quant_cfg.has_value());
  ASSERT_FALSE(meas_cfg.meas_gap_cfg.has_value());
  ASSERT_FALSE(meas_cfg.meas_gap_sharing_cfg.has_value());
}

TEST_F(cell_meas_manager_test, when_basic_config_is_used_then_meas_config_is_created)
{
  create_manager_with_one_cell_and_one_neighbor_cell();

  nr_cell_id_t   cid      = 0;
  cu_cp_meas_cfg meas_cfg = manager->get_measurement_config(cid);

  // Make sure meas_cfg is created.
  ASSERT_EQ(meas_cfg.meas_obj_to_add_mod_list.size(), 1);
  // FIXME: Enable asserts once full meas config is created.
  // ASSERT_EQ(meas_cfg.report_cfg_to_add_mod_list.size(), 1);
  // ASSERT_EQ(meas_cfg.meas_id_to_add_mod_list.size(), 1);
}
