/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/adt/optional.h"
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
