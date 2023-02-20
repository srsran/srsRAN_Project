/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"

namespace srsran {

/// DL buffer state for a given RLC bearer.
struct dl_buffer_state_indication_message {
  du_ue_index_t ue_index;
  lcid_t        lcid;
  unsigned      bs;
};

/// Scheduler interface to push DL buffer state updates for a given RLC bearer.
class scheduler_dl_buffer_state_indication_handler
{
public:
  virtual ~scheduler_dl_buffer_state_indication_handler() = default;

  /// Forward DL buffer state update to scheduler.
  virtual void handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& bs) = 0;
};

} // namespace srsran
