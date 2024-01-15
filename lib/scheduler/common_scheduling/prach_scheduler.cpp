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

#include "prach_scheduler.h"
#include "srsran/ran/frame_types.h"
#include "srsran/ran/prach/prach_cyclic_shifts.h"
#include "srsran/ran/prach/prach_frequency_mapping.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/support/compiler.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

prach_scheduler::prach_scheduler(const cell_configuration& cfg_) :
  cell_cfg(cfg_),
  logger(srslog::fetch_basic_logger("SCHED")),
  prach_cfg(prach_configuration_get(frequency_range::FR1,
                                    cell_cfg.paired_spectrum ? duplex_mode::FDD : duplex_mode::TDD,
                                    rach_cfg_common().rach_cfg_generic.prach_config_index))
{
  // With SCS 15kHz and 30kHz, only normal CP is supported.
  static const unsigned nof_symbols_per_slot = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;

  // Convert list of PRACH subframe occasions to bitmap.
  for (const unsigned pos : prach_cfg.subframe) {
    prach_subframe_occasion_bitmap.set(pos, true);
  }

  prach_symbols_slots_duration prach_duration_info =
      get_prach_duration_info(prach_cfg, cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs);

  // Derive PRACH duration information.
  // The information we need are not related to whether it is the last PRACH occasion.
  const bool                       is_last_prach_occasion = false;
  const prach_preamble_information info =
      is_long_preamble(prach_cfg.format)
          ? get_prach_preamble_long_info(prach_cfg.format)
          : get_prach_preamble_short_info(
                prach_cfg.format,
                to_ra_subcarrier_spacing(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs),
                is_last_prach_occasion);
  start_slot_pusch_scs = prach_duration_info.start_slot_pusch_scs;
  prach_length_slots   = prach_duration_info.prach_length_slots;

  // This is a safety margin that prevents PUSCH REs to be decoded by the PRACH decoder with short PRACH formats.
  // TODO: remove them when the PHY is supports short PRACH with adjacent PUCCH/PUSCH.
  const unsigned prach_to_pusch_guardband = is_short_preamble(prach_cfg.format) ? 5U : 0U;

  for (unsigned id_fd_ra = 0; id_fd_ra != rach_cfg_common().rach_cfg_generic.msg1_fdm; ++id_fd_ra) {
    cached_prach_occasion& cached_prach = cached_prachs.emplace_back();

    const unsigned prach_nof_prbs =
        prach_frequency_mapping_get(info.scs, cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs).nof_rb_ra;
    // This is the start of the PRBs dedicated to PRACH.
    const uint8_t      prb_start = rach_cfg_common().rach_cfg_generic.msg1_frequency_start + id_fd_ra * prach_nof_prbs;
    const prb_interval prach_prbs{prb_start, prb_start + prach_nof_prbs + prach_to_pusch_guardband};
    const crb_interval crbs = prb_to_crb(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params, prach_prbs);

    if (is_long_preamble(prach_cfg.format)) {
      // If the PRACH preamble is longer than 1 slot, then allocate a grant for each slot that include the preamble.
      for (unsigned prach_slot_idx = 0; prach_slot_idx < prach_length_slots; ++prach_slot_idx) {
        // For the first slot, use the start_symbol_pusch_scs; in any other case, the preamble starts from the initial
        // symbol. For the last slot, compute the final symbol; in any other case, the preamble ends at the last slot's
        // symbol.
        const ofdm_symbol_range prach_symbols{
            prach_slot_idx == 0 ? prach_duration_info.start_symbol_pusch_scs : 0,
            prach_slot_idx < prach_length_slots - 1
                ? nof_symbols_per_slot
                : (prach_duration_info.start_symbol_pusch_scs + prach_duration_info.nof_symbols) %
                      nof_symbols_per_slot};
        cached_prach.grant_list.emplace_back(
            grant_info{cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs, prach_symbols, crbs});
      }
    } else {
      const ofdm_symbol_range prach_symbols{prach_duration_info.start_symbol_pusch_scs,
                                            prach_duration_info.start_symbol_pusch_scs +
                                                prach_duration_info.nof_symbols};
      // If the burst of PRACH opportunities extends over 1 slot, then allocate a grant for each slot that include these
      // opportunities (1 or 2 slots).
      for (unsigned prach_slot_idx = 0; prach_slot_idx < prach_length_slots; ++prach_slot_idx) {
        // For the short PRACH formats, both grants (if more than 1) occupy the same symbols within the slot.
        cached_prach.grant_list.emplace_back(
            grant_info{cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs, prach_symbols, crbs});
      }
    }

    // Pre-compute PRACH occasion parameters.
    cached_prach.occasion.pci    = cell_cfg.pci;
    cached_prach.occasion.format = prach_cfg.format;
    // Note: the Starting Symbol that is required for the PHY interface is actually the one given by Table 6.3.3.2-2
    // and 6.3.3.2-3, TS 38.211.
    cached_prach.occasion.start_symbol        = prach_cfg.starting_symbol;
    cached_prach.occasion.nof_prach_occasions = prach_cfg.nof_occasions_within_slot;
    cached_prach.occasion.nof_cs              = prach_cyclic_shifts_get(
        info.scs, rach_cfg_common().restricted_set, rach_cfg_common().rach_cfg_generic.zero_correlation_zone_config);
    cached_prach.occasion.index_fd_ra = id_fd_ra;
    cached_prach.occasion.nof_fd_ra   = rach_cfg_common().rach_cfg_generic.msg1_fdm;
    // We assume all the preambles from 0 to 64 are assigned to the same PRACH occasion.
    // TODO: adapt scheduler to difference preamble indices intervals.
    cached_prach.occasion.start_preamble_index = 0;
    if (cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->total_nof_ra_preambles.has_value()) {
      cached_prach.occasion.nof_preamble_indexes =
          cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->total_nof_ra_preambles.value();
    } else {
      // If the field is absent, all possible preambles are available for RA.
      cached_prach.occasion.nof_preamble_indexes = MAX_NOF_PRACH_PREAMBLES;
    }
  }
}

void prach_scheduler::run_slot(cell_resource_allocator& res_grid)
{
  if (SRSRAN_UNLIKELY(first_slot_ind)) {
    // If called for the first time, pre-allocates the PRACH PDUs over the entire grid, until the last
    // (farthest in the future) usable slot.
    first_slot_ind = false;
    // NOTE: Min value of cell_resource_allocator.max_ul_slot_alloc_delay is 20, max value of prach_length_slots is 7.
    for (unsigned sl = 0; sl < (res_grid.max_ul_slot_alloc_delay - prach_length_slots + 1); ++sl) {
      allocate_slot_prach_pdus(res_grid, res_grid[sl].slot);
    }
    return;
  }

  // Pre-allocate PRACH PDU in the last slot.
  allocate_slot_prach_pdus(res_grid, res_grid[res_grid.max_ul_slot_alloc_delay - prach_length_slots].slot);
}

void prach_scheduler::allocate_slot_prach_pdus(cell_resource_allocator& res_grid, slot_point sl)
{
  // If any of the slots over which the PRACH preamble should be allocated isn't an UL slot, return.
  for (unsigned sl_idx = 0; sl_idx < prach_length_slots; ++sl_idx) {
    if (not cell_cfg.is_fully_ul_enabled(sl + sl_idx)) {
      return;
    }
  }

  if (sl.sfn() % prach_cfg.x != prach_cfg.y) {
    // PRACH is not enabled in this SFN.
    return;
  }
  if (sl.subframe_slot_index() != start_slot_pusch_scs) {
    // PRACH does not start in this slot.
    return;
  }
  if (not prach_subframe_occasion_bitmap.test(sl.subframe_index())) {
    // PRACH is not enabled in this subframe.
    return;
  }

  for (const cached_prach_occasion& cached_prach : cached_prachs) {
    if (is_long_preamble(prach_cfg.format)) {
      // Reserve RBs and symbols of the PRACH occasion in the resource grid for each grant (over multiple slots) of the
      // preamble.
      for (unsigned sl_idx = 0; sl_idx < prach_length_slots; ++sl_idx) {
        res_grid[sl + sl_idx].ul_res_grid.fill(cached_prach.grant_list[sl_idx]);
      }

      // Add PRACH occasion to scheduler slot output (one PRACH PDU per preamble).
      res_grid[sl].result.ul.prachs.push_back(cached_prach.occasion);
    } else {
      // Reserve RBs and symbols of the PRACH occasion in the resource grid for each grant (over 1 or 2 slots) of the
      // preamble and add the PRACH occasions to scheduler slot output (1 or 2 PRACH PDU per burst of PRACH
      // opportunities, depending on whether this burst repeats over 1 or 2 slots).
      for (unsigned sl_idx = 0; sl_idx < prach_length_slots; ++sl_idx) {
        res_grid[sl + sl_idx].ul_res_grid.fill(cached_prach.grant_list[sl_idx]);
        res_grid[sl + sl_idx].result.ul.prachs.push_back(cached_prach.occasion);
      }
    }
  }
}
