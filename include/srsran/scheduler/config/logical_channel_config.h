/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/lcid.h"
#include "srsran/ran/rrm.h"
#include "srsran/ran/sr_configuration.h"
#include "srsran/scheduler/config/logical_channel_group.h"

namespace srsran {

/// \c LogicalChannelConfig, TS 38.331.
struct logical_channel_config {
  lcid_t  lcid;
  uint8_t priority;
  // TODO: add remaining fields;
  lcg_id_t                             lc_group;
  std::optional<scheduling_request_id> sr_id;
  bool                                 lc_sr_mask;
  bool                                 lc_sr_delay_timer_applied;
  rrm_policy_member                    rrm_policy;

  bool operator==(const logical_channel_config& rhs) const
  {
    return lcid == rhs.lcid and priority == rhs.priority and lc_group == rhs.lc_group and sr_id == rhs.sr_id and
           lc_sr_mask == rhs.lc_sr_mask and lc_sr_delay_timer_applied == rhs.lc_sr_delay_timer_applied and
           rrm_policy == rhs.rrm_policy;
  }
};

} // namespace srsran
