/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cell/resource_grid.h"
#include "srsgnb/ran/prach/prach_configuration.h"
#include "srsgnb/ran/slot_point.h"
#include <bitset>

namespace srsgnb {

class prach_scheduler
{
public:
  explicit prach_scheduler(const cell_configuration& cfg_);

  /// Allocate RACH PDUs for the given slot.
  void run_slot(cell_slot_resource_allocator& res_grid);

private:
  const cell_configuration& cell_cfg;
  srslog::basic_logger&     logger;

  /// PRACH Configuration parameters derived from the cell configuration.
  prach_configuration prach_cfg;

  /// Bitset that defines whether PRACH is enabled for a given subframe.
  std::bitset<NOF_SUBFRAMES_PER_FRAME> prach_subframe_occasion_bitmap;

  /// RB x symbol resources used for PRACHs.
  grant_info prach_grant_resources;
};

} // namespace srsgnb