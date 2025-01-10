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

ul_logical_channel_manager::ul_logical_channel_manager(subcarrier_spacing scs) :
  slots_per_sec(get_nof_slots_per_subframe(scs) * 1000)
{
  set_status(uint_to_lcg_id(0), true);
}

void ul_logical_channel_manager::slot_indication()
{
  // Update the bit rates of the UE logical channel groups with tracked bit rates.
  for (unsigned i = 0, e = groups.size(); i < e; i++) {
    if (groups[i].avg_bytes_per_slot.size() > 0) {
      groups[i].avg_bytes_per_slot.push(groups[i].last_sched_bytes);
      groups[i].last_sched_bytes = 0;
    }
  }
}

/// \brief Update the configurations of the provided lists of bearers.
void ul_logical_channel_manager::configure(span<const logical_channel_config> log_channels_configs)
{
  for (unsigned i = 1; i != groups.size(); ++i) {
    groups[i].active = false;
  }
  for (const logical_channel_config& lc_ch : log_channels_configs) {
    set_status(lc_ch.lc_group, true);
    if (lc_ch.qos.has_value() and lc_ch.qos.value().gbr_qos_info.has_value()) {
      // Track average rate for GBR logical channel groups.
      unsigned win_size_msec = lc_ch.qos.value().qos.average_window_ms.value_or(2000);
      groups[lc_ch.lc_group].avg_bytes_per_slot.resize(win_size_msec * slots_per_sec / 1000);
    }
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

void ul_logical_channel_manager::handle_ul_grant(unsigned grant_size)
{
  for (unsigned i = 0, e = groups.size(); i != e and grant_size > 0; i++) {
    if (groups[i].active and groups[i].buf_st > groups[i].sched_bytes_accum) {
      unsigned bytes_sched = std::min(groups[i].buf_st - groups[i].sched_bytes_accum, grant_size);
      groups[i].last_sched_bytes += bytes_sched;
      groups[i].sched_bytes_accum += bytes_sched;
      grant_size -= bytes_sched;
    }
  }
}
