/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
