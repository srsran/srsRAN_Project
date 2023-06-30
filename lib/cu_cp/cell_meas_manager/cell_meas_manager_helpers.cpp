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

#include "cell_meas_manager_helpers.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace srs_cu_cp;

#define LOG_CHAN ("CU-CP")

bool srsran::srs_cu_cp::is_valid_configuration(const cell_meas_manager_cfg& cfg)
{
  // Verify neighbor cell lists: cell id must not be included in neighbor cell list.
  for (const auto& cell : cfg.neighbor_cell_list) {
    const auto& cid = cell.first;
    for (const auto& neighbor_cell_cid : cell.second) {
      if (cid == neighbor_cell_cid) {
        srslog::fetch_basic_logger(LOG_CHAN).error("Cell {} must not be its own neighbor.", cid);
        return false;
      }
    }
  }

  if (not cfg.neighbor_cell_list.empty()) {
    // At least one event shall be configured.
    if (not cfg.a3_event_config.has_value()) {
      srslog::fetch_basic_logger(LOG_CHAN).error("At least one event must be configured.");
      return false;
    }
  }

  return true;
}
