/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_cell_resource_allocator_impl.h"
#include "srsgnb/scheduler/config/serving_cell_config_factory.h"

using namespace srsgnb;
using namespace srs_du;

du_cell_resource_allocator_impl::du_cell_resource_allocator_impl(span<const du_cell_config> cell_cfg_list_) :
  cell_cfg_list(cell_cfg_list_),
  default_ul_cfg(config_helpers::make_default_ue_uplink_config()),
  logger(srslog::fetch_basic_logger("DU-MNG")),
  ue_res_list(cell_cfg_list.size())
{
  // Setup resource free lists.
  const unsigned period = sr_periodicity_to_slot(default_ul_cfg.init_ul_bwp.pucch_cfg->sr_res_list[0].period);
  sr_offset_free_list.resize(period);
  for (unsigned offset = 0; offset != period; ++offset) {
    sr_offset_free_list[offset] = offset;
  }
}

ue_cell_resource_list du_cell_resource_allocator_impl::update_resources(du_ue_index_t         ue_index,
                                                                        span<du_cell_index_t> ue_cells)
{
  ue_cell_resource_list ret(ue_cells.size());

  // Process cells that have been removed.
  for (unsigned i = 0; i != ue_res_list.size(); ++i) {
    du_cell_index_t           cell_index = to_du_cell_index(i);
    ue_cell_resource_context& ue_res_ctx = get_res(ue_index, cell_index);
    if (not ue_res_ctx.empty() and std::find(ue_cells.begin(), ue_cells.end(), cell_index) == ue_cells.end()) {
      // > cell removed. Deallocate resources.
      deallocate_cell_resources(ue_index, cell_index);
    }
  }

  // Process cells that have been added or modified.
  for (unsigned i = 0; i != ue_cells.size(); ++i) {
    auto                      ue_cell_idx = (ue_cell_index_t)i;
    du_cell_index_t           cell_idx    = ue_cells[i];
    ue_cell_resource_context& ue_res_ctx  = get_res(ue_index, cell_idx);
    ret[ue_cell_idx]                      = &ue_res_ctx.res;

    if (ue_res_ctx.empty()) {
      // > new UE cell. Allocate resources.
      if (not allocate_cell_resources(ue_index, cell_idx, ue_cell_idx)) {
        // >> Allocation failed.
        ret[ue_cell_idx] = nullptr;
      }
      continue;
    }

    if (ue_res_ctx.ue_cell_index == ue_cell_idx) {
      // > The UE cell index did not change. No allocation/deallocation required.
      continue;
    }

    if (ue_res_ctx.ue_cell_index == PCELL_INDEX or ue_cell_idx == PCELL_INDEX) {
      // > The UE cell index changed, and the affected cell is or was a PCell.
      // TODO: Deallocate PCell-specific resources.
    }

    ue_res_ctx.ue_cell_index = ue_cell_idx;
  }

  return ret;
}

bool du_cell_resource_allocator_impl::allocate_cell_resources(du_ue_index_t   ue_index,
                                                              du_cell_index_t cell_index,
                                                              ue_cell_index_t ue_cell_index)
{
  ue_cell_resource_context& ue_res_ctx = get_res(ue_index, cell_index);
  srsgnb_assert(ue_res_ctx.empty(), "Allocation called for already allocated resource");

  // Allocate PUCCH resources.
  ue_res_ctx.res.pucch_res_list = default_ul_cfg.init_ul_bwp.pucch_cfg->pucch_res_list;
  ue_res_ctx.res.sr_res_list    = default_ul_cfg.init_ul_bwp.pucch_cfg->sr_res_list;
  for (auto& sr : ue_res_ctx.res.sr_res_list) {
    if (sr_offset_free_list.empty()) {
      logger.warning("Unable to allocate dedicated PUCCH SR resource for UE={}, cell={}", ue_index, cell_index);
      return false;
    }
    sr.offset = sr_offset_free_list.back();
    sr_offset_free_list.pop_back();
  }

  // Mark the resource as allocated.
  ue_res_ctx.ue_cell_index = ue_cell_index;
  return true;
}

void du_cell_resource_allocator_impl::deallocate_cell_resources(du_ue_index_t ue_index, du_cell_index_t cell_index)
{
  ue_cell_resource_context& ue_res_ctx = get_res(ue_index, cell_index);
  srsgnb_assert(not ue_res_ctx.empty(), "Deallocation called for already deallocated resource");
  ue_res_ctx.ue_cell_index = INVALID_UE_CELL_INDEX;

  // Return resources back to free lists.
  for (auto& sr : ue_res_ctx.res.sr_res_list) {
    sr_offset_free_list.push_back(sr.offset);
  }
}
