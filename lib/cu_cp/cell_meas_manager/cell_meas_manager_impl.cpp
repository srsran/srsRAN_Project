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
}

cu_cp_meas_cfg cell_meas_manager_impl::get_measurement_config(const nr_cell_id_t& cid)
{
  // Find cell.
  if (cfg.neighbor_cell_list.find(cid) == cfg.neighbor_cell_list.end()) {
    logger.error("Couldn't find neighbor cell list for cid={}", cid);
    return {};
  }

  const auto& neighbor_cells = cfg.neighbor_cell_list.at(cid);

  cu_cp_meas_cfg meas_cfg;

  // Create meas object for each neighbor cell
  for (unsigned i = 0; i < neighbor_cells.size(); ++i) {
    cu_cp_meas_obj_to_add_mod meas_obj;
    meas_obj.meas_obj_id = i;
    meas_obj.meas_obj_nr.emplace();
    // TODO: add config
    meas_cfg.meas_obj_to_add_mod_list.push_back(meas_obj);
  }

  return meas_cfg;
}

void cell_meas_manager_impl::report_measurement(const cu_cp_meas_results& meas_results)
{
  logger.debug("Received measurement result with meas_id={}", meas_results.meas_id);

  // TODO: handle measurement result
}
