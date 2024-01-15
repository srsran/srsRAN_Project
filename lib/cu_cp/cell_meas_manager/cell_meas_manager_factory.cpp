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

#include "cell_meas_manager_helpers.h"
#include "cell_meas_manager_impl.h"
#include "srsran/cu_cp/cell_meas_manager.h"

using namespace srsran;

std::unique_ptr<srs_cu_cp::cell_meas_manager>
srsran::srs_cu_cp::create_cell_meas_manager(const srs_cu_cp::cell_meas_manager_cfg& cfg_,
                                            cell_meas_mobility_manager_notifier&    mobility_mng_notfier_)
{
  srsran_assert(is_valid_configuration(cfg_), "Cell measurement manager configuration is invalid");
  return std::make_unique<srs_cu_cp::cell_meas_manager_impl>(cfg_, mobility_mng_notfier_);
}