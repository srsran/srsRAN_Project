/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "cell_meas_manager_test_helpers.h"

using namespace srsran;
using namespace srs_cu_cp;

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

TEST_F(cell_meas_manager_test, when_empty_config_is_used_then_no_neighbor_cells_are_available)
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

TEST_F(cell_meas_manager_test, when_serving_cell_not_found_no_neighbor_cells_are_available)
{
  create_default_manager();

  nr_cell_id_t   cid      = 5;
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

TEST_F(cell_meas_manager_test, when_serving_cell_found_then_neighbor_cells_are_available)
{
  create_default_manager();

  for (unsigned cid = 0; cid < 3; ++cid) {
    cu_cp_meas_cfg meas_cfg = manager->get_measurement_config(cid);
    check_default_meas_cfg(meas_cfg);
  }
}
