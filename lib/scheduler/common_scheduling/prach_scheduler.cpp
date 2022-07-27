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
#include "srsgnb/ran/prach/prach_cyclic_shifts.h"
#include "srsgnb/ran/prach/prach_preamble_information.h"

using namespace srsgnb;

prach_scheduler::prach_scheduler(const cell_configuration& cfg_) :
  cell_cfg(cfg_),
  logger(srslog::fetch_basic_logger("MAC")),
  prach_cfg(prach_configuration_get(frequency_range::FR1,
                                    cell_cfg.paired_spectrum ? duplex_mode::FDD : duplex_mode::TDD,
                                    rach_cfg_common().rach_cfg_generic.prach_config_index))
{
  static constexpr unsigned SUBFRAME_DURATION_MSEC = 1;
  static const double       symbol_duration_msec =
      SUBFRAME_DURATION_MSEC * get_nof_slots_per_subframe(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs) /
      (double)(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.cp_extended ? NOF_OFDM_SYM_PER_SLOT_EXTENDED_CP
                                                                             : NOF_OFDM_SYM_PER_SLOT_NORMAL_CP);

  // Convert list of PRACH subframe occasions to bitmap.
  for (unsigned pos : prach_cfg.subframe) {
    prach_subframe_occasion_bitmap.set(pos, true);
  }

  // Derive PRACH subcarrier spacing and other parameters.
  prach_preamble_information info;
  if (prach_cfg.format < preamble_format::OTHER) {
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
        srsgnb_terminate("Invalid msg1-SCS");
    }
  }

  // Pre-compute and store potential PRACH occasions.
  unsigned nof_occasions = prach_cfg.format < preamble_format::OTHER ? 1 : prach_cfg.nof_occasions_within_slot;
  for (unsigned i = 0; i != nof_occasions; ++i) {
    cached_prachs.emplace_back();
    cached_prach_occasion& cached_prach = cached_prachs.back();

    // Compute PRACH CRB x symbols resources.
    static const unsigned PRACH_NOF_PRBS = 6U; // TODO: Derive this value.
    uint8_t               prb_start      = rach_cfg_common().rach_cfg_generic.msg1_frequency_start;
    prb_interval          prach_prbs{prb_start, prb_start + PRACH_NOF_PRBS};
    crb_interval          prach_crbs  = prb_to_crb(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params, prach_prbs);
    cached_prach.grant_resources.ch   = grant_info::channel::prach;
    cached_prach.grant_resources.scs  = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs;
    cached_prach.grant_resources.crbs = prach_crbs;
    if (prach_cfg.format < preamble_format::OTHER) {
      double   length_msecs                = (info.cp_length.to_seconds() + info.symbol_length.to_seconds()) * 1000;
      unsigned nof_symbols                 = ceil(length_msecs / symbol_duration_msec);
      cached_prach.grant_resources.symbols = {prach_cfg.starting_symbol, prach_cfg.starting_symbol + nof_symbols};
    } else {
      srsgnb_sanity_check(prach_cfg.duration > 0, "Invalid duration for short preamble");
      unsigned start                       = prach_cfg.starting_symbol + i * prach_cfg.duration;
      cached_prach.grant_resources.symbols = {start, start + prach_cfg.duration};
    }

    // Pre-compute PRACH occasion parameters.
    cached_prach.occasion.format              = prach_cfg.format;
    cached_prach.occasion.start_symbol        = prach_cfg.starting_symbol;
    cached_prach.occasion.nof_prach_occasions = prach_cfg.nof_occasions_within_slot;
    cached_prach.occasion.nof_cs              = prach_cyclic_shifts_get(
        info.scs, rach_cfg_common().restricted_set, rach_cfg_common().rach_cfg_generic.zero_correlation_zone_config);
    // TODO: How to derive indexFdRa.
    cached_prach.occasion.fd_ra_resources    = {0, rach_cfg_common().rach_cfg_generic.msg1_fdm};
    cached_prach.occasion.prach_config_index = rach_cfg_common().rach_cfg_generic.prach_config_index;
    cached_prach.occasion.start_preamble_index =
        cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->prach_root_seq_index;
    cached_prach.occasion.nof_preamble_indexes =
        cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->total_nof_ra_preambles;
  }
}

void prach_scheduler::run_slot(cell_slot_resource_allocator& slot_res_grid)
{
  slot_point sl = slot_res_grid.slot;

  if (sl.sfn() % prach_cfg.x != prach_cfg.y) {
    // PRACH is not enabled in this SFN.
    return;
  }
  if (not prach_subframe_occasion_bitmap.test(sl.subframe_index())) {
    // PRACH is not enabled in this subframe.
    return;
  }

  for (cached_prach_occasion& cached_prach : cached_prachs) {
    // Reserve RB vs symbol resources for PRACH in cell resource grid.
    if (slot_res_grid.ul_res_grid.collides(cached_prach.grant_resources)) {
      logger.warning("SCHED: Cannot allocate PRACH occasion. Cause: CRBs={},symbols={} already used.",
                     cached_prach.grant_resources.crbs,
                     cached_prach.grant_resources.symbols);
      continue;
    }
    slot_res_grid.ul_res_grid.fill(cached_prach.grant_resources);

    // Add PRACH occasion to scheduler slot output.
    slot_res_grid.result.ul.prachs.push_back(cached_prach.occasion);
  }
}
