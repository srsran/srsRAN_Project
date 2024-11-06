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
