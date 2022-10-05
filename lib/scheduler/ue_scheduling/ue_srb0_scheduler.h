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

#include "../policy/ue_allocator.h"
#include "../support/slot_event_list.h"
#include "srsgnb/scheduler/scheduler_configurator.h"
#include "ue.h"
#include <queue>

namespace srsgnb {

/// Defines SRB0 scheduler that is used to allocate grants for UE's SRB0 DL messages in a given slot
class ue_srb0_scheduler
{
public:
  explicit ue_srb0_scheduler(srslog::basic_logger& logger);

  /// Handles DL buffer state reported by upper layers.
  /// \param[in] ue_index UE's DU Index for which SRB0 message needs to be scheduled.
  void handle_dl_buffer_state_indication(du_ue_index_t ue_index);

  /// Schedule UE's SRB0 DL grants for a given slot and one or more cells.
  /// \param[in/out] pdsch_alloc PDSCH grant allocator. This object provides a handle to allocate PDSCH grants in the
  ///                            gNB resource grid and observe the current DL gNB resource grid occupancy state.
  /// \param[in] ues List of eligible UEs to be scheduled in the given slot.
  void run_slot(ue_pdsch_allocator& pdsch_alloc, const ue_list& ues);

private:
  /// \brief Tries to schedule SRB0 messages for a UE. Returns true if successful, false otherwise.
  bool schedule_srb0(ue_pdsch_allocator& pdsch_alloc, const ue& u);

  /// List of UE's DU Indexes for which SRB0 messages needs to be scheduled.
  std::vector<du_ue_index_t> pending_ues;

  srslog::basic_logger& logger;
};

} // namespace srsgnb
