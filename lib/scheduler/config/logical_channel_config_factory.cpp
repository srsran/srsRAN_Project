/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/scheduler/config/logical_channel_config_factory.h"

using namespace srsgnb;

logical_channel_config srsgnb::config_helpers::create_default_logical_channel_config(lcid_t lcid)
{
  logical_channel_config lc_ch;
  lc_ch.lcid                      = lcid;
  lc_ch.priority                  = 1;
  lc_ch.lc_group                  = uint_to_lcg_id(0);
  lc_ch.lc_sr_mask                = false;
  lc_ch.lc_sr_delay_timer_applied = false;
  return lc_ch;
}
