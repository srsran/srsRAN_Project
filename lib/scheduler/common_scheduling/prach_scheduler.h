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
  // [Implementation-defined] This is the maximum number of slots per PRACH preamble. It's obtained by considering the
  // longest preamble length (which can be derived from Table 6.3.3.1-1, TS 38.211 for Format 2) and the shortest slot
  // duration currently supported by the GNB, which is 0.5ms for SCS 30KHz.
  static const unsigned MAX_SLOTS_PER_PRACH = 7;

  // The maximum number of PRACH preamble indexes that the UE can choose from.
  static const unsigned MAX_NOF_PRACH_PREAMBLES = 64;

  struct cached_prach_occasion {
    /// RB x symbol resources used for the PRACH.
    static_vector<grant_info, MAX_SLOTS_PER_PRACH> grant_list;
    /// Pre-generated PRACH occasion.
    prach_occasion_info occasion;
  };

  const rach_config_common& rach_cfg_common() const { return *cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common; }

  void allocate_slot_prach_pdus(cell_resource_allocator& res_grid, slot_point sl);

  const cell_configuration& cell_cfg;
  srslog::basic_logger&     logger;

  bool first_slot_ind = true;
  /// This is the first symbol within a slot (with reference to the PUSCH SCS) where the preamble (for long formats) or
  /// the first preamble of a burst of PRACH opportunities (for short formats) starts.
  unsigned start_slot_pusch_scs = 0;
  /// For long PRACH preamble formats, this is the duration of the PRACH preamble in slots, which can be more than 1.
  /// For short PRACH preamble formats, this is the duration of the burst of PRACH opportunities, which can be 1 or 2
  /// slots, as per Section 5.3.2, and Tables 6.3.3.2-2 and 6.3.3.2-3, TS 38.211.
  unsigned prach_length_slots = 1;

  /// PRACH Configuration parameters derived from the cell configuration.
  prach_configuration prach_cfg;

  /// Bitset that defines whether PRACH is enabled for a given subframe.
  std::bitset<NOF_SUBFRAMES_PER_FRAME> prach_subframe_occasion_bitmap;

  /// Pre-generated PRACH occasions.
  static_vector<cached_prach_occasion, MAX_PRACH_OCCASIONS_PER_SLOT> cached_prachs;
};

} // namespace srsran