/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#pragma once

#include "../cell/resource_grid.h"
#include "../ue_scheduling/ue_configuration.h"
#include "pdcch_resource_allocator.h"
#include "srsran/scheduler/scheduler_dci.h"

namespace srsran {

class pdcch_resource_allocator_impl final : public pdcch_resource_allocator
{
public:
  explicit pdcch_resource_allocator_impl(const cell_configuration& cell_cfg_);
  ~pdcch_resource_allocator_impl() override;

  void slot_indication(slot_point sl_tx);

  pdcch_dl_information* alloc_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                           rnti_t                        rnti,
                                           search_space_id               ss_id,
                                           aggregation_level             aggr_lvl) override;

  pdcch_ul_information* alloc_ul_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                              rnti_t                        rnti,
                                              search_space_id               ss_id,
                                              aggregation_level             aggr_lvl) override;

  pdcch_dl_information* alloc_dl_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        rnti,
                                          const ue_cell_configuration&  user,
                                          search_space_id               ss_id,
                                          aggregation_level             aggr_lvl) override;

  pdcch_ul_information* alloc_ul_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        rnti,
                                          const ue_cell_configuration&  user,
                                          search_space_id               ss_id,
                                          aggregation_level             aggr_lvl) override;

  bool cancel_last_pdcch(cell_slot_resource_allocator& slot_alloc) override;

private:
  class pdcch_slot_allocator;

  /// Size of the ring buffer of pdcch_slot_allocator. This size sets a limit on how far in advance a PDCCH can be
  /// allocated.
  static const size_t SLOT_ALLOCATOR_RING_SIZE = get_allocator_ring_size_gt_min(SCHEDULER_MAX_K0);

  pdcch_slot_allocator& get_pdcch_slot_alloc(slot_point sl);

  pdcch_dl_information* alloc_dl_pdcch_helper(cell_slot_resource_allocator&     slot_alloc,
                                              rnti_t                            rnti,
                                              const bwp_configuration&          bwp_cfg,
                                              const coreset_configuration&      cs_cfg,
                                              const search_space_configuration& ss_cfg,
                                              aggregation_level                 aggr_lvl);

  pdcch_ul_information* alloc_ul_pdcch_helper(cell_slot_resource_allocator&     slot_alloc,
                                              rnti_t                            rnti,
                                              const bwp_configuration&          bwp_cfg,
                                              const coreset_configuration&      cs_cfg,
                                              const search_space_configuration& ss_cfg,
                                              aggregation_level                 aggr_lvl);

  const cell_configuration& cell_cfg;

  /// Last slot for which slot_indication has been called.
  slot_point last_sl_ind;

  /// Intermediate results used by PDCCH scheduler.
  std::array<std::unique_ptr<pdcch_slot_allocator>, SLOT_ALLOCATOR_RING_SIZE> slot_records;
};

} // namespace srsran
