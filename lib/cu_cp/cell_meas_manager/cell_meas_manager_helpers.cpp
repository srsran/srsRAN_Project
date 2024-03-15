/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
  if (!cfg.cells.empty()) {
    logger.debug("Configured cells:");
    for (const auto& cell : cfg.cells) {
      logger.debug(" - {}", cell.second);
    }
  }
}

bool srsran::srs_cu_cp::is_complete(const serving_cell_meas_config& cfg)
{
  // All mandatory values must be present.
  if (!cfg.pci.has_value() || !cfg.band.has_value() || !cfg.ssb_mtc.has_value() || !cfg.ssb_arfcn.has_value() ||
      !cfg.ssb_scs.has_value()) {
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
        "Invalid SSB ARFCN={} for band {}. Cause: {}", cfg.ssb_arfcn.value(), cfg.band.value(), ret.error());
    return false;
  }
#endif // SSB_ARFC_VALIDATOR

  return true;
}

bool srsran::srs_cu_cp::is_valid_configuration(const cell_meas_manager_cfg& cfg)
{
  bool                      serving_cell_periodic_reports_enabled = false;
  std::vector<nr_cell_id_t> ncis;
  // Verify neighbor cell lists: cell id must not be included in neighbor cell list.
  for (const auto& cell : cfg.cells) {
    const auto& nci = cell.first;
    if (std::find(ncis.begin(), ncis.end(), nci) != ncis.end()) {
      srslog::fetch_basic_logger(LOG_CHAN).error("Cell {} already present, but must be unique", nci);
      return false;
    }
    ncis.push_back(nci);

    if (cell.second.periodic_report_cfg_id.has_value()) {
      serving_cell_periodic_reports_enabled = true;
    }
    for (const auto& ncell_nci : cell.second.ncells) {
      if (nci == ncell_nci.nci) {
        srslog::fetch_basic_logger(LOG_CHAN).error("Cell {} must not be its own neighbor", nci);
        return false;
      }
    }
  }

  if (not cfg.cells.empty()) {
    // At least one event shall be configured.
    if (cfg.report_config_ids.empty()) {
      srslog::fetch_basic_logger(LOG_CHAN).error("At least one event must be configured");
      return false;
    }

    if (serving_cell_periodic_reports_enabled && cfg.report_config_ids.size() < 2) {
      srslog::fetch_basic_logger(LOG_CHAN).error(
          "At least one event beside the periodical report config for the serving cell must be configured");
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
        srslog::fetch_basic_logger(LOG_CHAN).error("No config for cell id {} found", ncell.nci);
        return false;
      }

      // Verify the config for this cell is complete.
      if (!is_complete(cfg.cells.at(ncell.nci).serving_cell_cfg)) {
        srslog::fetch_basic_logger(LOG_CHAN).error("Measurement config for cell id {} is not complete", ncell.nci);
        return false;
      }
    }
  }

  return true;
}

void srsran::srs_cu_cp::add_old_meas_config_to_rem_list(const rrc_meas_cfg& old_cfg, rrc_meas_cfg& new_cfg)
{
  // Remove measurement objects.
  for (const auto& meas_obj : old_cfg.meas_obj_to_add_mod_list) {
    new_cfg.meas_obj_to_rem_list.push_back(meas_obj.meas_obj_id);
  }

  // Remove measurement IDs.
  for (const auto& meas_id : old_cfg.meas_id_to_add_mod_list) {
    new_cfg.meas_id_to_rem_list.push_back(meas_id.meas_id);
  }

  // Remove active reports.
  for (const auto& report : old_cfg.report_cfg_to_add_mod_list) {
    new_cfg.report_cfg_to_rem_list.push_back(report.report_cfg_id);
  }
}

std::vector<ssb_frequency_t> srsran::srs_cu_cp::generate_measurement_object_list(const cell_meas_manager_cfg& cfg,
                                                                                 nr_cell_id_t serving_nci)
{
  srsran_assert(cfg.cells.find(serving_nci) != cfg.cells.end(), "No cell config for nci={:#x}", serving_nci);

  // Add cells to lookup if report is configured
  std::vector<ssb_frequency_t> ssb_freqs;
  // Add serving cell if periodic report is configured
  auto& serving_cell = cfg.cells.at(serving_nci);
  if (serving_cell.periodic_report_cfg_id.has_value() && is_complete(serving_cell.serving_cell_cfg)) {
    ssb_freqs.push_back(serving_cell.serving_cell_cfg.ssb_arfcn.value());
  }
  // Add neighbor cells if report is configured
  for (const auto& ncell : serving_cell.ncells) {
    auto& cell_cfg = cfg.cells.at(ncell.nci);
    if (!ncell.report_cfg_ids.empty() && is_complete(cell_cfg.serving_cell_cfg)) {
      if (std::find(ssb_freqs.begin(), ssb_freqs.end(), cell_cfg.serving_cell_cfg.ssb_arfcn.value()) ==
          ssb_freqs.end()) {
        ssb_freqs.push_back(cell_cfg.serving_cell_cfg.ssb_arfcn.value());
      }
    }
  }

  return ssb_freqs;
}

void srsran::srs_cu_cp::generate_report_config(const cell_meas_manager_cfg&  cfg,
                                               const nr_cell_id_t            nci,
                                               const report_cfg_id_t         report_cfg_id,
                                               rrc_meas_cfg&                 meas_cfg,
                                               cell_meas_manager_ue_context& ue_meas_context)
{
  // add report cfg to add mod
  rrc_report_cfg_to_add_mod report_cfg_to_add_mod;
  report_cfg_to_add_mod.report_cfg_id = report_cfg_id;
  report_cfg_to_add_mod.report_cfg    = cfg.report_config_ids.at(report_cfg_id);
  meas_cfg.report_cfg_to_add_mod_list.push_back(report_cfg_to_add_mod);

  // Add meas id to link the cell and the report together.
  rrc_meas_id_to_add_mod meas_id_to_add_mod;
  meas_id_to_add_mod.meas_id       = ue_meas_context.allocate_meas_id();
  meas_id_to_add_mod.meas_obj_id   = ue_meas_context.nci_to_meas_obj_id.at(nci);
  meas_id_to_add_mod.report_cfg_id = report_cfg_id;
  meas_cfg.meas_id_to_add_mod_list.push_back(meas_id_to_add_mod);

  // add meas id to lookup
  ue_meas_context.meas_id_to_meas_context.emplace(
      meas_id_to_add_mod.meas_id, meas_context_t{meas_id_to_add_mod.meas_obj_id, meas_id_to_add_mod.report_cfg_id});
}

bool srsran::srs_cu_cp::is_duplicate(const rrc_meas_obj_nr& obj_1, const rrc_meas_obj_nr& obj_2)
{
  // TS 38.331 section 5.5.2.1:
  // For all SSB based measurements there is at most one measurement object with
  // the same ssbFrequency;
  // - an smtc1 included in any measurement object with the same ssbFrequency has the
  //   same value and that an smtc2 included in any measurement object with the
  //   same ssbFrequency has the same value;
  // - to ensure that all measurement objects configured in this specification and in
  //   TS 36.331 [10] with the same ssbFrequency have the same ssbSubcarrierSpacing;
  return obj_1.ssb_freq == obj_2.ssb_freq && obj_1.ssb_subcarrier_spacing == obj_2.ssb_subcarrier_spacing &&
         obj_1.smtc1 == obj_2.smtc1 && obj_1.smtc2 == obj_2.smtc2;
}

void srsran::srs_cu_cp::log_meas_objects(const srslog::basic_logger&                                 logger,
                                         const std::unordered_map<ssb_frequency_t, rrc_meas_obj_nr>& meas_objects)
{
  if (!meas_objects.empty()) {
    logger.debug("Measurement objects:");
    for (const auto& meas_obj : meas_objects) {
      logger.debug(" - ssb_freq={}: {}", meas_obj.first, meas_obj.second);
    }
  }
}
