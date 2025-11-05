/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/ran/slot_point.h"
#include "srsran/srslog/logger.h"
#include <bitset>

namespace srsran {

class prach_scheduler
{
public:
  explicit prach_scheduler(const cell_configuration& cfg_);

  /// Allocate RACH PDUs for the given slot.
  void run_slot(cell_resource_allocator& res_grid);

  void stop();

private:
  // [Implementation-defined] This is the maximum number of slots per PRACH preamble. It's obtained by considering the
  // longest preamble length (which can be derived from Table 6.3.3.1-1, TS 38.211 for Format 2) and the shortest slot
  // duration currently supported by the GNB, which is 0.5ms for SCS 30KHz.
  static constexpr unsigned MAX_SLOTS_PER_PRACH = 7;
  /// Maximum PRACH SFN opportunity period. Maximum value for the parameter \f$x\f$. This value is deduced from TS
  /// 38.211 Tables 6.3.3.2-2, 6.3.3.2-3, and 6.3.3.2-4.
  static constexpr unsigned MAX_PRACH_SFN_PERIOD = 16;
  /// Maximum subcarrier spacing limited by the maximum SS/PBCH block subcarrier spacing for FR2 given in the TS
  /// 38.104 Table 5.4.3.3-2).
  static constexpr subcarrier_spacing MAX_COMMON_SCS = subcarrier_spacing::kHz240;
  /// Maximum number of slots that are contained in a system frame.
  static constexpr unsigned MAX_NOF_PRACH_SLOTS_PER_FRAME =
      NOF_SUBFRAMES_PER_FRAME * pow2(to_numerology_value(MAX_COMMON_SCS));

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
  /// Set to true if the selected preamble is long.
  bool use_long_preamble = false;
  /// Starting slot within the reference slot. Parameter \f$n_{slot}^{RA}\f$; as described in TS 38.211 Section 5.3.2.
  unsigned start_slot_pusch_scs = 0;
  /// For long PRACH preamble formats, this is the duration of the PRACH preamble in slots, which can be more than 1.
  /// For short PRACH preamble formats, this is the duration of the burst of PRACH opportunities, which can be 1 or 2
  /// slots, as per Section 5.3.2, and Tables 6.3.3.2-2 and 6.3.3.2-3, TS 38.211.
  unsigned prach_length_slots = 1;
  /// Set of system frame numbers (SFN) that satisfy the condition \f$n_{SFN} \bmod x = y\f$.
  std::bitset<MAX_PRACH_SFN_PERIOD> prach_sfn_occasions;
  /// PRACH system frame (SFN) period. Parameter \f$x\f$ in TS 38.211 Section 6.3.3.2.
  unsigned prach_sfn_period;
  /// Set of the slots that contain PRACH occasions within a system frame.
  std::bitset<MAX_NOF_PRACH_SLOTS_PER_FRAME> prach_slot_occasions;
  /// Pre-generated PRACH occasions.
  static_vector<cached_prach_occasion, MAX_PRACH_OCCASIONS_PER_SLOT> cached_prachs;
};

} // namespace srsran
