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

TEST_F(cell_meas_manager_test, when_empty_cell_config_is_used_validation_fails)
{
  cell_meas_cfg cell_cfg;
  ASSERT_FALSE(is_complete(cell_cfg));
}

TEST_F(cell_meas_manager_test, when_valid_cell_config_is_used_validation_succeeds)
{
  cell_meas_cfg cell_cfg;
  cell_cfg.nci                 = 0;
  cell_cfg.band.emplace()      = nr_band::n78;
  cell_cfg.ssb_arfcn.emplace() = 12;
  cell_cfg.ssb_scs.emplace()   = subcarrier_spacing::kHz30;
  rrc_ssb_mtc ssb_mtc;
  ssb_mtc.dur                                  = 1;
  ssb_mtc.periodicity_and_offset.sf5.emplace() = 0;
  cell_cfg.ssb_mtc.emplace()                   = ssb_mtc;
  ASSERT_TRUE(is_complete(cell_cfg));
}

TEST_F(cell_meas_manager_test, when_empty_config_is_used_validation_succeeds)
{
  cell_meas_manager_cfg cfg = {};
  ASSERT_TRUE(is_valid_configuration(cfg));
}

TEST_F(cell_meas_manager_test, when_empty_neighbor_is_defined_but_no_event_configured_validation_fails)
{
  cell_meas_manager_cfg cfg = {};
  cell_meas_cfg         cell_cfg;
  cell_cfg.nci = 0;
  cfg.cells.insert({cell_cfg.nci, cell_cfg});

  ASSERT_FALSE(is_valid_configuration(cfg));
}

TEST_F(cell_meas_manager_test, when_empty_config_is_used_then_no_neighbor_cells_are_available)
{
  create_empty_manager();

  nr_cell_id_t cid      = 0;
  rrc_meas_cfg meas_cfg = manager->get_measurement_config(cid);

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

  nr_cell_id_t cid      = 0;
  rrc_meas_cfg meas_cfg = manager->get_measurement_config(cid);

  // Make sure meas_cfg is created.
  ASSERT_EQ(meas_cfg.meas_obj_to_add_mod_list.size(), 1);
  // FIXME: Enable asserts once full meas config is created.
  // ASSERT_EQ(meas_cfg.report_cfg_to_add_mod_list.size(), 1);
  // ASSERT_EQ(meas_cfg.meas_id_to_add_mod_list.size(), 1);
}

TEST_F(cell_meas_manager_test, when_serving_cell_not_found_no_neighbor_cells_are_available)
{
  create_default_manager();

  nr_cell_id_t cid      = 5;
  rrc_meas_cfg meas_cfg = manager->get_measurement_config(cid);

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
    rrc_meas_cfg meas_cfg = manager->get_measurement_config(cid);
    check_default_meas_cfg(meas_cfg);
  }
}
