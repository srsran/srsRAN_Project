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

#include "../cell/cell_configuration.h"
#include "../cell/resource_grid.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"

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
  /// Size of the ring buffer of \c cell_slot_resource_allocator. This size sets a limit on how far in advance a SSB can
  /// be allocated. See remark of \c get_allocator_ring_size_ge_min(unsigned minimum_value).
  static const size_t RING_ALLOCATOR_SIZE =
      get_allocator_ring_size_gt_min(std::max(SCHEDULER_MAX_K0 + SCHEDULER_MAX_K1, SCHEDULER_MAX_K2 + MAX_MSG3_DELTA));

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
