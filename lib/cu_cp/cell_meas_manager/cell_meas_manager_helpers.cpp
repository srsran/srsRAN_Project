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

void srsran::srs_cu_cp::log_cells(const srslog::basic_logger& logger, const cell_meas_manager_cfg& cfg)
{
  logger.debug("Configured cells:");
  for (const auto& cell : cfg.cells) {
    logger.debug(" - {}", cell.second);
  }
}

bool srsran::srs_cu_cp::is_complete(const cell_meas_cfg& cfg)
{
  // All mendatory values must be present.
  if (!cfg.band.has_value() || !cfg.ssb_mtc.has_value() || !cfg.ssb_arfcn.has_value() || !cfg.ssb_scs.has_value()) {
    return false;
  }
  // Call validators of individual params.
  if (!is_scs_valid(cfg.ssb_scs.value())) {
    return false;
  }

  error_type<std::string> ret =
      band_helper::is_dl_arfcn_valid_given_band(cfg.band.value(), cfg.ssb_arfcn.value(), cfg.ssb_scs.value());
  if (ret.is_error()) {
    srslog::fetch_basic_logger(LOG_CHAN).error(
        "Invalid SSB ARFCN={} for band {}. Cause: {}.", cfg.ssb_arfcn.value(), cfg.band.value(), ret.error());
    return false;
  }

  return true;
}

bool srsran::srs_cu_cp::is_valid_configuration(const cell_meas_manager_cfg& cfg)
{
  // Verify neighbor cell lists: cell id must not be included in neighbor cell list.
  for (const auto& cell : cfg.cells) {
    const auto& nci = cell.first;
    for (const auto& ncell_nci : cell.second.ncells) {
      if (nci == ncell_nci) {
        srslog::fetch_basic_logger(LOG_CHAN).error("Cell {} must not be its own neighbor.", nci);
        return false;
      }
    }
  }

  if (not cfg.cells.empty()) {
    // At least one event shall be configured.
    if (not cfg.a3_event_config.has_value()) {
      srslog::fetch_basic_logger(LOG_CHAN).error("At least one event must be configured.");
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
    for (const auto& ncell_nci : cell.second.ncells) {
      // Verify NCI is present.
      if (cfg.cells.find(ncell_nci) == cfg.cells.end()) {
        srslog::fetch_basic_logger(LOG_CHAN).error("No config for cell id {} found.", ncell_nci);
        return false;
      }

      // Verify the config for this cell is complete.
      if (!is_complete(cfg.cells.at(ncell_nci))) {
        srslog::fetch_basic_logger(LOG_CHAN).error("Measurement config for cell id {} is not complete.", ncell_nci);
        return false;
      }
    }
  }

  return true;
}
