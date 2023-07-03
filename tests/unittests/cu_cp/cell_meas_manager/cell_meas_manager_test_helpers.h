/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_cp/cell_meas_manager.h"
#include <gtest/gtest.h>

namespace srsran {
namespace srs_cu_cp {

/// Fixture class to create cell meas manager object.
class cell_meas_manager_test : public ::testing::Test
{
protected:
  cell_meas_manager_test();
  ~cell_meas_manager_test() override;

  void create_empty_manager();
  void create_default_manager();
  void create_default_manager_with_cell_params();
  void check_default_meas_cfg(const rrc_meas_cfg& meas_cfg);
  void verify_empty_meas_cfg(const rrc_meas_cfg& meas_cfg);

  srslog::basic_logger& test_logger  = srslog::fetch_basic_logger("TEST");
  srslog::basic_logger& cu_cp_logger = srslog::fetch_basic_logger("CU-CP", false);

  std::unique_ptr<cell_meas_manager> manager;
};

} // namespace srs_cu_cp
} // namespace srsran
