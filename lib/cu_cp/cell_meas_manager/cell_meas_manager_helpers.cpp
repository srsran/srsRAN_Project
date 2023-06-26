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
