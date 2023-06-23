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
    manager                   = create_cell_meas_manager(cfg);
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

TEST_F(cell_meas_manager_test, when_empty_config_is_used_then_no_neighbor_cells_are_availble)
{
  create_empty_manager();

  nr_cell_id_t   cid      = 0;
  cu_cp_meas_cfg meas_cfg = manager->get_measurement_config(cid);

  // Make sure meas_cfg is empty.
  ASSERT_EQ(meas_cfg.meas_obj_to_add_mod_list.size(), 0);
  ASSERT_EQ(meas_cfg.report_cfg_to_add_mod_list.size(), 0);
  ASSERT_EQ(meas_cfg.meas_id_to_add_mod_list.size(), 0);
  ASSERT_EQ(meas_cfg.report_cfg_to_add_mod_list.size(), 0);
  ASSERT_FALSE(meas_cfg.s_measure_cfg.has_value());
  ASSERT_FALSE(meas_cfg.quant_cfg.has_value());
  ASSERT_FALSE(meas_cfg.meas_gap_cfg.has_value());
  ASSERT_FALSE(meas_cfg.meas_gap_sharing_cfg.has_value());
}
