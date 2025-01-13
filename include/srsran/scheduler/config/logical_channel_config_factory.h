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

#include "srsran/scheduler/config/logical_channel_config.h"

namespace srsran {
namespace config_helpers {

/// \brief Creates a default logical channel configuration to be used by the scheduler.
constexpr logical_channel_config create_default_logical_channel_config(lcid_t lcid)
{
  logical_channel_config lc_ch{};
  lc_ch.lcid = lcid;
  // See TS 38.331, 9.2.1 Default SRB configurations.
  if (is_srb(lcid)) {
    lc_ch.lc_group = uint_to_lcg_id(0);
  } else {
    lc_ch.lc_group = uint_to_lcg_id(2);
  }
  lc_ch.lc_sr_mask                = false;
  lc_ch.lc_sr_delay_timer_applied = false;
  return lc_ch;
}

} // namespace config_helpers
} // namespace srsran
