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
#include <mutex>
#include <unordered_map>

namespace srsgnb {

/// Defines SRB0 scheduler that is used to allocate grants for UE's SRB0 DL messages in a given slot
class ue_srb0_scheduler
{
public:
  explicit ue_srb0_scheduler(srslog::basic_logger& logger);

  /// Handles DL buffer state reported by upper layers.
  /// \param[in] bs DL buffer state. This object provides buffer state for a particular logical channel.
  void handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& bs);

  /// Schedule UE's SRB0 DL grants for a given slot and one or more cells.
  /// \param[in/out] pdsch_alloc PDSCH grant allocator. This object provides a handle to allocate PDSCH grants in the
  ///                            gNB resource grid and observe the current DL gNB resource grid occupancy state.
  /// \param[in] ues List of eligible UEs to be scheduled in the given slot.
  void run_slot(ue_pdsch_allocator& pdsch_alloc, const ue_list& ues);

private:
  /// List of SRB0 DL buffer state reported by upper layers to be scheduled.
  std::unordered_map<rnti_t, dl_buffer_state_indication_message> ue_dl_bs;

  srslog::basic_logger& logger;
};

} // namespace srsgnb
