/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_pucch_resource_manager.h"

using namespace srsgnb;
using namespace srs_du;

du_pucch_resource_manager::du_pucch_resource_manager(span<const du_cell_config> cell_cfg_list_,
                                                     const pucch_config&        default_pucch_cfg_) :
  default_pucch_cfg(default_pucch_cfg_), cells(cell_cfg_list_.size())
{
  srsgnb_assert(not default_pucch_cfg.sr_res_list.empty(), "There must be at least one SR Resource");

  // Compute fundamental SR period.
  // TODO: Handle more than one SR period.
  unsigned sr_period = sr_periodicity_to_slot(default_pucch_cfg_.sr_res_list[0].period);

  // Setup RAN resources per cell.
  for (auto& cell : cells) {
    cell.sr_offset_free_list.resize(sr_period);
    for (unsigned offset = 0; offset != sr_period; ++offset) {
      cell.sr_offset_free_list[offset] = offset;
    }
  }
}

bool du_pucch_resource_manager::alloc_resources(cell_group_config& cell_grp_cfg)
{
  // TODO: Support multiple BWPs per cell.
  cell_grp_cfg.spcell_cfg.spcell_cfg_ded.ul_config->init_ul_bwp.pucch_cfg = default_pucch_cfg;
  auto& target_pucch_cfg = *cell_grp_cfg.spcell_cfg.spcell_cfg_ded.ul_config->init_ul_bwp.pucch_cfg;
  auto& sr_res_list      = target_pucch_cfg.sr_res_list;

  auto&    free_list = cells[cell_grp_cfg.spcell_cfg.cell_index].sr_offset_free_list;
  unsigned i         = 0;
  for (; i != sr_res_list.size(); ++i) {
    if (free_list.empty()) {
      break;
    }
    sr_res_list[i].offset = free_list.back();
    free_list.pop_back();
  }
  if (i != sr_res_list.size()) {
    // Allocation failed. Return resources back to the pool.
    for (unsigned j = 0; j != i; ++j) {
      free_list.push_back(sr_res_list[i].offset);
    }
    return false;
  }

  return true;
}

void du_pucch_resource_manager::dealloc_resources(cell_group_config& cell_grp_cfg)
{
  for (auto& sr : cell_grp_cfg.spcell_cfg.spcell_cfg_ded.ul_config->init_ul_bwp.pucch_cfg->sr_res_list) {
    cells[cell_grp_cfg.spcell_cfg.cell_index].sr_offset_free_list.push_back(sr.offset);
  }
}
