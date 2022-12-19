/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_ue/du_ue_resource_manager.h"
#include <deque>

namespace srsgnb {
namespace srs_du {

class du_cell_resource_allocator_impl : public du_ran_resource_allocator
{
public:
  du_cell_resource_allocator_impl(span<const du_cell_config> cell_cfg_list_,
                                  const serving_cell_config& default_ue_cell_cfg_);
  du_cell_resource_allocator_impl(du_cell_resource_allocator_impl&&)                 = delete;
  du_cell_resource_allocator_impl(const du_cell_resource_allocator_impl&)            = delete;
  du_cell_resource_allocator_impl& operator=(du_cell_resource_allocator_impl&&)      = delete;
  du_cell_resource_allocator_impl& operator=(const du_cell_resource_allocator_impl&) = delete;

  const cell_group_config* update_context(du_ue_index_t                         ue_index,
                                          du_cell_index_t                       pcell_idx,
                                          const f1ap_ue_context_update_request& upd_req) override;

private:
  bool allocate_cell_resources(du_ue_index_t ue_index, du_cell_index_t cell_index, serv_cell_index_t serv_cell_index);
  void deallocate_cell_resources(du_ue_index_t ue_index, serv_cell_index_t serv_cell_index);

  span<const du_cell_config> cell_cfg_list;
  const serving_cell_config  default_ue_cell_cfg;
  srslog::basic_logger&      logger;

  /// Current UE Resource Allocations.
  slotted_array<cell_group_config, MAX_NOF_DU_UES, false> ue_res_list;

  /// Lists of PHY/MAC resources available to be allocated.
  static_vector<std::vector<unsigned>, MAX_NOF_DU_CELLS> sr_offset_free_list;
};

} // namespace srs_du
} // namespace srsgnb