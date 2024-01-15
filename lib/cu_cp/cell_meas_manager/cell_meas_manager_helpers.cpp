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
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace srs_cu_cp;

#define LOG_CHAN ("CU-CP")

void srsran::srs_cu_cp::log_cells(const srslog::basic_logger& logger, const cell_meas_manager_cfg& cfg)
{
  if (!cfg.cells.empty()) {
    logger.debug("Configured cells:");
    for (const auto& cell : cfg.cells) {
      logger.debug(" - {}", cell.second);
    }
  }
}

bool srsran::srs_cu_cp::is_complete(const serving_cell_meas_config& cfg)
{
  // All mendatory values must be present.
  if (!cfg.band.has_value() || !cfg.ssb_mtc.has_value() || !cfg.ssb_arfcn.has_value() || !cfg.ssb_scs.has_value()) {
    return false;
  }
  // Call validators of individual params.
  if (!is_scs_valid(cfg.ssb_scs.value())) {
    return false;
  }

  // TODO: validate ssb arfcn
#ifdef SSB_ARFC_VALIDATOR
  error_type<std::string> ret =
      band_helper::is_dl_arfcn_valid_given_band(cfg.band.value(), cfg.ssb_arfcn.value(), cfg.ssb_scs.value());
  if (ret.is_error()) {
    srslog::fetch_basic_logger(LOG_CHAN).error(
        "Invalid SSB ARFCN={} for band {}. Cause: {}.", cfg.ssb_arfcn.value(), cfg.band.value(), ret.error());
    return false;
  }
#endif // SSB_ARFC_VALIDATOR

  return true;
}

bool srsran::srs_cu_cp::is_valid_configuration(const cell_meas_manager_cfg& cfg)
{
  bool serving_cell_periodic_reports_enabled = false;
  // Verify neighbor cell lists: cell id must not be included in neighbor cell list.
  for (const auto& cell : cfg.cells) {
    const auto& nci = cell.first;
    if (cell.second.periodic_report_cfg_id.has_value()) {
      serving_cell_periodic_reports_enabled = true;
    }
    for (const auto& ncell_nci : cell.second.ncells) {
      if (nci == ncell_nci.nci) {
        srslog::fetch_basic_logger(LOG_CHAN).error("Cell {} must not be its own neighbor.", nci);
        return false;
      }
    }
  }

  if (not cfg.cells.empty()) {
    // At least one event shall be configured.
    if (cfg.report_config_ids.empty()) {
      srslog::fetch_basic_logger(LOG_CHAN).error("At least one event must be configured.");
      return false;
    }

    if (serving_cell_periodic_reports_enabled && cfg.report_config_ids.size() < 2) {
      srslog::fetch_basic_logger(LOG_CHAN).error(
          "At least one event beside the periodical report config for the serving cell must be configured.");
      return false;
    }
  }

  return true;
}

bool srsran::srs_cu_cp::is_complete(const cell_meas_manager_cfg& cfg)
{
  if (!is_valid_configuration(cfg)) {
    return false;
  }

  // Verify each neighbor cell has a valid config.
  for (const auto& cell : cfg.cells) {
    for (const auto& ncell : cell.second.ncells) {
      // Verify NCI is present.
      if (cfg.cells.find(ncell.nci) == cfg.cells.end()) {
        srslog::fetch_basic_logger(LOG_CHAN).error("No config for cell id {} found.", ncell.nci);
        return false;
      }

      // Verify the config for this cell is complete.
      if (!is_complete(cfg.cells.at(ncell.nci).serving_cell_cfg)) {
        srslog::fetch_basic_logger(LOG_CHAN).error("Measurement config for cell id {} is not complete.", ncell.nci);
        return false;
      }
    }
  }

  return true;
}
