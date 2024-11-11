/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
    for (unsigned i = 0; i != cells.size(); ++i) {
      if (cells[i].mcg_params.drx.has_value()) {
        // cell has DRX configured.
        offset_count.resize(i + 1);

        // initialize pool of offsets.
        const drx_params& params   = cells[i].mcg_params.drx.value();
        const unsigned    nof_bins = params.long_cycle.count() / params.on_duration.count();
        for (unsigned j = 0; j < nof_bins; ++j) {
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
    return std::chrono::milliseconds(min_it->second);
  }

  void deallocate(du_cell_index_t cell_idx, std::chrono::milliseconds offset)
  {
    srsran_sanity_check(offset_count[cell_idx].count(offset) > 0 and offset_count[cell_idx].at(offset) > 0,
                        "invalid offset {}",
                        offset.count());
    offset_count[cell_idx].at(offset)--;
  }

private:
  std::vector<std::map<std::chrono::milliseconds, unsigned>> offset_count;
};

du_drx_resource_manager::du_drx_resource_manager(span<const du_cell_config> cell_cfg_list_) :
  cell_cfg_list(cell_cfg_list_)
{
}

du_drx_resource_manager::~du_drx_resource_manager() = default;

void du_drx_resource_manager::alloc_resources(cell_group_config& cell_grp_cfg)
{
  const du_cell_index_t pcell_index = cell_grp_cfg.cells[0].serv_cell_cfg.cell_index;
  const du_cell_config& pcell_cfg   = cell_cfg_list[pcell_index];

  cell_grp_cfg.mcg_cfg.drx_cfg.reset();
  if (not pcell_cfg.mcg_params.drx.has_value()) {
    return;
  }

  cell_grp_cfg.mcg_cfg.drx_cfg.emplace();
  drx_config& drx       = cell_grp_cfg.mcg_cfg.drx_cfg.value();
  drx.long_cycle        = pcell_cfg.mcg_params.drx->long_cycle;
  drx.long_start_offset = offset_pool->allocate(pcell_index);
  drx.on_duration_timer = pcell_cfg.mcg_params.drx->on_duration;
  drx.inactivity_timer  = pcell_cfg.mcg_params.drx->inactivity_timer;
}

void du_drx_resource_manager::dealloc_resources(cell_group_config& cell_grp_cfg)
{
  if (not cell_grp_cfg.mcg_cfg.drx_cfg.has_value()) {
    return;
  }

  // Return offset back to the pool.
  drx_config& drx = cell_grp_cfg.mcg_cfg.drx_cfg.value();
  offset_pool->deallocate(cell_grp_cfg.cells[0].serv_cell_cfg.cell_index, drx.long_start_offset);
}
