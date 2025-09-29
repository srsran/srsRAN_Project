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

#include "cell_meas_manager_test_helpers.h"
#include "srsran/ran/plmn_identity.h"

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
  cell_cfg.serving_cell_cfg.nci                 = nr_cell_identity::create(0x19b0).value();
  cell_cfg.serving_cell_cfg.gnb_id_bit_length   = 32;
  cell_cfg.serving_cell_cfg.pci                 = 1;
  cell_cfg.serving_cell_cfg.band.emplace()      = nr_band::n78;
  cell_cfg.serving_cell_cfg.ssb_arfcn.emplace() = 632628;
  cell_cfg.serving_cell_cfg.ssb_scs.emplace()   = subcarrier_spacing::kHz30;
  rrc_ssb_mtc ssb_mtc;
  ssb_mtc.dur                                 = 1;
  ssb_mtc.periodicity_and_offset.periodicity  = rrc_periodicity_and_offset::periodicity_t::sf5;
  ssb_mtc.periodicity_and_offset.offset       = 0;
  cell_cfg.serving_cell_cfg.ssb_mtc.emplace() = ssb_mtc;
  ASSERT_TRUE(is_complete(cell_cfg.serving_cell_cfg));
}

TEST_F(cell_meas_manager_test, when_empty_config_is_used_validation_succeeds)
{
  cell_meas_manager_cfg cfg = {};
  ASSERT_TRUE(is_valid_configuration(cfg));
}

TEST_F(cell_meas_manager_test, when_empty_config_is_used_then_no_neighbor_cells_are_available)
{
  create_empty_manager();

  ue_index_t ue_index = ue_mng.add_ue(uint_to_du_index(0));
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  nr_cell_identity            nci      = nr_cell_identity::create(0x19b0).value();
  std::optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(ue_index, nci);

  // Make sure meas_cfg is empty.
  verify_empty_meas_cfg(meas_cfg);
}

TEST_F(cell_meas_manager_test, when_serving_cell_not_found_no_neighbor_cells_are_available)
{
  create_default_manager();

  ue_index_t ue_index = ue_mng.add_ue(uint_to_du_index(0));
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  nr_cell_identity            nci      = nr_cell_identity::create(0x19b5).value();
  std::optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(ue_index, nci);

  // Make sure meas_cfg is empty.
  verify_empty_meas_cfg(meas_cfg);
}

TEST_F(cell_meas_manager_test, when_serving_cell_found_then_neighbor_cells_are_available)
{
  create_default_manager();

  ue_index_t ue_index = ue_mng.add_ue(uint_to_du_index(0));
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));

  for (unsigned nci_val = 0x19b0; nci_val < 0x19b2; ++nci_val) {
    std::optional<rrc_meas_cfg> meas_cfg =
        manager->get_measurement_config(ue_index, nr_cell_identity::create(nci_val).value());
    check_default_meas_cfg(meas_cfg, meas_obj_id_t::min);
    verify_meas_cfg(meas_cfg);
  }
}

TEST_F(cell_meas_manager_test, when_inexisting_cell_config_is_updated_then_config_is_added)
{
  create_default_manager();

  ue_index_t ue_index = ue_mng.add_ue(uint_to_du_index(0));
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  const nr_cell_identity nci = nr_cell_identity::create(0x19b1).value();

  // get current config
  std::optional<cell_meas_config> cell_cfg = manager->get_cell_config(nci);
  ASSERT_TRUE(cell_cfg.has_value());

  // update config for cell 3
  auto& cell_cfg_val                                = cell_cfg.value();
  cell_cfg_val.serving_cell_cfg.gnb_id_bit_length   = 32;
  cell_cfg_val.serving_cell_cfg.nci                 = nr_cell_identity::create(0x19b3).value();
  cell_cfg_val.serving_cell_cfg.band.emplace()      = nr_band::n78;
  cell_cfg_val.serving_cell_cfg.ssb_arfcn.emplace() = 632628;
  cell_cfg_val.serving_cell_cfg.ssb_scs.emplace()   = subcarrier_spacing::kHz30;

  // Make sure meas_cfg is created.
  std::optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(ue_index, nci);
  check_default_meas_cfg(meas_cfg, meas_obj_id_t::min);
  verify_meas_cfg(meas_cfg);
}

TEST_F(cell_meas_manager_test, when_incomplete_cell_config_is_updated_then_valid_meas_config_is_created)
{
  create_default_manager();

  ue_index_t ue_index = ue_mng.add_ue(uint_to_du_index(0));
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  const nr_cell_identity nci = nr_cell_identity::create(0x19b1).value();

  // get current config
  std::optional<cell_meas_config> cell_cfg = manager->get_cell_config(nci);
  ASSERT_TRUE(cell_cfg.has_value());

  // update config for cell 1
  auto& cell_cfg_val                                = cell_cfg.value();
  cell_cfg_val.serving_cell_cfg.band.emplace()      = nr_band::n78;
  cell_cfg_val.serving_cell_cfg.ssb_arfcn.emplace() = 632628;
  cell_cfg_val.serving_cell_cfg.ssb_scs.emplace()   = subcarrier_spacing::kHz30;

  // Make sure meas_cfg is created.
  std::optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(ue_index, nci);
  check_default_meas_cfg(meas_cfg, meas_obj_id_t::min);
  verify_meas_cfg(meas_cfg);
}

TEST_F(cell_meas_manager_test, when_empty_cell_config_is_used_then_meas_cfg_is_not_set)
{
  // Create a manager without ncells and without report config.
  create_manager_without_ncells_and_periodic_report();

  ue_index_t ue_index = ue_mng.add_ue(uint_to_du_index(0));
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  nr_cell_identity            nci      = nr_cell_identity::create(0x19b0).value();
  std::optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(ue_index, nci);

  // Make sure meas_cfg is empty.
  verify_empty_meas_cfg(meas_cfg);
}

TEST_F(cell_meas_manager_test, when_old_meas_config_is_provided_old_ids_are_removed)
{
  create_default_manager();

  ue_index_t ue_index = ue_mng.add_ue(uint_to_du_index(0));
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  const nr_cell_identity initial_nci = nr_cell_identity::create(0x19b0).value();

  // Make sure meas_cfg is created (no previous meas config provided)
  std::optional<rrc_meas_cfg> initial_meas_cfg = manager->get_measurement_config(ue_index, initial_nci);
  check_default_meas_cfg(initial_meas_cfg, meas_obj_id_t::min);
  verify_meas_cfg(initial_meas_cfg);

  const nr_cell_identity      target_nci      = nr_cell_identity::create(0x19b1).value();
  std::optional<rrc_meas_cfg> target_meas_cfg = manager->get_measurement_config(ue_index, target_nci, initial_meas_cfg);

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

TEST_F(cell_meas_manager_test, when_only_event_based_reports_configured_then_meas_objects_are_created)
{
  create_manager_with_incomplete_cells_and_periodic_report_at_target_cell();

  ue_index_t ue_index = ue_mng.add_ue(uint_to_du_index(0));
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  const nr_cell_identity initial_nci = nr_cell_identity::create(0x19b0).value();
  const nr_cell_identity target_nci  = nr_cell_identity::create(0x19b1).value();

  // Make sure no meas_cfg is created (incomplete cell config)
  ASSERT_FALSE(manager->get_measurement_config(ue_index, initial_nci).has_value());
  ASSERT_FALSE(manager->get_measurement_config(ue_index, target_nci).has_value());

  serving_cell_meas_config serving_cell_cfg;
  serving_cell_cfg.gnb_id_bit_length   = 32;
  serving_cell_cfg.nci                 = initial_nci;
  serving_cell_cfg.pci                 = 1;
  serving_cell_cfg.band.emplace()      = nr_band::n78;
  serving_cell_cfg.ssb_arfcn.emplace() = 632628;
  serving_cell_cfg.ssb_scs.emplace()   = subcarrier_spacing::kHz30;
  {
    rrc_ssb_mtc ssb_mtc;
    ssb_mtc.dur                                = 1;
    ssb_mtc.periodicity_and_offset.periodicity = rrc_periodicity_and_offset::periodicity_t::sf5;
    ssb_mtc.periodicity_and_offset.offset      = 0;
    serving_cell_cfg.ssb_mtc.emplace()         = ssb_mtc;
  }

  // Update cell config for cell 1
  ASSERT_TRUE(manager->update_cell_config(initial_nci, serving_cell_cfg));

  // Update cell config for cell 2
  serving_cell_cfg.nci = target_nci;
  ASSERT_TRUE(manager->update_cell_config(target_nci, serving_cell_cfg));

  // Make sure meas_cfg is created and contains measurement objects to add mod
  std::optional<rrc_meas_cfg> initial_meas_cfg = manager->get_measurement_config(ue_index, initial_nci);
  ASSERT_TRUE(initial_meas_cfg.has_value());
  ASSERT_EQ(initial_meas_cfg.value().meas_obj_to_add_mod_list.size(), 1);
  ASSERT_TRUE(initial_meas_cfg.value().meas_obj_to_add_mod_list.begin()->meas_obj_nr.has_value());
  ASSERT_EQ(initial_meas_cfg.value().meas_obj_to_add_mod_list.begin()->meas_obj_nr.value().ssb_freq,
            serving_cell_cfg.ssb_arfcn);
  ASSERT_EQ(initial_meas_cfg.value().report_cfg_to_add_mod_list.size(), 1);

  std::optional<rrc_meas_cfg> target_meas_cfg = manager->get_measurement_config(ue_index, target_nci, initial_meas_cfg);
  ASSERT_TRUE(target_meas_cfg.has_value());
  ASSERT_EQ(target_meas_cfg.value().meas_obj_to_add_mod_list.size(), 1);
  ASSERT_TRUE(target_meas_cfg.value().meas_obj_to_add_mod_list.begin()->meas_obj_nr.has_value());
  ASSERT_EQ(target_meas_cfg.value().meas_obj_to_add_mod_list.begin()->meas_obj_nr.value().ssb_freq,
            serving_cell_cfg.ssb_arfcn);
  ASSERT_EQ(target_meas_cfg.value().report_cfg_to_add_mod_list.size(), 2);
}

TEST_F(cell_meas_manager_test, when_invalid_cell_config_update_received_then_config_is_not_updated)
{
  create_manager_with_incomplete_cells_and_periodic_report_at_target_cell();

  ue_index_t ue_index = ue_mng.add_ue(uint_to_du_index(0));
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  const nr_cell_identity initial_nci = nr_cell_identity::create(0x19b0).value();
  const nr_cell_identity target_nci  = nr_cell_identity::create(0x19b1).value();

  // Make sure no meas_cfg is created (incomplete cell config)
  ASSERT_FALSE(manager->get_measurement_config(ue_index, initial_nci).has_value());
  ASSERT_FALSE(manager->get_measurement_config(ue_index, target_nci).has_value());

  serving_cell_meas_config serving_cell_cfg;
  serving_cell_cfg.gnb_id_bit_length = 32;
  serving_cell_cfg.nci               = initial_nci;
  serving_cell_cfg.pci               = 1;
  serving_cell_cfg.band              = nr_band::n78;
  serving_cell_cfg.ssb_arfcn         = 632628;
  serving_cell_cfg.ssb_scs           = subcarrier_spacing::kHz30;
  {
    rrc_ssb_mtc ssb_mtc;
    ssb_mtc.dur                                = 1;
    ssb_mtc.periodicity_and_offset.periodicity = rrc_periodicity_and_offset::periodicity_t::sf5;
    ssb_mtc.periodicity_and_offset.offset      = 0;
    serving_cell_cfg.ssb_mtc                   = ssb_mtc;
  }

  // Update cell config for cell 1
  ASSERT_TRUE(manager->update_cell_config(initial_nci, serving_cell_cfg));

  // Update cell config for cell 2 with different scs for same ssb_freq
  serving_cell_cfg.nci     = target_nci;
  serving_cell_cfg.ssb_scs = subcarrier_spacing::kHz15;

  ASSERT_FALSE(manager->update_cell_config(target_nci, serving_cell_cfg));

  // Make sure meas_cfg is created for cell 1 and contains measurement objects to add mod
  std::optional<rrc_meas_cfg> initial_meas_cfg = manager->get_measurement_config(ue_index, initial_nci);
  ASSERT_TRUE(initial_meas_cfg.has_value());
  ASSERT_TRUE(initial_meas_cfg.value().meas_obj_to_add_mod_list.empty());
  ASSERT_TRUE(initial_meas_cfg.value().report_cfg_to_add_mod_list.empty());

  std::optional<rrc_meas_cfg> target_meas_cfg = manager->get_measurement_config(ue_index, target_nci, initial_meas_cfg);
  ASSERT_FALSE(target_meas_cfg.has_value());
}
