/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PDCCH_HELPERS_H
#define SRSGNB_PDCCH_HELPERS_H

#include "srsgnb/ran/slot_point.h"
#include "srsgnb/scheduler/bwp_configuration.h"

namespace srsgnb {

inline bool is_pdcch_monitoring_active(slot_point sl, const search_space_configuration& ss_cfg)
{
  unsigned slot_offset = sl.to_uint() % ss_cfg.monitoring_slot_period;
  unsigned lower_lim   = ss_cfg.monitoring_slot_offset;
  unsigned upper_lim   = (lower_lim + ss_cfg.duration) % ss_cfg.monitoring_slot_period;

  if (lower_lim < upper_lim) {
    return slot_offset >= lower_lim and slot_offset < upper_lim;
  }
  return slot_offset >= lower_lim or slot_offset < upper_lim;
}

} // namespace srsgnb

#endif // SRSGNB_PDCCH_HELPERS_H
