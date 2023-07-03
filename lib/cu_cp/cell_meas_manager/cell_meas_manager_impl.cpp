/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cell_meas_manager_impl.h"
#include "cell_meas_manager_helpers.h"

using namespace srsran;
using namespace srs_cu_cp;

cell_meas_manager_impl::cell_meas_manager_impl(const cell_meas_manager_cfg& cfg_) :
  cfg(cfg_), logger(srslog::fetch_basic_logger("CU-CP"))
{
  srsran_assert(is_valid_configuration(cfg), "Invalid cell measurment configuration");
  log_cells(logger, cfg);
}

rrc_meas_cfg cell_meas_manager_impl::get_measurement_config(nr_cell_id_t serving_nci)
{
  // Find cell.
  if (cfg.cells.find(serving_nci) == cfg.cells.end()) {
    logger.error("Couldn't find neighbor cell list for nci={}", serving_nci);
    return {};
  }

  const auto& cell_config = cfg.cells.at(serving_nci);

  rrc_meas_cfg meas_cfg;

  // Create meas object for each neighbor cell
  unsigned id_counter = 0;
  for (const auto& ncell_nci : cell_config.ncells) {
    // Verify we have a complete config for this cell.
    if (!is_complete(cfg.cells.at(ncell_nci))) {
      logger.debug(
          "Cell {} is neighbor of {} but skipping due to missing measurement parameters.", ncell_nci, serving_nci);
      continue;
    }
    srsran_assert(cfg.cells.find(ncell_nci) != cfg.cells.end(), "Cell id {} not found in list", ncell_nci);
    const auto& ncell_config = cfg.cells.at(ncell_nci);

    rrc_meas_obj_to_add_mod meas_obj;
    meas_obj.meas_obj_id = id_counter++;
    meas_obj.meas_obj_nr.emplace();
    auto& meas_obj_nr                  = meas_obj.meas_obj_nr.value();
    meas_obj_nr.ssb_freq               = ncell_config.ssb_arfcn;
    meas_obj_nr.ssb_subcarrier_spacing = ncell_config.ssb_scs;
    meas_obj_nr.smtc1                  = ncell_config.ssb_mtc;

    // Mandatory fields.
    meas_obj_nr.ref_sig_cfg.ssb_cfg_mob.emplace().derive_ssb_idx_from_cell = true;
    meas_obj_nr.nrof_ss_blocks_to_average.emplace()                        = 8;
    meas_obj_nr.quant_cfg_idx                                              = 0;
    meas_obj_nr.freq_band_ind_nr.emplace()                                 = nr_band_to_uint(ncell_config.band.value());

    meas_cfg.meas_obj_to_add_mod_list.push_back(meas_obj);
  }

  return meas_cfg;
}

optional<cell_meas_cfg> cell_meas_manager_impl::get_cell_config(nr_cell_id_t nci)
{
  optional<cell_meas_cfg> cell_cfg;
  if (cfg.cells.find(nci) != cfg.cells.end()) {
    cell_cfg = cfg.cells.at(nci);
  }
  return cell_cfg;
}

bool cell_meas_manager_impl::update_cell_config(nr_cell_id_t nci, const cell_meas_cfg& cell_cfg_)
{
  if (cfg.cells.find(nci) == cfg.cells.end()) {
    logger.error("Can't update configuration for nci={}. Cell not found.", nci);
    return false;
  }
  if (!is_complete(cell_cfg_)) {
    logger.warning("Updating incomplete cell measurement configuration for nci={}", nci);
  }

  // Perform update.
  cfg.cells.at(nci) = cell_cfg_;

  log_cells(logger, cfg);

  return true;
}

void cell_meas_manager_impl::report_measurement(const rrc_meas_results& meas_results)
{
  logger.debug("Received measurement result with meas_id={}", meas_results.meas_id);

  // TODO: handle measurement result
}
