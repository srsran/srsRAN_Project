/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../cell/resource_grid.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/ran/slot_point.h"
#include <bitset>

namespace srsran {

class prach_scheduler
{
public:
  explicit prach_scheduler(const cell_configuration& cfg_);

  /// Allocate RACH PDUs for the given slot.
  void run_slot(cell_resource_allocator& res_grid);

private:
  struct cached_prach_occasion {
    /// RB x symbol resources used for the PRACH.
    grant_info grant_resources;
    /// Pre-generated PRACH occasion.
    prach_occasion_info occasion;
  };

  const rach_config_common& rach_cfg_common() const { return *cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common; }

  void allocate_slot_prach_pdus(cell_slot_resource_allocator& sl_res_grid);

  const cell_configuration& cell_cfg;
  srslog::basic_logger&     logger;

  bool first_slot_ind = true;

  /// PRACH Configuration parameters derived from the cell configuration.
  prach_configuration prach_cfg;

  /// Bitset that defines whether PRACH is enabled for a given subframe.
  std::bitset<NOF_SUBFRAMES_PER_FRAME> prach_subframe_occasion_bitmap;

  /// Pre-generated PRACH occasions.
  static_vector<cached_prach_occasion, MAX_PRACH_OCCASIONS_PER_SLOT> cached_prachs;
};

} // namespace srsran