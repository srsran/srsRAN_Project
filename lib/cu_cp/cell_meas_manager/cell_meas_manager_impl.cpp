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

#include "cell_meas_manager_impl.h"
#include "cell_meas_manager_helpers.h"
#include "srsran/cu_cp/cell_meas_manager_config.h"
#include "srsran/rrc/meas_types.h"
#include "srsran/support/compiler.h"
#include "srsran/support/srsran_assert.h"
#include <utility>

using namespace srsran;
using namespace srs_cu_cp;

cell_meas_manager::cell_meas_manager(const cell_meas_manager_cfg&         cfg_,
                                     cell_meas_mobility_manager_notifier& mobility_mng_notifier_,
                                     ue_manager&                          ue_mng_) :
  cfg(cfg_), mobility_mng_notifier(mobility_mng_notifier_), ue_mng(ue_mng_), logger(srslog::fetch_basic_logger("CU-CP"))
{
  srsran_assert(is_valid_configuration(cfg, ssb_freq_to_meas_object), "Invalid cell measurement configuration");
  generate_measurement_objects_for_serving_cells();
  log_cells(logger, cfg);
}

std::optional<rrc_meas_cfg>
cell_meas_manager::get_measurement_config(ue_index_t                         ue_index,
                                          nr_cell_identity                   serving_nci,
                                          const std::optional<rrc_meas_cfg>& current_meas_config)
{
  std::optional<rrc_meas_cfg> meas_cfg;

  // Find cell.
  if (cfg.cells.find(serving_nci) == cfg.cells.end()) {
    logger.debug("Couldn't find cell config for nci={:#x}", serving_nci);
    return meas_cfg;
  }
  const auto& cell_config = cfg.cells.at(serving_nci);

  // Measurement config is only generated if serving cell config is complete
  if (!is_complete(cell_config.serving_cell_cfg)) {
    logger.debug("ue={}: Serving cell config is incomplete for nci={:#x}", ue_index, serving_nci);
    return meas_cfg;
  }

  // Measurement config is only generated if neighbor cells are configured or serving cell measurements are enabled.
  if (cell_config.ncells.empty() and !cell_config.periodic_report_cfg_id.has_value()) {
    logger.debug("ue={}: No neighbor cells configured and periodic serving cell reports disabled for nci={:#x}",
                 ue_index,
                 serving_nci);
    return meas_cfg;
  }

  auto& ue_meas_context = ue_mng.get_measurement_context(ue_index);

  // Create fresh config.
  meas_cfg.emplace();
  auto& new_cfg = meas_cfg.value();

  // Request removal of old measurement config if it exists.
  if (current_meas_config.has_value()) {
    add_old_meas_config_to_rem_list(current_meas_config.value(), new_cfg);
  }

  // Clear existing measurement object ids and measurement ids in UE context
  ue_meas_context.clear_meas_obj_ids();
  ue_meas_context.clear_meas_ids();

  // TODO: Filter measurement objects using UE capabilities.

  for (const auto& ssb_freq : generate_measurement_object_list(cfg, serving_nci)) {
    // Add measurement object
    rrc_meas_obj_to_add_mod meas_obj_to_add;
    meas_obj_to_add.meas_obj_id = ue_meas_context.allocate_meas_obj_id();
    meas_obj_to_add.meas_obj_nr = ssb_freq_to_meas_object.at(ssb_freq);
    new_cfg.meas_obj_to_add_mod_list.push_back(meas_obj_to_add);

    // add meas obj id to lookup
    for (const auto& nci : ssb_freq_to_ncis.at(ssb_freq)) {
      ue_meas_context.nci_to_meas_obj_id.emplace(nci, meas_obj_to_add.meas_obj_id);
    }

    if (cell_config.serving_cell_cfg.ssb_arfcn.value() == ssb_freq && cell_config.periodic_report_cfg_id.has_value()) {
      logger.debug("ue={}: Adding periodic report config for nci={:#x}", ue_index, serving_nci);
      generate_report_config(cfg, serving_nci, cell_config.periodic_report_cfg_id.value(), new_cfg, ue_meas_context);
    }

    for (const auto& ncell : cell_config.ncells) {
      if (is_complete(cfg.cells.at(ncell.nci).serving_cell_cfg) &&
          cfg.cells.at(ncell.nci).serving_cell_cfg.ssb_arfcn.value() == ssb_freq) {
        logger.debug("ue={}: Adding neighbor cell nci={:#x} to measurement config", ue_index, ncell.nci);
        for (const auto& report_cfg_id : ncell.report_cfg_ids) {
          generate_report_config(cfg, ncell.nci, report_cfg_id, new_cfg, ue_meas_context);
        }
      }
    }
  }

  // Ensure reports are not repeated.
  auto cmp_id = [](const rrc_report_cfg_to_add_mod& lhs, const rrc_report_cfg_to_add_mod& rhs) {
    return lhs.report_cfg_id < rhs.report_cfg_id;
  };
  std::sort(new_cfg.report_cfg_to_add_mod_list.begin(), new_cfg.report_cfg_to_add_mod_list.end(), cmp_id);
  auto eq_id = [](const rrc_report_cfg_to_add_mod& lhs, const rrc_report_cfg_to_add_mod& rhs) {
    return lhs.report_cfg_id == rhs.report_cfg_id;
  };
  auto end_it =
      std::unique(new_cfg.report_cfg_to_add_mod_list.begin(), new_cfg.report_cfg_to_add_mod_list.end(), eq_id);
  new_cfg.report_cfg_to_add_mod_list.erase(end_it, new_cfg.report_cfg_to_add_mod_list.end());
  std::sort(new_cfg.report_cfg_to_add_mod_list.begin(), new_cfg.report_cfg_to_add_mod_list.end(), cmp_id);

  // Add quantity config.
  rrc_quant_cfg    quant_cfg;
  rrc_quant_cfg_nr quant_cfg_nr;
  quant_cfg_nr.quant_cfg_cell.ssb_filt_cfg.filt_coef_rsrp    = 6; // TODO: remove hardcoded values
  quant_cfg_nr.quant_cfg_cell.csi_rs_filt_cfg.filt_coef_rsrp = 6; // TODO: remove hardcoded values
  quant_cfg.quant_cfg_nr_list.push_back(quant_cfg_nr);

  new_cfg.quant_cfg = quant_cfg;

  return meas_cfg;
}

std::optional<cell_meas_config> cell_meas_manager::get_cell_config(nr_cell_identity nci)
{
  std::optional<cell_meas_config> cell_cfg;
  if (cfg.cells.find(nci) != cfg.cells.end()) {
    cell_cfg = cfg.cells.at(nci);
  }
  return cell_cfg;
}

bool cell_meas_manager::update_cell_config(nr_cell_identity nci, const serving_cell_meas_config& serv_cell_cfg)
{
  // Store old config to revert if new config is invalid.
  cell_meas_manager_cfg tmp_cfg = cfg;

  if (cfg.cells.find(nci) == cfg.cells.end()) {
    logger.debug("No configuration to update for nci={:#x}. Adding configuration", nci);

    cell_meas_config meas_cfg;
    meas_cfg.serving_cell_cfg = serv_cell_cfg;

    cfg.cells.emplace(nci, meas_cfg);
  } else {
    logger.debug("Updating measurement configuration for nci={:#x}", nci);

    // Update serving cell config
    cfg.cells.at(nci).serving_cell_cfg = serv_cell_cfg;
  }

  if (!is_valid_configuration(cfg, ssb_freq_to_meas_object)) {
    logger.warning("Invalid cell measurement configuration");
    cfg = tmp_cfg;
    return false;
  }

  if (!is_complete(serv_cell_cfg)) {
    logger.debug("Added/Updated incomplete cell measurement configuration for nci={:#x}", nci);
  } else {
    // Only update measurement object if the configuration is complete.
    update_measurement_object(nci, serv_cell_cfg);
    log_meas_objects(logger, ssb_freq_to_meas_object);
  }

  log_cells(logger, cfg);

  return true;
}

static std::optional<uint8_t> get_ssb_rsrp(const rrc_meas_result_nr& meas_result)
{
  std::optional<uint8_t> rsrp;
  if (meas_result.cell_results.results_ssb_cell.has_value()) {
    if (meas_result.cell_results.results_ssb_cell.value().rsrp.has_value()) {
      rsrp = meas_result.cell_results.results_ssb_cell.value().rsrp.value();
    }
  }
  return rsrp;
}

static std::optional<pci_t> find_strongest_neighbor(ue_index_t              ue_index,
                                                    const rrc_meas_results& meas_results,
                                                    srslog::basic_logger&   logger,
                                                    std::optional<uint8_t>  periodic_ho_rsrp_offset = std::nullopt)
{
  std::optional<pci_t> strongest_neighbor;
  // Find strongest neighbor cell
  if (meas_results.meas_result_neigh_cells.has_value()) {
    // Find strongest neighbor here.
    std::optional<uint8_t> serv_cell_rsrp;

    // Extract RSRP of SSB for servCellId 0.
    if (meas_results.meas_result_serving_mo_list.contains(0)) {
      serv_cell_rsrp = get_ssb_rsrp(meas_results.meas_result_serving_mo_list[0].meas_result_serving_cell);
    }

    if (serv_cell_rsrp.has_value()) {
      uint8_t max_rsrp = serv_cell_rsrp.value();

      for (const auto& report : meas_results.meas_result_neigh_cells.value().meas_result_list_nr) {
        std::optional<uint8_t> neighbor_rsrp = get_ssb_rsrp(report);
        if (neighbor_rsrp.has_value()) {
          if (neighbor_rsrp.value() > max_rsrp + periodic_ho_rsrp_offset.value_or(0)) {
            // Found stronger neighbor, take note of it's details.
            max_rsrp           = neighbor_rsrp.value();
            strongest_neighbor = report.pci;
          }
        }
      }

      if (strongest_neighbor.has_value()) {
        logger.info("ue={}: Neighbor PCI={} (ssb_rsrp={}) stronger than current serving cell (ssb_rsrp={})",
                    ue_index,
                    strongest_neighbor.value(),
                    max_rsrp,
                    serv_cell_rsrp.value());
      }
    }
  }
  return strongest_neighbor;
}

void cell_meas_manager::report_measurement(ue_index_t ue_index, const rrc_meas_results& meas_results)
{
  logger.debug("ue={}: Received measurement result with meas_id={}", ue_index, fmt::underlying(meas_results.meas_id));

  auto& ue_meas_context = ue_mng.get_measurement_context(ue_index);

  // Verify meas_id is valid.
  if (ue_meas_context.meas_id_to_meas_context.find(meas_results.meas_id) ==
      ue_meas_context.meas_id_to_meas_context.end()) {
    logger.debug(
        "ue={}: Measurement result for unknown meas_id={} received", ue_index, fmt::underlying(meas_results.meas_id));
    return;
  }

  // Store measurement results.
  store_measurement_results(ue_index, meas_results);

  auto& meas_ctxt = ue_meas_context.meas_id_to_meas_context.at(meas_results.meas_id);

  // Handle periodic measurement results.
  if (cfg.cells.at(meas_ctxt.nci).periodic_report_cfg_id.has_value() &&
      cfg.cells.at(meas_ctxt.nci).periodic_report_cfg_id.value() == meas_ctxt.report_cfg_id) {
    uint8_t periodic_ho_rsrp_offset = 0;
    if (const auto* periodical = std::get_if<rrc_periodical_report_cfg>(
            &cfg.report_config_ids.at(cfg.cells.at(meas_ctxt.nci).periodic_report_cfg_id.value()));
        periodical != nullptr) {
      if (periodical->periodic_ho_rsrp_offset == -1) {
        logger.debug("ue={}: Handover from periodic measurements is disabled", ue_index);
        return;
      }
      periodic_ho_rsrp_offset = uint8_t(periodical->periodic_ho_rsrp_offset);
    }

    // Find strongest neighbor cell.
    std::optional<pci_t> strongest_neighbor =
        find_strongest_neighbor(ue_index, meas_results, logger, periodic_ho_rsrp_offset);
    if (strongest_neighbor.has_value()) {
      for (const auto& ncell : cfg.cells.at(meas_ctxt.nci).ncells) {
        const cell_meas_config& ncell_cfg = cfg.cells.at(ncell.nci);
        if (ncell_cfg.serving_cell_cfg.pci.has_value() &&
            ncell_cfg.serving_cell_cfg.pci.value() == strongest_neighbor.value()) {
          // Report cell.
          mobility_mng_notifier.on_neighbor_better_than_spcell(
              ue_index,
              ncell_cfg.serving_cell_cfg.nci.gnb_id(ncell_cfg.serving_cell_cfg.gnb_id_bit_length),
              ncell_cfg.serving_cell_cfg.nci,
              strongest_neighbor.value());
          return;
        }
      }
    }
  } else {
    // For meas_id with e.g. A3 event configured:

    // Iterate through serving cell results and check if best neighbor is reported.
    for (const auto& serv_cell : meas_results.meas_result_serving_mo_list) {
      if (serv_cell.meas_result_best_neigh_cell.has_value()) {
        // Report this cell.
        if (serv_cell.meas_result_best_neigh_cell.value().pci.has_value()) {
          mobility_mng_notifier.on_neighbor_better_than_spcell(
              ue_index,
              meas_ctxt.nci.gnb_id(meas_ctxt.gnb_id_bit_length),
              meas_ctxt.nci,
              serv_cell.meas_result_best_neigh_cell.value().pci.value());
          return;
        }
      }
    }

    // Find strongest neighbor cell.
    std::optional<pci_t> strongest_neighbor = find_strongest_neighbor(ue_index, meas_results, logger);
    if (strongest_neighbor.has_value()) {
      // Report cell.
      mobility_mng_notifier.on_neighbor_better_than_spcell(
          ue_index, meas_ctxt.nci.gnb_id(meas_ctxt.gnb_id_bit_length), meas_ctxt.nci, strongest_neighbor.value());
      return;
    }
  }
}

void cell_meas_manager::generate_measurement_objects_for_serving_cells()
{
  for (const auto& cell : cfg.cells) {
    if (is_complete(cell.second.serving_cell_cfg)) {
      update_measurement_object(cell.first, cell.second.serving_cell_cfg);
    }
  }

  log_meas_objects(logger, ssb_freq_to_meas_object);
}

void cell_meas_manager::update_measurement_object(nr_cell_identity                nci,
                                                  const serving_cell_meas_config& serving_cell_cfg)
{
  srsran_assert(is_complete(serving_cell_cfg), "Incomplete measurement object update for nci={:#x}", nci);

  ssb_frequency_t ssb_freq = serving_cell_cfg.ssb_arfcn.value();

  // Add to lookup
  if (ssb_freq_to_ncis.find(ssb_freq) != ssb_freq_to_ncis.end()) {
    ssb_freq_to_ncis.at(ssb_freq).push_back(nci);
  } else {
    ssb_freq_to_ncis.emplace(ssb_freq, std::vector<nr_cell_identity>{nci});
  }
  nci_to_serving_cell_meas_config.emplace(serving_cell_cfg.nci, serving_cell_cfg);

  if (ssb_freq_to_meas_object.find(ssb_freq) != ssb_freq_to_meas_object.end()) {
    // If the measurement object is already present, we ignore the duplicate.
    logger.debug("Measurement object for ssb_freq={} already exists", ssb_freq);
    return;
  }
  ssb_freq_to_meas_object.emplace(ssb_freq, generate_measurement_object(serving_cell_cfg));
}

#ifndef SRSRAN_HAS_ENTERPRISE

void cell_meas_manager::store_measurement_results(ue_index_t ue_index, const rrc_meas_results& meas_results) {}

#endif // SRSRAN_HAS_ENTERPRISE
