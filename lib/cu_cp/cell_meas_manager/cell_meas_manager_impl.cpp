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

#include "cell_meas_manager_impl.h"
#include "cell_meas_manager_helpers.h"

using namespace srsran;
using namespace srs_cu_cp;

cell_meas_manager_impl::cell_meas_manager_impl(const cell_meas_manager_cfg&         cfg_,
                                               cell_meas_mobility_manager_notifier& mobility_mng_notifier_) :
  cfg(cfg_), mobility_mng_notifier(mobility_mng_notifier_), logger(srslog::fetch_basic_logger("CU-CP"))
{
  srsran_assert(is_valid_configuration(cfg), "Invalid cell measurement configuration");
  log_cells(logger, cfg);

  // Mark zero index as occupied as the first valid meas(-obj) ID is 1.
  meas_ids.emplace(0);
  meas_obj_ids.emplace(0);
}

optional<rrc_meas_cfg> cell_meas_manager_impl::get_measurement_config(nr_cell_id_t           serving_nci,
                                                                      optional<rrc_meas_cfg> current_meas_config)
{
  optional<rrc_meas_cfg> meas_cfg;

  // Find cell.
  if (cfg.cells.find(serving_nci) == cfg.cells.end()) {
    logger.debug("Couldn't find cell config for nci={}", serving_nci);
    return meas_cfg;
  }
  const auto& cell_config = cfg.cells.at(serving_nci);

  // Measurement config is only generated if neighbor cells are configured or serving cell measurements are enabled.
  if (cell_config.ncells.empty() and !cell_config.periodic_report_cfg_id.has_value()) {
    logger.debug("No neighbor cells configured and periodic serving cell reports disabled for nci={}", serving_nci);
    return meas_cfg;
  }

  // Create fresh config.
  meas_cfg.emplace();
  auto& new_cfg = meas_cfg.value();

  if (current_meas_config.has_value()) {
    // Remove meas objects and configs.
    const auto& old_cfg = current_meas_config.value();

    // Remove measurement objects.
    for (const auto& meas_obj : old_cfg.meas_obj_to_add_mod_list) {
      logger.debug("Removing meas_obj_id={}", meas_obj.meas_obj_id);
      meas_obj_ids.erase(meas_obj_id_to_uint(meas_obj.meas_obj_id));
      new_cfg.meas_obj_to_rem_list.push_back(meas_obj.meas_obj_id);
    }

    // Remove measurement IDs.
    for (const auto& meas_id : old_cfg.meas_id_to_add_mod_list) {
      logger.debug("Removing meas_id={}", meas_id.meas_id);
      meas_ids.erase(meas_id_to_uint(meas_id.meas_id));
      new_cfg.meas_id_to_rem_list.push_back(meas_id.meas_id);
    }

    // Remove active reports.
    for (const auto& report : old_cfg.report_cfg_to_add_mod_list) {
      new_cfg.report_cfg_to_rem_list.push_back(report.report_cfg_id);
    }
  }

  // Create meas object for each neighbor cell
  for (const auto& ncell : cell_config.ncells) {
    // Verify we have a complete config for this cell.
    if (!is_complete(cfg.cells.at(ncell.nci).serving_cell_cfg)) {
      logger.debug(
          "Cell {} is neighbor of {} but skipping due to missing measurement parameters", ncell.nci, serving_nci);
      continue;
    }
    srsran_assert(cfg.cells.find(ncell.nci) != cfg.cells.end(), "Cell id {} not found in list", ncell.nci);
    const auto& meas_cell_config = cfg.cells.at(ncell.nci);

    // add meas obj to add mod
    rrc_meas_obj_to_add_mod meas_obj;
    meas_obj.meas_obj_id = get_next_meas_obj_id();
    meas_obj.meas_obj_nr.emplace();
    auto& meas_obj_nr                  = meas_obj.meas_obj_nr.value();
    meas_obj_nr.ssb_freq               = meas_cell_config.serving_cell_cfg.ssb_arfcn;
    meas_obj_nr.ssb_subcarrier_spacing = meas_cell_config.serving_cell_cfg.ssb_scs;
    meas_obj_nr.smtc1                  = meas_cell_config.serving_cell_cfg.ssb_mtc;

    // Mandatory fields.
    meas_obj_nr.ref_sig_cfg.ssb_cfg_mob.emplace().derive_ssb_idx_from_cell = true;
    meas_obj_nr.nrof_ss_blocks_to_average.emplace()                        = 8;
    meas_obj_nr.quant_cfg_idx                                              = 1;
    meas_obj_nr.freq_band_ind_nr.emplace() = nr_band_to_uint(meas_cell_config.serving_cell_cfg.band.value());

    new_cfg.meas_obj_to_add_mod_list.push_back(meas_obj);

    // add meas obj id to lookup
    meas_obj_id_to_nci.emplace(meas_obj.meas_obj_id, ncell.nci);

    // add report cfg to add mod
    for (const auto& report_cfg_id : ncell.report_cfg_ids) {
      rrc_report_cfg_to_add_mod report_cfg_to_add_mod;
      report_cfg_to_add_mod.report_cfg_id = report_cfg_id;
      report_cfg_to_add_mod.report_cfg    = cfg.report_config_ids.at(report_cfg_id);

      new_cfg.report_cfg_to_add_mod_list.push_back(report_cfg_to_add_mod);

      // Add meas id to link the neighbor cell and the report together.
      rrc_meas_id_to_add_mod meas_id_to_add_mod;

      meas_id_to_add_mod.meas_id       = get_next_meas_id();
      meas_id_to_add_mod.meas_obj_id   = meas_obj.meas_obj_id;
      meas_id_to_add_mod.report_cfg_id = report_cfg_id;

      new_cfg.meas_id_to_add_mod_list.push_back(meas_id_to_add_mod);

      // add meas id to lookup
      meas_id_to_meas_context.emplace(meas_id_to_add_mod.meas_id,
                                      meas_context_t{meas_id_to_add_mod.meas_obj_id, meas_id_to_add_mod.report_cfg_id});
    }
  }

  // Add periodic report configuration for the serving cell.
  // Create meas object for serving cell if periodic report cfg is set
  if (cell_config.periodic_report_cfg_id.has_value()) {
    if (meas_obj_id_to_nci.size() == MAX_NOF_MEAS_OBJ) {
      logger.warning("Can't add periodical report config for serving cell. Maximum ({}) reached", MAX_NOF_MEAS_OBJ);
    } else {
      // add meas obj to add mod
      rrc_meas_obj_to_add_mod meas_obj;
      meas_obj.meas_obj_id               = get_next_meas_obj_id();
      auto& meas_obj_nr                  = meas_obj.meas_obj_nr.emplace();
      meas_obj_nr.ssb_freq               = cell_config.serving_cell_cfg.ssb_arfcn;
      meas_obj_nr.ssb_subcarrier_spacing = cell_config.serving_cell_cfg.ssb_scs;
      meas_obj_nr.smtc1                  = cell_config.serving_cell_cfg.ssb_mtc;

      // Mandatory fields.
      meas_obj_nr.ref_sig_cfg.ssb_cfg_mob.emplace().derive_ssb_idx_from_cell = true;
      meas_obj_nr.nrof_ss_blocks_to_average.emplace()                        = 8;
      meas_obj_nr.quant_cfg_idx                                              = 1;
      meas_obj_nr.freq_band_ind_nr.emplace() = nr_band_to_uint(cell_config.serving_cell_cfg.band.value());

      new_cfg.meas_obj_to_add_mod_list.push_back(meas_obj);

      // add meas obj id to lookup
      meas_obj_id_to_nci.emplace(meas_obj.meas_obj_id, serving_nci);

      // add report cfg to add mod
      rrc_report_cfg_to_add_mod report_cfg_to_add_mod;
      report_cfg_to_add_mod.report_cfg_id = cell_config.periodic_report_cfg_id.value();
      report_cfg_to_add_mod.report_cfg    = cfg.report_config_ids.at(cell_config.periodic_report_cfg_id.value());

      new_cfg.report_cfg_to_add_mod_list.push_back(report_cfg_to_add_mod);

      // Add meas id to link the neighbor cell and the report together.
      rrc_meas_id_to_add_mod meas_id_to_add_mod;

      meas_id_to_add_mod.meas_id       = get_next_meas_id();
      meas_id_to_add_mod.meas_obj_id   = meas_obj.meas_obj_id;
      meas_id_to_add_mod.report_cfg_id = cell_config.periodic_report_cfg_id.value();

      new_cfg.meas_id_to_add_mod_list.push_back(meas_id_to_add_mod);

      // add meas id to lookup
      meas_id_to_meas_context.emplace(meas_id_to_add_mod.meas_id,
                                      meas_context_t{meas_id_to_add_mod.meas_obj_id, meas_id_to_add_mod.report_cfg_id});
    }
  }

  // Add quantity config.
  rrc_quant_cfg    quant_cfg;
  rrc_quant_cfg_nr quant_cfg_nr;
  quant_cfg_nr.quant_cfg_cell.ssb_filt_cfg.filt_coef_rsrp    = 6;
  quant_cfg_nr.quant_cfg_cell.csi_rs_filt_cfg.filt_coef_rsrp = 6;
  quant_cfg.quant_cfg_nr_list.push_back(quant_cfg_nr);

  new_cfg.quant_cfg = quant_cfg;

  return meas_cfg;
}

optional<cell_meas_config> cell_meas_manager_impl::get_cell_config(nr_cell_id_t nci)
{
  optional<cell_meas_config> cell_cfg;
  if (cfg.cells.find(nci) != cfg.cells.end()) {
    cell_cfg = cfg.cells.at(nci);
  }
  return cell_cfg;
}

void cell_meas_manager_impl::update_cell_config(nr_cell_id_t                           nci,
                                                const serving_cell_meas_config&        serv_cell_cfg_,
                                                std::vector<neighbor_cell_meas_config> ncells_)
{
  if (!is_complete(serv_cell_cfg_)) {
    logger.debug("Updating incomplete cell measurement configuration for nci={}", nci);
  }

  if (cfg.cells.find(nci) == cfg.cells.end()) {
    logger.debug("No configuration to update for nci={}. Adding configuration.", nci);

    cell_meas_config meas_cfg;
    meas_cfg.serving_cell_cfg = serv_cell_cfg_;
    meas_cfg.ncells           = ncells_;

    cfg.cells.emplace(nci, meas_cfg);
  } else {
    logger.debug("Updating measurement configuration for nci={}.", nci);

    // Update serving cell config
    cfg.cells.at(nci).serving_cell_cfg = serv_cell_cfg_;

    // Update neighbor cells
    if (!ncells_.empty()) {
      cfg.cells.at(nci).ncells = ncells_;
    }
  }

  log_cells(logger, cfg);
}

optional<uint8_t> get_ssb_rsrp(const rrc_meas_result_nr& meas_result)
{
  optional<uint8_t> rsrp;
  if (meas_result.cell_results.results_ssb_cell.has_value()) {
    if (meas_result.cell_results.results_ssb_cell.value().rsrp.has_value()) {
      rsrp = meas_result.cell_results.results_ssb_cell.value().rsrp.value();
    }
  }
  return rsrp;
}

void cell_meas_manager_impl::report_measurement(const ue_index_t ue_index, const rrc_meas_results& meas_results)
{
  logger.debug("ue={} Received measurement result with meas_id={}", ue_index, meas_results.meas_id);

  // Verify meas_id is valid.
  if (meas_id_to_meas_context.find(meas_results.meas_id) == meas_id_to_meas_context.end()) {
    logger.debug("ue={} Measurement result for unknown meas_id={} received.", ue_index, meas_results.meas_id);
    return;
  }

  // Ignore id with periodic measurements.

  // For meas_id with e.g. A3 event configured:

  // Iterate through serving cell results and check if best neighbor is reported.
  for (const auto& serv_cell : meas_results.meas_result_serving_mo_list) {
    if (serv_cell.meas_result_best_neigh_cell.has_value()) {
      // Report this cell.
      if (serv_cell.meas_result_best_neigh_cell.value().pci.has_value()) {
        mobility_mng_notifier.on_neighbor_better_than_spcell(ue_index,
                                                             serv_cell.meas_result_best_neigh_cell.value().pci.value());
        return;
      }
    }
  }

  // Find strongest neighbor cell
  if (meas_results.meas_result_neigh_cells.has_value()) {
    // Find strongest neighbor here.
    optional<uint8_t> serv_cell_rsrp;

    // Extract RSRP of SSB for servCellId 0.
    if (meas_results.meas_result_serving_mo_list.contains(0)) {
      serv_cell_rsrp = get_ssb_rsrp(meas_results.meas_result_serving_mo_list[0].meas_result_serving_cell);
    }

    if (serv_cell_rsrp.has_value()) {
      uint8_t         max_rsrp = serv_cell_rsrp.value();
      optional<pci_t> strongest_neighbor;

      for (const auto& report : meas_results.meas_result_neigh_cells.value().meas_result_list_nr) {
        optional<uint8_t> neighbor_rsrp = get_ssb_rsrp(report);
        if (neighbor_rsrp.has_value()) {
          if (neighbor_rsrp.value() > max_rsrp) {
            // Found stronger neighbor, take note of it's details.
            max_rsrp           = neighbor_rsrp.value();
            strongest_neighbor = report.pci;
          }
        }
      }

      if (strongest_neighbor.has_value()) {
        // Report cell.
        logger.info("Neighbor PCI={} (ssb_rsrp={}) stronger than current serving cell (ssb_rsrp={})",
                    strongest_neighbor.value(),
                    max_rsrp,
                    serv_cell_rsrp.value());
        mobility_mng_notifier.on_neighbor_better_than_spcell(ue_index, strongest_neighbor.value());
        return;
      }
    }
  }
}

/// \brief Get the next available meas id.
meas_id_t cell_meas_manager_impl::get_next_meas_id()
{
  // return invalid when no meas id is available
  if (meas_id_to_meas_context.size() == MAX_NOF_MEAS) {
    return meas_id_t::invalid;
  }

  meas_id_t new_meas_id = (meas_id_t)meas_ids.find_first_empty();
  if (new_meas_id == meas_id_t::max) {
    return meas_id_t::invalid;
  }
  meas_ids.emplace(meas_id_to_uint(new_meas_id));
  return new_meas_id;
}

/// \brief Get the next available meas obj id.
meas_obj_id_t cell_meas_manager_impl::get_next_meas_obj_id()
{
  // return invalid when no meas obj id is available
  if (meas_obj_id_to_nci.size() == MAX_NOF_MEAS_OBJ) {
    return meas_obj_id_t::invalid;
  }

  meas_obj_id_t new_meas_obj_id = (meas_obj_id_t)meas_obj_ids.find_first_empty();
  if (new_meas_obj_id == meas_obj_id_t::max) {
    return meas_obj_id_t::invalid;
  }
  meas_obj_ids.emplace(meas_obj_id_to_uint(new_meas_obj_id));
  return new_meas_obj_id;
}
