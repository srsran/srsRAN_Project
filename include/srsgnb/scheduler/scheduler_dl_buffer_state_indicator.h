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

#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"

namespace srsgnb {

/// DL Buffer state update for a given RLC bearer.
struct dl_bsr_indication_message {
  du_ue_index_t ue_index;
  lcid_t        lcid;
  unsigned      bsr;
};

/// Scheduler interface to push DL Buffer State Reports for a given RLC bearer.
class scheduler_dl_buffer_state_indicator
{
public:
  virtual ~scheduler_dl_buffer_state_indicator() = default;

  /// Forward DL buffer state update to scheduler.
  virtual void handle_dl_bsr_indication(const dl_bsr_indication_message& bsr) = 0;
};

} // namespace srsgnb