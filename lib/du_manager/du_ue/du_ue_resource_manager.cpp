/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_ue_resource_manager.h"

using namespace srsgnb;
using namespace srs_du;

void srsgnb::srs_du::fill_cell_group_resources(cell_group_config& cell_group_cfg, const ue_cell_resource& res)
{
  cell_group_cfg.spcell_cfg.spcell_cfg_ded.ul_config->init_ul_bwp.pucch_cfg->sr_res_list    = res.sr_res_list;
  cell_group_cfg.spcell_cfg.spcell_cfg_ded.ul_config->init_ul_bwp.pucch_cfg->pucch_res_list = res.pucch_res_list;
}

bool du_ue_resource_manager::set_cells(span<du_cell_index_t> cell_indexes)
{
  ue_cells = cell_res_mng->update_resources(ue_index, cell_indexes);
  update_du_cell_ue_cell_index_map(cell_indexes);
  for (unsigned i = 0; i != cell_indexes.size(); ++i) {
    if (ue_cells[i] == nullptr) {
      return false;
    }
  }
  return true;
}

void du_ue_resource_manager::update_du_cell_ue_cell_index_map(span<du_cell_index_t> cell_indexes)
{
  std::fill(du_cell_to_ue_cell_idx.begin(), du_cell_to_ue_cell_idx.end(), INVALID_UE_CELL_INDEX);
  for (unsigned i = 0; i != cell_indexes.size(); ++i) {
    if (ue_cells[i] != nullptr) {
      du_cell_to_ue_cell_idx[cell_indexes[i]] = (ue_cell_index_t)i;
    }
  }
}
