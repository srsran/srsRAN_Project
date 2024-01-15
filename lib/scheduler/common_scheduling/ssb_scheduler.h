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
#include "../config/cell_configuration.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/slot_point.h"
#include "srsran/scheduler/scheduler_slot_handler.h"

namespace srsran {

enum class ssb_pattern_case;
struct cell_slot_resource_allocator;

using ssb_information_list = srsran::static_vector<ssb_information, MAX_SSB_PER_SLOT>;

class ssb_scheduler
{
public:
  explicit ssb_scheduler(const cell_configuration& cfg_);

  /// \brief Schedule grant(s) for SSB.
  ///
  /// The functions schedules the SSB according to a given periodicity, depending on the frequency and SSB case for up
  /// to N slots when run for first time. In subsequent runs schedules SSB on N + 1 slot.
  ///
  /// \param[out] res_allocator  Cell resource grid.
  /// \param[in]  sl_point       Slot point carrying information about current slot.
  ///
  /// \remark This function only works for FR1, or L_max = 4 or 8.
  void run_slot(cell_resource_allocator& res_allocator, const slot_point& sl_point);

  /// \brief Schedule grant for SSB.
  ///
  /// The functions schedules the SSB per slot according to a given periodicity, depending on the frequency and
  /// SSB case.
  ///
  /// \param[out] slot_allocator Slot allocator object with SSB grants and DL PRBs.
  ///
  /// \remark This function only works for FR1, or L_max = 4 or 8.
  void schedule_ssb(cell_slot_resource_allocator& slot_allocator);

private:
  /// \brief Perform allocation for case A and C (both paired and unpaired spectrum) - TS 38.213, Section 4.1.
  void ssb_alloc_case_A_C(ssb_information_list& ssb_list, uint32_t freq_arfcn_cut_off, const slot_point& sl_point_mod);

  /// \brief Perform SSB allocation for case B (both paired and unpaired spectrum) - TS 38.213, Section 4.1.
  void ssb_alloc_case_B(ssb_information_list& ssb_list, const slot_point& sl_point_mod);

  static void fill_ssb_parameters(ssb_information_list& ssb_list,
                                  ssb_offset_to_pointA  offset_to_point_A,
                                  ssb_subcarrier_offset ssb_subc_offset,
                                  subcarrier_spacing    ssb_scs,
                                  subcarrier_spacing    scs_common,
                                  uint8_t               ssb_burst_symb_idx,
                                  uint8_t               ssb_idx);

  /// Flag indicating whether \c run_slot is called for the first time or not.
  bool first_run_slot{true};

  /// Periodicity of SSB in milliseconds (or in nof. subframes).
  uint8_t ssb_period;

  const cell_configuration& cell_cfg;
  srslog::basic_logger&     logger;
};

} // namespace srsran
