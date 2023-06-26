/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cell_meas_manager_helpers.h"
#include "cell_meas_manager_impl.h"
#include "srsran/cu_cp/cell_meas_manager.h"

using namespace srsran;

std::unique_ptr<srs_cu_cp::cell_meas_manager>
srsran::srs_cu_cp::create_cell_meas_manager(const srs_cu_cp::cell_meas_manager_cfg& cfg_)
{
  srsran_assert(is_valid_configuration(cfg_) == true, "Cell measurement manager configuration is invalid");
  return std::make_unique<srs_cu_cp::cell_meas_manager_impl>(cfg_);
}