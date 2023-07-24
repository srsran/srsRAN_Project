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
  manager                   = create_cell_meas_manager(cfg, mobility_manager);
  ASSERT_NE(manager, nullptr);
}

void cell_meas_manager_test::create_default_manager()
{
  cell_meas_manager_cfg cfg;

  // Add 2 cells - one being the neighbor of the other one

  cell_meas_config cell_cfg;
  cell_cfg.serving_cell_cfg.nci = 0;
  cell_cfg.ncells.push_back(1);
  cell_cfg.serving_cell_cfg.band.emplace()      = nr_band::n78;
  cell_cfg.serving_cell_cfg.ssb_arfcn.emplace() = 632628;
  cell_cfg.serving_cell_cfg.ssb_scs.emplace()   = subcarrier_spacing::kHz30;
  {
    rrc_ssb_mtc ssb_mtc;
    ssb_mtc.dur                                  = 1;
    ssb_mtc.periodicity_and_offset.sf5.emplace() = 0;
    cell_cfg.serving_cell_cfg.ssb_mtc.emplace()  = ssb_mtc;
  }
  cfg.cells.emplace(cell_cfg.serving_cell_cfg.nci, cell_cfg);

  // Reuse config to setup config for next cell.
  cell_cfg.serving_cell_cfg.nci = 1;
  cell_cfg.ncells.clear();
  cell_cfg.ncells.push_back(0);
  cell_cfg.serving_cell_cfg.band.emplace()      = nr_band::n78;
  cell_cfg.serving_cell_cfg.ssb_arfcn.emplace() = 632128;
  cell_cfg.serving_cell_cfg.ssb_scs.emplace()   = subcarrier_spacing::kHz30;
  {
    rrc_ssb_mtc ssb_mtc;
    ssb_mtc.dur                                  = 1;
    ssb_mtc.periodicity_and_offset.sf5.emplace() = 0;
    cell_cfg.serving_cell_cfg.ssb_mtc.emplace()  = ssb_mtc;
  }
  cfg.cells.emplace(cell_cfg.serving_cell_cfg.nci, cell_cfg);

  // Add A3 event.
  cfg.a3_event_config.emplace();
  cfg.a3_event_config.value().a3_offset.rsrp.emplace();
  cfg.a3_event_config.value().a3_offset.rsrp.value() = 6;

  manager = create_cell_meas_manager(cfg, mobility_manager);
  ASSERT_NE(manager, nullptr);
}

void cell_meas_manager_test::check_default_meas_cfg(const optional<rrc_meas_cfg>& meas_cfg)
{
  ASSERT_TRUE(meas_cfg.has_value());
  ASSERT_EQ(meas_cfg.value().meas_obj_to_add_mod_list.size(), 1);
  ASSERT_EQ(meas_cfg.value().meas_obj_to_add_mod_list.at(0).meas_obj_id, 1);
  // TODO: Add checks for more values
}

void cell_meas_manager_test::verify_empty_meas_cfg(const optional<rrc_meas_cfg>& meas_cfg)
{
  ASSERT_FALSE(meas_cfg.has_value());
}