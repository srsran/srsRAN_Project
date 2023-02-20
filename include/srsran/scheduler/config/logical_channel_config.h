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

#include "srsran/adt/optional.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/sr_configuration.h"
#include "srsran/scheduler/config/logical_channel_group.h"

namespace srsran {

/// \c LogicalChannelConfig, TS 38.331.
struct logical_channel_config {
  lcid_t  lcid;
  uint8_t priority;
  // TODO: add remaining fields;
  lcg_id_t                        lc_group;
  optional<scheduling_request_id> sr_id;
  bool                            lc_sr_mask;
  bool                            lc_sr_delay_timer_applied;
};

} // namespace srsran