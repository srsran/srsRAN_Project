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
#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "../ue_scheduling/ue.h"
#include "../ue_scheduling/ue_repository.h"
#include "../ue_scheduling/ue_scheduler.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// Information relative to a UE PDSCH grant.
struct ue_pdsch_grant {
  const ue*         user;
  du_cell_index_t   cell_index;
  harq_id_t         h_id;
  search_space_id   ss_id;
  unsigned          time_res_index;
  crb_interval      crbs;
  aggregation_level aggr_lvl = aggregation_level::n4;
  sch_mcs_index     mcs;
};

/// Information relative to a UE PUSCH grant.
struct ue_pusch_grant {
  const ue*         user;
  du_cell_index_t   cell_index;
  harq_id_t         h_id;
  crb_interval      crbs;
  ofdm_symbol_range symbols;
  unsigned          time_res_index;
  search_space_id   ss_id    = to_search_space_id(1);
  aggregation_level aggr_lvl = aggregation_level::n4;
  sch_mcs_index     mcs;
};

/// \brief Outcome of a UE grant allocation, and action for the scheduler policy to follow afterwards.
///
/// The current outcomes are:
/// - success - the allocation was successful with the provided parameters.
/// - skip_slot - failure to allocate and the scheduler policy should terminate the current slot processing.
/// - skip_ue - failure to allocate and the scheduler policy should move on to the next candidate UE.
/// - invalid_params - failure to allocate and the scheduler policy should try a different set of grant parameters.
enum class alloc_outcome { success, skip_slot, skip_ue, invalid_params };

/// Allocator of PDSCH grants for UEs.
class ue_pdsch_allocator
{
public:
  virtual ~ue_pdsch_allocator() = default;

  virtual alloc_outcome allocate_dl_grant(const ue_pdsch_grant& grant) = 0;
};

/// Allocator of PUSCH grants for UEs.
class ue_pusch_allocator
{
public:
  virtual ~ue_pusch_allocator() = default;

  virtual alloc_outcome allocate_ul_grant(const ue_pusch_grant& grant) = 0;
};

} // namespace srsran
