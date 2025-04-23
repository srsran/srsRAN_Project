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

#include "fapi_config_translator.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "fapi_config.h"

using namespace srsran;

void srsran::fill_fapi_worker_manager_config(worker_manager_config&  config,
                                             const fapi_unit_config& unit_cfg,
                                             unsigned                nof_cells)
{
  // No configuration for FAPI if there is no buffered module.
  if (unit_cfg.l2_nof_slots_ahead == 0) {
    return;
  }

  auto& fapi_cfg     = config.fapi_cfg.emplace();
  fapi_cfg.nof_cells = nof_cells;
}
