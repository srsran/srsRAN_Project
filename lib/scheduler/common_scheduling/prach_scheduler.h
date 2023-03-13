/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  /// Duration of the PRACH preamble in slots. For Long Format, it can be more than 1; for short format, it must be 1.
  unsigned prach_length_slots   = 1;
  unsigned start_slot_pusch_scs = 0;

  /// PRACH Configuration parameters derived from the cell configuration.
  prach_configuration prach_cfg;

  /// Bitset that defines whether PRACH is enabled for a given subframe.
  std::bitset<NOF_SUBFRAMES_PER_FRAME> prach_subframe_occasion_bitmap;

  /// Pre-generated PRACH occasions.
  static_vector<cached_prach_occasion, MAX_PRACH_OCCASIONS_PER_SLOT> cached_prachs;
};

} // namespace srsran