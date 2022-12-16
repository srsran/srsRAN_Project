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

namespace srsgnb {
namespace srs_du {

class du_cell_resource_allocator_impl : public du_cell_resource_allocator
{
public:
  du_cell_resource_allocator_impl(span<const du_cell_config> cell_cfg_list_);
  du_cell_resource_allocator_impl(du_cell_resource_allocator_impl&&)                 = delete;
  du_cell_resource_allocator_impl(const du_cell_resource_allocator_impl&)            = delete;
  du_cell_resource_allocator_impl& operator=(du_cell_resource_allocator_impl&&)      = delete;
  du_cell_resource_allocator_impl& operator=(const du_cell_resource_allocator_impl&) = delete;

  ue_cell_resource_list update_resources(du_ue_index_t ue_index, span<du_cell_index_t> ue_cells) override;

private:
  struct ue_cell_resource_context {
    ue_cell_index_t  ue_cell_index = INVALID_UE_CELL_INDEX;
    ue_cell_resource res;

    bool empty() const { return ue_cell_index == INVALID_UE_CELL_INDEX; }
  };

  ue_cell_resource_context& get_res(du_ue_index_t ue_idx, du_cell_index_t cell_idx)
  {
    return ue_res_list[cell_idx][ue_idx];
  }
  bool allocate_cell_resources(du_ue_index_t ue_index, du_cell_index_t cell_index, ue_cell_index_t ue_cell_index);
  void deallocate_cell_resources(du_ue_index_t ue_index, du_cell_index_t cell_index);

  span<const du_cell_config> cell_cfg_list;
  uplink_config              default_ul_cfg;
  srslog::basic_logger&      logger;

  std::vector<std::array<ue_cell_resource_context, MAX_NOF_DU_UES>> ue_res_list;

  // Lists of resources available.
  std::vector<unsigned> sr_offset_free_list;
};

} // namespace srs_du
} // namespace srsgnb