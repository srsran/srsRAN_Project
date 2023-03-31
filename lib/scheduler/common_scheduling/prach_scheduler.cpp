/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  srsran_assert(is_long_preamble(prach_cfg.format), "Short PRACH preamble not supported");

  // With SCS 15kHz and 30kHz, only normal CP is supported.
  static const unsigned nof_symbols_per_slot = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;

  // Convert list of PRACH subframe occasions to bitmap.
  for (const unsigned pos : prach_cfg.subframe) {
    prach_subframe_occasion_bitmap.set(pos, true);
  }

  prach_symbols_slots_duration prach_duration_info =
      get_prach_duration_info(prach_cfg, cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs);

  // Derive PRACH duration information.
  const prach_preamble_information info = get_prach_preamble_long_info(prach_cfg.format);
  start_slot_pusch_scs                  = prach_duration_info.start_slot_pusch_scs;
  prach_length_slots                    = prach_duration_info.prach_length_slots;

  for (unsigned id_fd_ra = 0; id_fd_ra != rach_cfg_common().rach_cfg_generic.msg1_fdm; ++id_fd_ra) {
    cached_prach_occasion& cached_prach = cached_prachs.emplace_back();

    const unsigned prach_nof_prbs =
        prach_frequency_mapping_get(info.scs, cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs).nof_rb_ra;
    const uint8_t      prb_start = rach_cfg_common().rach_cfg_generic.msg1_frequency_start + id_fd_ra * prach_nof_prbs;
    const prb_interval prach_prbs{prb_start, prb_start + prach_nof_prbs};
    const crb_interval crbs = prb_to_crb(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params, prach_prbs);

    // If the PRACH preamble is longer than 1 slot, then allocate a grant for each slot that include the preamble.
    for (unsigned prach_slot_idx = 0; prach_slot_idx < prach_length_slots; ++prach_slot_idx) {
      // For the first slot, use the start_symbol_pusch_scs; in any other case, the preamble starts from the initial
      // symbol. For the last slot, compute the final symbol; in any other case, the preamble ends at the last slot's
      // symbol.
      const ofdm_symbol_range prach_symbols{
          prach_slot_idx == 0 ? prach_duration_info.start_symbol_pusch_scs : 0,
          prach_slot_idx < prach_length_slots - 1
              ? nof_symbols_per_slot
              : (prach_duration_info.start_symbol_pusch_scs + prach_duration_info.nof_symbols) % nof_symbols_per_slot};
      cached_prach.grant_list.emplace_back(
          grant_info{cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs, prach_symbols, crbs});
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
    // NOTE: Min value of cell_resource_allocator::RING_ALLOCATOR_SIZE is 20, max value of prach_length_slots is 7.
    for (unsigned sl = 0; sl < (cell_resource_allocator::RING_ALLOCATOR_SIZE - prach_length_slots + 1); ++sl) {
      allocate_slot_prach_pdus(res_grid, res_grid[sl].slot);
    }
    return;
  }

  // Pre-allocate PRACH PDU in the last slot.
  allocate_slot_prach_pdus(res_grid, res_grid[cell_resource_allocator::RING_ALLOCATOR_SIZE - prach_length_slots].slot);
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
    for (unsigned sl_idx = 0; sl_idx < prach_length_slots; ++sl_idx) {
      // Check if any of the grants (over multiple slots) of the preamble collides with other allocations.
      if (res_grid[sl + sl_idx].ul_res_grid.collides(cached_prach.grant_list[sl_idx])) {
        logger.warning("Cannot allocate PRACH occasion. Cause: Lack of space in resource grid.");
        continue;
      }
    }

    // Reserve RBs and symbols of the PRACH occasion in the resource grid for each grant (over multiple slots) of the
    // preamble.
    for (unsigned sl_idx = 0; sl_idx < prach_length_slots; ++sl_idx) {
      res_grid[sl + sl_idx].ul_res_grid.fill(cached_prach.grant_list[sl_idx]);
    }

    // Add PRACH occasion to scheduler slot output (one PRACH PDU per preamble).
    res_grid[sl].result.ul.prachs.push_back(cached_prach.occasion);
  }
}
