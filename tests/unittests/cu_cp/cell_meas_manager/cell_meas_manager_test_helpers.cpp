/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  cell_meas_cfg cell_cfg;
  cell_cfg.nci = 0;
  cell_cfg.ncells.push_back(1);
  cell_cfg.ncells.push_back(2);
  cfg.cells.emplace(cell_cfg.nci, cell_cfg);

  // Reuse config to setup config for next cell.
  cell_cfg.nci = 1;
  cell_cfg.ncells.clear();
  cell_cfg.ncells.push_back(0);
  cell_cfg.ncells.push_back(2);
  cfg.cells.emplace(cell_cfg.nci, cell_cfg);

  cell_cfg.nci = 2;
  cell_cfg.ncells.clear();
  cell_cfg.ncells.push_back(0);
  cell_cfg.ncells.push_back(1);
  cfg.cells.emplace(cell_cfg.nci, cell_cfg);

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
  cell_meas_cfg cell_cfg;
  cell_cfg.nci = 0;
  cell_cfg.ncells.push_back(1);
  cfg.cells.insert({cell_cfg.nci, cell_cfg});

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
