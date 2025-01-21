/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ra_resource_manager.h"

using namespace srsran;
using namespace srs_du;

ra_resource_manager::ra_resource_manager(span<const du_cell_config> cell_cfg_list)
{
  cells.resize(cell_cfg_list.size());
  for (unsigned i = 0, e = cells.size(); i != e; ++i) {
    cells[i].cell_cfg = &cell_cfg_list[i];

    if (cells[i].cell_cfg->cfra_enabled and cells[i].cell_cfg->ul_cfg_common.init_ul_bwp.rach_cfg_common.has_value()) {
      const auto& rach_common = cells[i].cell_cfg->ul_cfg_common.init_ul_bwp.rach_cfg_common.value();

      // Store the preambles used for CFRA.
      const unsigned nof_cf_preambles = rach_common.total_nof_ra_preambles - rach_common.nof_cb_preambles_per_ssb;
      cells[i].free_preamble_idx_list.reserve(nof_cf_preambles);
      for (unsigned j = 0; j != nof_cf_preambles; ++j) {
        cells[i].free_preamble_idx_list.push_back(rach_common.nof_cb_preambles_per_ssb + j);
      }
    }
  }
}

void ra_resource_manager::allocate_cfra_resources(du_ue_resource_config& ue_res_cfg)
{
  if (ue_res_cfg.cfra.has_value()) {
    // UE has CFRA already configured.
    return;
  }

  du_cell_index_t  pcell_index = ue_res_cfg.cell_group.cells[0].serv_cell_cfg.cell_index;
  cell_ra_context& pcell_ra    = cells[pcell_index];

  if (pcell_ra.free_preamble_idx_list.empty()) {
    // CFRA is either not enabled for this cell or there are no free RA preambles.
    return;
  }

  // Allocate a CFRA preamble.
  const unsigned selected_preamble = pcell_ra.free_preamble_idx_list.back();
  pcell_ra.free_preamble_idx_list.pop_back();

  // Store allocated CFRA preambled in UE resources.
  ue_res_cfg.cfra.emplace();
  ue_res_cfg.cfra.value().preamble_id = selected_preamble;
}

void ra_resource_manager::deallocate_cfra_resources(du_ue_resource_config& ue_res_cfg)
{
  if (ue_res_cfg.cfra.has_value()) {
    // Return allocated CFRA preamble to the pool.
    cell_ra_context& cell = cells[ue_res_cfg.cell_group.cells[0].serv_cell_cfg.cell_index];
    cell.free_preamble_idx_list.push_back(ue_res_cfg.cfra.value().preamble_id);

    // Reset CFRA resources.
    ue_res_cfg.cfra.reset();
  }
}
