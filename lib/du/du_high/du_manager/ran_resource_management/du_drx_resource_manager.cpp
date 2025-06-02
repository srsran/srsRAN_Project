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

#include "du_drx_resource_manager.h"

using namespace srsran;
using namespace srs_du;

/// Pool of DRX start offsets, optimized for an equal distribution of offsets across UEs.
class srsran::srs_du::drx_offset_pool
{
public:
  drx_offset_pool(span<const du_cell_config> cells)
  {
    offset_count.reserve(cells.size());
    for (unsigned i = 0, sz = cells.size(); i != sz; ++i) {
      if (cells[i].mcg_params.drx.has_value()) {
        // Cell has DRX configured.
        offset_count.resize(i + 1);

        // Initialize pool of offsets.
        const drx_params& params   = cells[i].mcg_params.drx.value();
        const unsigned    nof_bins = params.long_cycle.count() / params.on_duration.count();
        for (unsigned j = 0; j != nof_bins; ++j) {
          offset_count[i].insert(std::make_pair(j * params.on_duration, 0));
        }
      }
    }
  }

  std::chrono::milliseconds allocate(du_cell_index_t cell_idx)
  {
    // Find the DRX cycle offset with lowest count.
    auto min_it = std::min_element(offset_count[cell_idx].begin(),
                                   offset_count[cell_idx].end(),
                                   [](const auto& lhs, const auto& rhs) { return lhs.second < rhs.second; });
    min_it->second++;
    return min_it->first;
  }

  void deallocate(du_cell_index_t cell_idx, std::chrono::milliseconds offset)
  {
    srsran_assert(offset_count[cell_idx].count(offset) > 0 and offset_count[cell_idx].at(offset) > 0,
                  "invalid offset {}",
                  offset.count());
    offset_count[cell_idx].at(offset)--;
  }

private:
  std::vector<std::map<std::chrono::milliseconds, unsigned>> offset_count;
};

du_drx_resource_manager::du_drx_resource_manager(span<const du_cell_config> cell_cfg_list_) :
  cell_cfg_list(cell_cfg_list_), offset_pool(std::make_unique<drx_offset_pool>(cell_cfg_list_))
{
}

du_drx_resource_manager::~du_drx_resource_manager() = default;

void du_drx_resource_manager::handle_ue_creation(cell_group_config& cell_grp_cfg)
{
  // UE always starts with DRX disabled.
  cell_grp_cfg.mcg_cfg.drx_cfg.reset();
}

void du_drx_resource_manager::handle_ue_cap_update(cell_group_config& cell_grp_cfg, bool long_drx_cycle_supported)
{
  std::optional<drx_config>& current_ue_drx   = cell_grp_cfg.mcg_cfg.drx_cfg;
  const du_cell_index_t      pcell_index      = cell_grp_cfg.cells[0].serv_cell_cfg.cell_index;
  const du_cell_config&      pcell_cfg_common = cell_cfg_list[pcell_index];

  // If both UE and gNB cell are configured to support long DRX cycle.
  const bool long_drx_cycle_enable_cmd = long_drx_cycle_supported and pcell_cfg_common.mcg_params.drx.has_value() and
                                         pcell_cfg_common.mcg_params.drx.value().long_cycle.count() != 0;
  // Previous UE DRX state.
  const bool prev_ue_long_drx_support = current_ue_drx.has_value() and current_ue_drx->long_cycle.count() != 0;
  if (long_drx_cycle_enable_cmd == prev_ue_long_drx_support) {
    // No change detected in UE state. Early exit.
    return;
  }

  if (current_ue_drx.has_value()) {
    // UE had a DRX already configured. Deallocate its resources before proceeding.
    handle_ue_removal(cell_grp_cfg);
  }

  if (not long_drx_cycle_enable_cmd) {
    // gNB cell does not have DRX configured. No point in proceeding.
    return;
  }

  // Enable DRX for the UE.
  current_ue_drx.emplace();
  drx_config& drx       = current_ue_drx.value();
  drx.long_cycle        = pcell_cfg_common.mcg_params.drx->long_cycle;
  drx.long_start_offset = offset_pool->allocate(pcell_index);
  drx.on_duration_timer = pcell_cfg_common.mcg_params.drx->on_duration;
  drx.inactivity_timer  = pcell_cfg_common.mcg_params.drx->inactivity_timer;
  drx.retx_timer_dl     = pcell_cfg_common.mcg_params.drx->retx_timer_dl;
  drx.retx_timer_ul     = pcell_cfg_common.mcg_params.drx->retx_timer_ul;
}

void du_drx_resource_manager::handle_ue_removal(cell_group_config& cell_grp_cfg)
{
  if (not cell_grp_cfg.mcg_cfg.drx_cfg.has_value()) {
    return;
  }

  // Return offset back to the pool.
  drx_config& drx = cell_grp_cfg.mcg_cfg.drx_cfg.value();
  offset_pool->deallocate(cell_grp_cfg.cells[0].serv_cell_cfg.cell_index, drx.long_start_offset);

  // Clear DRX config.
  cell_grp_cfg.mcg_cfg.drx_cfg.reset();
}
