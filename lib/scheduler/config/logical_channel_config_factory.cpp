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

#include "srsran/scheduler/config/logical_channel_config_factory.h"

using namespace srsran;

logical_channel_config srsran::config_helpers::create_default_logical_channel_config(lcid_t lcid)
{
  logical_channel_config lc_ch;
  lc_ch.lcid                      = lcid;
  lc_ch.priority                  = is_srb(lcid) ? 1 : 2;
  lc_ch.lc_group                  = uint_to_lcg_id(is_srb(lcid) ? 0 : 1);
  lc_ch.lc_sr_mask                = false;
  lc_ch.lc_sr_delay_timer_applied = false;
  return lc_ch;
}
