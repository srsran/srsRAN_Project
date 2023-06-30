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

cell_meas_manager_test::cell_meas_manager_test()
{
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  test_logger.set_level(srslog::basic_levels::debug);
  test_logger.set_hex_dump_max_size(32);
  srslog::init();
}

cell_meas_manager_test::~cell_meas_manager_test()
{
  // flush logger after each test
  srslog::flush();
}

void cell_meas_manager_test::create_empty_manager()
{
  cell_meas_manager_cfg cfg = {};
  manager                   = create_cell_meas_manager(cfg);
  ASSERT_NE(manager, nullptr);
}

void cell_meas_manager_test::create_default_manager()
{
  cell_meas_manager_cfg cfg;

  // Add 3 cells with 2 neighbor cells each

  std::vector<nr_cell_id_t> neighbor_cells_0 = {1, 2};
  cfg.neighbor_cell_list.emplace(0, neighbor_cells_0);

  std::vector<nr_cell_id_t> neighbor_cells_1 = {0, 2};
  cfg.neighbor_cell_list.emplace(1, neighbor_cells_1);

  std::vector<nr_cell_id_t> neighbor_cells_2 = {0, 1};
  cfg.neighbor_cell_list.emplace(2, neighbor_cells_2);

  // Add A3 event.
  cfg.a3_event_config.emplace();
  cfg.a3_event_config.value().a3_offset.rsrp.emplace();
  cfg.a3_event_config.value().a3_offset.rsrp.value() = 6;

  manager = create_cell_meas_manager(cfg);
  ASSERT_NE(manager, nullptr);
}

void cell_meas_manager_test::create_manager_with_one_cell_and_one_neighbor_cell()
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

void cell_meas_manager_test::check_default_meas_cfg(const cu_cp_meas_cfg& meas_cfg)
{
  ASSERT_EQ(meas_cfg.meas_obj_to_add_mod_list.size(), 2);
  ASSERT_EQ(meas_cfg.meas_obj_to_add_mod_list.at(0).meas_obj_id, 0);
  ASSERT_EQ(meas_cfg.meas_obj_to_add_mod_list.at(1).meas_obj_id, 1);
  // TODO: Add checks for more values
}
