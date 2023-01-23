/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "prach_scheduler.h"
#include "srsgnb/ran/frame_types.h"
#include "srsgnb/ran/prach/prach_cyclic_shifts.h"
#include "srsgnb/ran/prach/prach_frequency_mapping.h"
#include "srsgnb/ran/prach/prach_preamble_information.h"
#include "srsgnb/support/error_handling.h"

using namespace srsgnb;

prach_scheduler::prach_scheduler(const cell_configuration& cfg_) :
  cell_cfg(cfg_),
  logger(srslog::fetch_basic_logger("SCHED")),
  prach_cfg(prach_configuration_get(frequency_range::FR1,
                                    cell_cfg.paired_spectrum ? duplex_mode::FDD : duplex_mode::TDD,
                                    rach_cfg_common().rach_cfg_generic.prach_config_index))
{
  static const unsigned nof_symbols_per_slot = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.cp_extended
                                                   ? NOF_OFDM_SYM_PER_SLOT_EXTENDED_CP
                                                   : NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
  static const double   symbol_duration_msec =
      SUBFRAME_DURATION_MSEC * get_nof_slots_per_subframe(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs) /
      (double)nof_symbols_per_slot;

  // Convert list of PRACH subframe occasions to bitmap.
  for (unsigned pos : prach_cfg.subframe) {
    prach_subframe_occasion_bitmap.set(pos, true);
  }

  // Derive PRACH subcarrier spacing and other parameters.
  prach_preamble_information info;
  if (prach_cfg.format.is_long_preamble()) {
    // Long preamble.
    info = get_prach_preamble_long_info(prach_cfg.format);

  } else {
    // Short preamble.
    subcarrier_spacing scs = rach_cfg_common().msg1_scs != subcarrier_spacing::invalid
                                 ? rach_cfg_common().msg1_scs
                                 : cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs;
    switch (scs) {
      case subcarrier_spacing::kHz15:
        info.scs = prach_subcarrier_spacing::kHz15;
        break;
      case subcarrier_spacing::kHz30:
        info.scs = prach_subcarrier_spacing::kHz30;
        break;
      case subcarrier_spacing::kHz60:
        info.scs = prach_subcarrier_spacing::kHz60;
        break;
      case subcarrier_spacing::kHz120:
        info.scs = prach_subcarrier_spacing::kHz120;
        break;
      default:
        report_fatal_error("Invalid msg1-SCS");
    }
  }

  // Pre-compute and store potential PRACH occasions.
  unsigned nof_td_occasions = prach_cfg.format.is_long_preamble() ? 1 : prach_cfg.nof_occasions_within_slot;
  for (unsigned i = 0; i != nof_td_occasions; ++i) {
    for (unsigned id_fd_ra = 0; id_fd_ra != rach_cfg_common().rach_cfg_generic.msg1_fdm; ++id_fd_ra) {
      cached_prach_occasion& cached_prach = cached_prachs.emplace_back();

      // Pre-compute PRACH symbol x RB resources.
      cached_prach.grant_resources.scs = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs;
      if (prach_cfg.format < preamble_format::OTHER) {
        double   length_msecs                = (info.cp_length.to_seconds() + info.symbol_length.to_seconds()) * 1000;
        unsigned nof_symbols                 = ceil(length_msecs / symbol_duration_msec);
        cached_prach.grant_resources.symbols = {prach_cfg.starting_symbol, prach_cfg.starting_symbol + nof_symbols};
      } else {
        srsgnb_sanity_check(prach_cfg.duration > 0, "Invalid duration for short preamble");
        unsigned start                       = prach_cfg.starting_symbol + i * prach_cfg.duration;
        cached_prach.grant_resources.symbols = {start, start + prach_cfg.duration};
      }
      srsgnb_sanity_check(cached_prach.grant_resources.symbols.stop() <= nof_symbols_per_slot,
                          "Invalid PRACH preamble position");
      unsigned prach_nof_prbs =
          prach_frequency_mapping_get(info.scs, cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs).nof_rb_ra;
      uint8_t      prb_start = rach_cfg_common().rach_cfg_generic.msg1_frequency_start + id_fd_ra * prach_nof_prbs;
      prb_interval prach_prbs{prb_start, prb_start + prach_nof_prbs};
      cached_prach.grant_resources.crbs = prb_to_crb(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params, prach_prbs);

      // Pre-compute PRACH occasion parameters.
      cached_prach.occasion.pci                 = cell_cfg.pci;
      cached_prach.occasion.format              = prach_cfg.format;
      cached_prach.occasion.start_symbol        = prach_cfg.starting_symbol;
      cached_prach.occasion.nof_prach_occasions = prach_cfg.nof_occasions_within_slot;
      cached_prach.occasion.nof_cs              = prach_cyclic_shifts_get(
          info.scs, rach_cfg_common().restricted_set, rach_cfg_common().rach_cfg_generic.zero_correlation_zone_config);
      cached_prach.occasion.index_fd_ra = id_fd_ra;
      cached_prach.occasion.nof_fd_ra   = rach_cfg_common().rach_cfg_generic.msg1_fdm;
      // We assume all the preambles from 0 to 64 are assigned to the same PRACH occasion.
      // TODO: adapt scheduler to difference preamble indices intervals.
      cached_prach.occasion.start_preamble_index = 0;
      cached_prach.occasion.nof_preamble_indexes =
          cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->total_nof_ra_preambles;
    }
  }
}

void prach_scheduler::run_slot(cell_resource_allocator& res_grid)
{
  if (srsgnb_unlikely(first_slot_ind)) {
    // If called for the first time, pre-allocates the PRACH PDUs over the entire grid, until the last
    // (farthest in the future) usable slot.
    first_slot_ind = false;
    for (unsigned sl = 0; sl < cell_resource_allocator::RING_ALLOCATOR_SIZE; ++sl) {
      allocate_slot_prach_pdus(res_grid[sl]);
    }
    return;
  }

  // Pre-allocate PRACH PDU in the last slot.
  allocate_slot_prach_pdus(res_grid[cell_resource_allocator::RING_ALLOCATOR_SIZE - 1]);
}

void prach_scheduler::allocate_slot_prach_pdus(cell_slot_resource_allocator& sl_res_grid)
{
  static const unsigned nof_symbols_per_slot = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.cp_extended
                                                   ? NOF_OFDM_SYM_PER_SLOT_EXTENDED_CP
                                                   : NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;

  // Start slot within the subframe.
  unsigned start_slot = prach_cfg.starting_symbol / nof_symbols_per_slot;

  if (not cell_cfg.is_ul_enabled(sl_res_grid.slot)) {
    // UL is not enabled in this slot.
    return;
  }
  if (sl_res_grid.slot.sfn() % prach_cfg.x != prach_cfg.y) {
    // PRACH is not enabled in this SFN.
    return;
  }
  if (sl_res_grid.slot.subframe_slot_index() != start_slot) {
    // PRACH does not start in this slot.
    return;
  }
  if (not prach_subframe_occasion_bitmap.test(sl_res_grid.slot.subframe_index())) {
    // PRACH is not enabled in this subframe.
    return;
  }

  for (cached_prach_occasion& cached_prach : cached_prachs) {
    // Reserve RBs and symbols of the PRACH occasion in the resource grid.
    if (sl_res_grid.ul_res_grid.collides(cached_prach.grant_resources)) {
      logger.warning("Cannot allocate PRACH occasion. Cause: Lack of space in resource grid.");
      continue;
    }
    sl_res_grid.ul_res_grid.fill(cached_prach.grant_resources);

    // Add PRACH occasion to scheduler slot output.
    sl_res_grid.result.ul.prachs.push_back(cached_prach.occasion);
  }
}
