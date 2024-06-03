/*
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  const ue*       user;
  du_cell_index_t cell_index;
  harq_id_t       h_id;
  /// Recommended nof. bytes to schedule. This field is not present/ignored in case of HARQ retransmission.
  std::optional<unsigned> recommended_nof_bytes;
  /// Maximum nof. RBs to allocate to the UE. This field is not present/ignored in case of HARQ retransmission.
  std::optional<unsigned> max_nof_rbs;
};

/// Information relative to a UE PUSCH grant.
struct ue_pusch_grant {
  const ue*       user;
  du_cell_index_t cell_index;
  harq_id_t       h_id;
  /// Recommended nof. bytes to schedule. This field is not present/ignored in case of HARQ retransmission.
  std::optional<unsigned> recommended_nof_bytes;
  /// Maximum nof. RBs to allocate to the UE. This field is not present/ignored in case of HARQ retransmission.
  std::optional<unsigned> max_nof_rbs;
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
