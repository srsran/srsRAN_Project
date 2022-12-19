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

du_ue_resource_manager::du_ue_resource_manager(du_ue_index_t ue_index_, du_ran_resource_allocator& cell_res_mng_) :
  ue_index(ue_index_), cell_res_mng(&cell_res_mng_)
{
}

du_ue_resource_manager::~du_ue_resource_manager()
{
  cell_res_mng->update_context(ue_index, INVALID_DU_CELL_INDEX, {});
}

bool du_ue_resource_manager::update_context(du_cell_index_t pcell_index, const f1ap_ue_context_update_request& upd_req)
{
  ue_res = cell_res_mng->update_context(ue_index, pcell_index, upd_req);
  if (ue_res == nullptr) {
    return false;
  }
  update_du_cell_ue_cell_index_map(pcell_index, upd_req);
  return true;
}

void du_ue_resource_manager::update_du_cell_ue_cell_index_map(du_cell_index_t                       pcell_index,
                                                              const f1ap_ue_context_update_request& upd_req)
{
  std::fill(du_cell_to_ue_cell_idx.begin(), du_cell_to_ue_cell_idx.end(), SERVING_CELL_PCELL_IDX);
  du_cell_to_ue_cell_idx[pcell_index] = SERVING_CELL_PCELL_IDX;
  for (const scell_to_setup& scell : upd_req.scells_to_setup) {
    du_cell_to_ue_cell_idx[scell.cell_index] = scell.serv_cell_index;
  }
}
