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
}

optional<rrc_meas_cfg> cell_meas_manager_impl::get_measurement_config(nr_cell_id_t serving_nci)
{
  optional<rrc_meas_cfg> meas_cfg;

  // Find cell.
  if (cfg.cells.find(serving_nci) == cfg.cells.end()) {
    logger.debug("Couldn't find cell config for nci={}", serving_nci);
    return meas_cfg;
  }
  const auto& cell_config = cfg.cells.at(serving_nci);

  // Create fresh config.
  meas_cfg.emplace();

  // Create meas object for each neighbor cell
  unsigned id_counter = 1;
  for (const auto& ncell_nci : cell_config.ncells) {
    // Verify we have a complete config for this cell.
    if (!is_complete(cfg.cells.at(ncell_nci).serving_cell_cfg)) {
      logger.debug(
          "Cell {} is neighbor of {} but skipping due to missing measurement parameters.", ncell_nci, serving_nci);
      continue;
    }
    srsran_assert(cfg.cells.find(ncell_nci) != cfg.cells.end(), "Cell id {} not found in list", ncell_nci);
    const auto& meas_cell_config = cfg.cells.at(ncell_nci);

    rrc_meas_obj_to_add_mod meas_obj;
    meas_obj.meas_obj_id = id_counter++;
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

    meas_cfg.value().meas_obj_to_add_mod_list.push_back(meas_obj);
  }

  // Add single periodic report configuration for NR.
  rrc_report_cfg_to_add_mod report_cfg_to_add_mod;

  report_cfg_to_add_mod.report_cfg_id = 1;

  rrc_report_cfg            report_cfg;
  rrc_report_cfg_nr         report_cfg_nr;
  rrc_periodical_report_cfg periodical;

  periodical.rs_type                = "ssb";
  periodical.report_interv          = 1024;
  periodical.report_amount          = -1;
  periodical.report_quant_cell.rsrp = true;
  periodical.report_quant_cell.rsrq = true;
  periodical.report_quant_cell.sinr = true;
  periodical.max_report_cells       = 4;

  rrc_meas_report_quant report_quant_rs_idxes;
  report_quant_rs_idxes.rsrp       = true;
  report_quant_rs_idxes.rsrq       = true;
  report_quant_rs_idxes.sinr       = true;
  periodical.report_quant_rs_idxes = report_quant_rs_idxes;

  periodical.max_nrof_rs_idxes_to_report = 4;
  periodical.include_beam_meass          = true;
  periodical.use_allowed_cell_list       = false;

  report_cfg_nr.periodical         = periodical;
  report_cfg.report_cfg_nr         = report_cfg_nr;
  report_cfg_to_add_mod.report_cfg = report_cfg;

  meas_cfg.value().report_cfg_to_add_mod_list.push_back(report_cfg_to_add_mod);

  // Add meas id to link the first neighbor cell and the report together.
  rrc_meas_id_to_add_mod meas_id_to_add_mod;

  meas_id_to_add_mod.meas_id       = 1;
  meas_id_to_add_mod.meas_obj_id   = 1;
  meas_id_to_add_mod.report_cfg_id = 1;

  meas_cfg.value().meas_id_to_add_mod_list.push_back(meas_id_to_add_mod);

  // Add quantity config.
  rrc_quant_cfg    quant_cfg;
  rrc_quant_cfg_nr quant_cfg_nr;
  quant_cfg_nr.quant_cfg_cell.ssb_filt_cfg.filt_coef_rsrp    = 6;
  quant_cfg_nr.quant_cfg_cell.csi_rs_filt_cfg.filt_coef_rsrp = 6;
  quant_cfg.quant_cfg_nr_list.push_back(quant_cfg_nr);

  meas_cfg.value().quant_cfg = quant_cfg;

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

void cell_meas_manager_impl::update_cell_config(nr_cell_id_t                    nci,
                                                const serving_cell_meas_config& serv_cell_cfg_,
                                                std::vector<nr_cell_id_t>       ncells_)
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

void cell_meas_manager_impl::report_measurement(const ue_index_t ue_index, const rrc_meas_results& meas_results)
{
  logger.debug("ue={} Received measurement result with meas_id={}", ue_index, meas_results.meas_id);

  // Verify meas_id is valid.

  // Ignore id with periodic measurements.

  // For meas_id with e.g. A3 event configured:

  // Iterate through serving cell results and check if best neighbor is reported.
  for (const auto& serv_cell : meas_results.meas_result_serving_mo_list) {
    if (serv_cell.meas_result_best_neigh_cell.has_value()) {
      // Report this cell.
      if (serv_cell.meas_result_best_neigh_cell.value().pci.has_value()) {
        mobility_mng_notifier.on_neighbor_better_than_spcell(ue_index,
                                                             serv_cell.meas_result_best_neigh_cell.value().pci.value());
      }
      return;
    }
  }

  // Find strongest neighbor cell
  if (meas_results.meas_result_neigh_cells.has_value()) {
    // Find strongest neighbor here.
    // Report cell.

    return;
  }
}