/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_SCHEDULER_PDCCH_SCHEDULING_PDCCH_CONFIG_HELPERS_H
#define SRSGNB_LIB_SCHEDULER_PDCCH_SCHEDULING_PDCCH_CONFIG_HELPERS_H

#include "srsgnb/ran/slot_point.h"
#include "srsgnb/scheduler/bwp_configuration.h"

namespace srsgnb {

/// \brief Helper function to check whether a SearchSpace PDCCH is monitored by the UE for a given slot.
///
/// \param sl Slot in which the SearchSpace is being monitored.
/// \param ss_cfg SearchSpace configuration.
/// \return true if the SearchSpace is being monitored. False, otherwise.
inline bool is_pdcch_monitoring_active(slot_point sl, const search_space_configuration& ss_cfg)
{
  unsigned slot_offset  = sl.to_uint() % ss_cfg.monitoring_slot_period;
  unsigned window_start = ss_cfg.monitoring_slot_offset;
  unsigned window_end   = (window_start + ss_cfg.duration) % ss_cfg.monitoring_slot_period;

  // Checks whether slot_offset falls [window_start, window_end), taking into account the wrap-around of "window_end".
  if (window_start < window_end) {
    return slot_offset >= window_start and slot_offset < window_end;
  }
  return slot_offset >= window_start or slot_offset < window_end;
}

} // namespace srsgnb

#endif // SRSGNB_LIB_SCHEDULER_PDCCH_SCHEDULING_PDCCH_CONFIG_HELPERS_H
