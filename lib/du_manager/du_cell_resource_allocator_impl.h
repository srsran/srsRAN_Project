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

class du_cell_resource_allocator_impl;

class du_ue_ran_resource_updater_impl final : public du_ue_resource_configurator::resource_updater
{
public:
  du_ue_ran_resource_updater_impl(cell_group_config*               cell_cfg_,
                                  du_cell_resource_allocator_impl& parent_,
                                  du_ue_index_t                    ue_index_);
  du_ue_ran_resource_updater_impl(const du_ue_ran_resource_updater_impl&)            = delete;
  du_ue_ran_resource_updater_impl(const du_ue_ran_resource_updater_impl&&)           = delete;
  du_ue_ran_resource_updater_impl& operator=(const du_ue_ran_resource_updater_impl&) = delete;
  du_ue_ran_resource_updater_impl& operator=(du_ue_ran_resource_updater_impl&&)      = delete;
  ~du_ue_ran_resource_updater_impl() override;

  du_ue_ran_resource_update_response update(du_cell_index_t                       pcell_index,
                                            const f1ap_ue_context_update_request& upd_req) override;

  const cell_group_config& get() override { return *cell_grp; }

private:
  cell_group_config*               cell_grp;
  du_cell_resource_allocator_impl* parent;
  du_ue_index_t                    ue_index;
};

class du_cell_resource_allocator_impl : public du_ue_resource_configurator_factory
{
public:
  du_cell_resource_allocator_impl(span<const du_cell_config> cell_cfg_list_,
                                  const serving_cell_config& default_ue_cell_cfg_);
  du_cell_resource_allocator_impl(du_cell_resource_allocator_impl&&)                 = delete;
  du_cell_resource_allocator_impl(const du_cell_resource_allocator_impl&)            = delete;
  du_cell_resource_allocator_impl& operator=(du_cell_resource_allocator_impl&&)      = delete;
  du_cell_resource_allocator_impl& operator=(const du_cell_resource_allocator_impl&) = delete;

  du_ue_resource_configurator create_ue_resource_configurator(du_ue_index_t   ue_index,
                                                              du_cell_index_t pcell_index) override;

  du_ue_ran_resource_update_response
  update_context(du_ue_index_t ue_index, du_cell_index_t pcell_idx, const f1ap_ue_context_update_request& upd_req);

  void deallocate_context(du_ue_index_t ue_index);

private:
  bool allocate_cell_resources(du_ue_index_t ue_index, du_cell_index_t cell_index, serv_cell_index_t serv_cell_index);
  void deallocate_cell_resources(du_ue_index_t ue_index, serv_cell_index_t serv_cell_index);

  span<const du_cell_config> cell_cfg_list;
  const serving_cell_config  default_ue_cell_cfg;
  srslog::basic_logger&      logger;

  struct ue_res_item {
    cell_group_config cg_cfg;
  };

  /// Current UE Resource Allocations.
  slotted_array<ue_res_item, MAX_NOF_DU_UES, false> ue_res_pool;

  /// Lists of PHY/MAC resources available to be allocated.
  static_vector<std::vector<unsigned>, MAX_NOF_DU_CELLS> sr_offset_free_list;
};

} // namespace srs_du
} // namespace srsgnb