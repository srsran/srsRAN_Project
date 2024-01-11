/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ul_logical_channel_manager.h"

using namespace srsran;

ul_logical_channel_manager::ul_logical_channel_manager()
{
  set_status(uint_to_lcg_id(0), true);
}

/// \brief Update the configurations of the provided lists of bearers.
void ul_logical_channel_manager::configure(span<const logical_channel_config> log_channels_configs)
{
  for (unsigned i = 1; i != groups.size(); ++i) {
    groups[i].active = false;
  }
  for (const logical_channel_config& lc_ch : log_channels_configs) {
    set_status(lc_ch.lc_group, true);
  }
}

void ul_logical_channel_manager::deactivate()
{
  // Disable UL SRBs and DRBs.
  for (unsigned lcg_id = 0; lcg_id <= MAX_LCG_ID; lcg_id++) {
    set_status((lcg_id_t)lcg_id, false);
  }

  // Reset any pending SR.
  reset_sr_indication();
}
