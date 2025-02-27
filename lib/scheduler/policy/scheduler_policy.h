/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/scheduler/result/pdsch_info.h"
#include "srsran/scheduler/result/pusch_info.h"

namespace srsran {

class slice_ue;
class ue_cell;

/// Type used to represent a UE priority.
using ue_sched_priority = double;

/// Priority value used to represent that the UE must not be scheduled.
const ue_sched_priority forbid_sched_priority = std::numeric_limits<ue_sched_priority>::min();

/// UE candidate for DL or UL scheduling.
struct ue_newtx_candidate {
  const slice_ue*   ue;
  const ue_cell*    ue_cc;
  unsigned          pending_bytes;
  ue_sched_priority priority;
};

/// Interface of data scheduler that is used to allocate UE DL and UL grants in a given slot
/// The data_scheduler object will be common to all cells and slots.
class scheduler_policy
{
public:
  virtual ~scheduler_policy() = default;

  virtual void add_ue(du_ue_index_t ue_index) = 0;

  virtual void rem_ue(du_ue_index_t ue_index) = 0;

  /// Compute UE candidate priorities for DL scheduling.
  ///
  /// \param[in] pdcch_slot PDCCH slot.
  /// \param[in] pdsch_slot PDSCH slot.
  /// \param[in] ue_candidates UE candidates with new HARQ transmissions to be scheduled.
  virtual void
  compute_ue_dl_priorities(slot_point pdcch_slot, slot_point pdsch_slot, span<ue_newtx_candidate> ue_candidates) = 0;

  /// Compute UE candidate priorities for UL scheduling.
  ///
  /// \param[in] pdcch_slot PDCCH slot.
  /// \param[in] pusch_slot PUSCH slot.
  /// \param[in] ue_candidates UE candidates with new HARQ transmissions to be scheduled.
  virtual void
  compute_ue_ul_priorities(slot_point pdcch_slot, slot_point pusch_slot, span<ue_newtx_candidate> ue_candidates) = 0;

  /// Save UE DL newtx grants.
  virtual void save_dl_newtx_grants(span<const dl_msg_alloc> dl_grants) = 0;

  /// Save UE UL newtx grants.
  virtual void save_ul_newtx_grants(span<const ul_sched_info> dl_grants) = 0;
};

} // namespace srsran
