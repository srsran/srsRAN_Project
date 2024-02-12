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

#pragma once

#include "lib/cu_cp/cell_meas_manager/cell_meas_manager_impl.h"
#include <gtest/gtest.h>

namespace srsran {
namespace srs_cu_cp {

class dummy_mobility_manager : public cell_meas_mobility_manager_notifier
{
public:
  void on_neighbor_better_than_spcell(ue_index_t ue_index, pci_t neighbor_pci) override
  {
    fmt::print("on_neighbor_better_than_spcell() called.\n");
  }
};

/// Fixture class to create cell meas manager object.
class cell_meas_manager_test : public ::testing::Test
{
protected:
  cell_meas_manager_test();
  ~cell_meas_manager_test() override;

  void create_empty_manager();
  void create_default_manager();
  void create_default_manager_with_cell_params();
  void create_manager_without_ncells_and_periodic_report();
  void check_default_meas_cfg(const optional<rrc_meas_cfg>& meas_cfg, meas_obj_id_t meas_obj_id);
  void verify_meas_cfg(const optional<rrc_meas_cfg>& meas_cfg);
  void verify_empty_meas_cfg(const optional<rrc_meas_cfg>& meas_cfg);

  srslog::basic_logger& test_logger  = srslog::fetch_basic_logger("TEST");
  srslog::basic_logger& cu_cp_logger = srslog::fetch_basic_logger("CU-CP", false);

  std::unique_ptr<cell_meas_manager> manager;
  dummy_mobility_manager             mobility_manager;
};

} // namespace srs_cu_cp
} // namespace srsran
