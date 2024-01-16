/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

TEST_F(cell_meas_manager_test, when_empty_cell_config_is_used_validation_fails)
{
  cell_meas_config cell_cfg;
  ASSERT_FALSE(is_complete(cell_cfg.serving_cell_cfg));
}

TEST_F(cell_meas_manager_test, when_valid_cell_config_is_used_validation_succeeds)
{
  cell_meas_config cell_cfg;
  cell_cfg.serving_cell_cfg.nci                 = 0;
  cell_cfg.serving_cell_cfg.band.emplace()      = nr_band::n78;
  cell_cfg.serving_cell_cfg.ssb_arfcn.emplace() = 632628;
  cell_cfg.serving_cell_cfg.ssb_scs.emplace()   = subcarrier_spacing::kHz30;
  rrc_ssb_mtc ssb_mtc;
  ssb_mtc.dur                                  = 1;
  ssb_mtc.periodicity_and_offset.sf5.emplace() = 0;
  cell_cfg.serving_cell_cfg.ssb_mtc.emplace()  = ssb_mtc;
  ASSERT_TRUE(is_complete(cell_cfg.serving_cell_cfg));
}

TEST_F(cell_meas_manager_test, when_empty_config_is_used_validation_succeeds)
{
  cell_meas_manager_cfg cfg = {};
  ASSERT_TRUE(is_valid_configuration(cfg));
}

TEST_F(cell_meas_manager_test, when_empty_neighbor_is_defined_but_no_event_configured_validation_fails)
{
  cell_meas_manager_cfg cfg = {};
  cell_meas_config      cell_cfg;
  cell_cfg.serving_cell_cfg.nci = 0;
  cfg.cells.insert({cell_cfg.serving_cell_cfg.nci, cell_cfg});

  ASSERT_FALSE(is_valid_configuration(cfg));
}

TEST_F(cell_meas_manager_test, when_empty_config_is_used_then_no_neighbor_cells_are_available)
{
  create_empty_manager();

  nr_cell_id_t           cid      = 0;
  optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(cid);

  // Make sure meas_cfg is empty.
  verify_empty_meas_cfg(meas_cfg);
}

TEST_F(cell_meas_manager_test, when_serving_cell_not_found_no_neighbor_cells_are_available)
{
  create_default_manager();

  nr_cell_id_t           cid      = 5;
  optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(cid);

  // Make sure meas_cfg is empty.
  verify_empty_meas_cfg(meas_cfg);
}

TEST_F(cell_meas_manager_test, when_serving_cell_found_then_neighbor_cells_are_available)
{
  create_default_manager();

  meas_obj_id_t meas_obj_id = meas_obj_id_t::min;
  for (unsigned cid = 0; cid < 2; ++cid) {
    optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(cid);
    check_default_meas_cfg(meas_cfg, meas_obj_id);
    meas_obj_id = uint_to_meas_obj_id(meas_obj_id_to_uint(meas_obj_id) + 2);
  }
}

TEST_F(cell_meas_manager_test, when_inexisting_cell_config_is_updated_then_config_is_added)
{
  create_default_manager();

  const nr_cell_id_t nci = 1;

  // get current config
  optional<cell_meas_config> cell_cfg = manager->get_cell_config(nci);
  ASSERT_TRUE(cell_cfg.has_value());

  // update config for cell 3
  auto& cell_cfg_val                                = cell_cfg.value();
  cell_cfg_val.serving_cell_cfg.nci                 = 3;
  cell_cfg_val.serving_cell_cfg.band.emplace()      = nr_band::n78;
  cell_cfg_val.serving_cell_cfg.ssb_arfcn.emplace() = 632628;
  cell_cfg_val.serving_cell_cfg.ssb_scs.emplace()   = subcarrier_spacing::kHz30;

  // Make sure meas_cfg is created.
  optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(nci);
  check_default_meas_cfg(meas_cfg, meas_obj_id_t::min);
}

TEST_F(cell_meas_manager_test, when_incomplete_cell_config_is_updated_then_valid_meas_config_is_created)
{
  create_default_manager();

  const nr_cell_id_t nci = 1;

  // get current config
  optional<cell_meas_config> cell_cfg = manager->get_cell_config(nci);
  ASSERT_TRUE(cell_cfg.has_value());

  // update config for cell 1
  auto& cell_cfg_val                                = cell_cfg.value();
  cell_cfg_val.serving_cell_cfg.band.emplace()      = nr_band::n78;
  cell_cfg_val.serving_cell_cfg.ssb_arfcn.emplace() = 632628;
  cell_cfg_val.serving_cell_cfg.ssb_scs.emplace()   = subcarrier_spacing::kHz30;

  // Make sure meas_cfg is created.
  optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(nci);
  check_default_meas_cfg(meas_cfg, meas_obj_id_t::min);
}

TEST_F(cell_meas_manager_test, when_empty_cell_config_is_used_then_meas_cfg_is_not_set)
{
  // Create a manager without ncells and without report config.
  create_manager_without_ncells_and_periodic_report();

  nr_cell_id_t           cid      = 0;
  optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(cid);

  // Make sure meas_cfg is empty.
  verify_empty_meas_cfg(meas_cfg);
}

TEST_F(cell_meas_manager_test, when_old_meas_config_is_provided_old_ids_are_removed)
{
  create_default_manager();

  const nr_cell_id_t initial_nci = 0;

  // Make sure meas_cfg is created (no previous meas config provided)
  optional<rrc_meas_cfg> initial_meas_cfg = manager->get_measurement_config(initial_nci);
  check_default_meas_cfg(initial_meas_cfg, meas_obj_id_t::min);
  verify_meas_cfg(initial_meas_cfg);

  const nr_cell_id_t     target_nci      = 1;
  optional<rrc_meas_cfg> target_meas_cfg = manager->get_measurement_config(target_nci, initial_meas_cfg);

  // Make sure initial IDs are release again.
  ASSERT_EQ(target_meas_cfg.value().meas_obj_to_rem_list.at(0),
            initial_meas_cfg.value().meas_obj_to_add_mod_list.at(0).meas_obj_id);

  ASSERT_EQ(target_meas_cfg.value().meas_id_to_rem_list.at(0),
            initial_meas_cfg.value().meas_id_to_add_mod_list.at(0).meas_id);

  ASSERT_EQ(target_meas_cfg.value().report_cfg_to_rem_list.at(0),
            initial_meas_cfg.value().report_cfg_to_add_mod_list.at(0).report_cfg_id);

  // The new config should reuse the IDs again.
  check_default_meas_cfg(target_meas_cfg, meas_obj_id_t::min);
  verify_meas_cfg(target_meas_cfg);
}