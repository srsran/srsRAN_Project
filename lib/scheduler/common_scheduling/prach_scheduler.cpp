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

using namespace srsgnb;

prach_scheduler::prach_scheduler(const cell_configuration& cfg_) :
  cell_cfg(cfg_),
  logger(srslog::fetch_basic_logger("MAC")),
  prach_cfg(
      prach_configuration_get(frequency_range::FR1,
                              cell_cfg.paired_spectrum ? duplex_mode::FDD : duplex_mode::TDD,
                              cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index))
{
  // Convert list of PRACH subframe occasions to bitmap.
  for (unsigned pos : prach_cfg.subframe) {
    prach_subframe_occasion_bitmap.set(pos, true);
  }

  // Derive PRACH subcarrier spacing.
  if (prach_cfg.format < preamble_format::OTHER) {
    if (prach_cfg.format < preamble_format::FORMAT3) {
      prach_scs = prach_subcarrier_spacing::kHz1_25;
    } else {
      prach_scs = prach_subcarrier_spacing::kHz5;
    }
  } else {
    subcarrier_spacing scs = rach_cfg_common().msg1_scs != subcarrier_spacing::invalid
                                 ? rach_cfg_common().msg1_scs
                                 : cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs;
    switch (scs) {
      case subcarrier_spacing::kHz15:
        prach_scs = prach_subcarrier_spacing::kHz15;
        break;
      case subcarrier_spacing::kHz30:
        prach_scs = prach_subcarrier_spacing::kHz30;
        break;
      case subcarrier_spacing::kHz60:
        prach_scs = prach_subcarrier_spacing::kHz60;
        break;
      case subcarrier_spacing::kHz120:
        prach_scs = prach_subcarrier_spacing::kHz120;
        break;
      default:
        srsgnb_terminate("Invalid msg1-SCS");
    }
  }

  // Compute CRBs reserved for PRACHs.
  static const unsigned PRACH_NOF_PRBS = 6U; // TODO: Derive this value.
  uint8_t               prb_start      = rach_cfg_common().rach_cfg_generic.msg1_frequency_start;
  prb_interval          prach_prbs{prb_start, prb_start + PRACH_NOF_PRBS};
  crb_interval          prach_crbs = prb_to_crb(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params, prach_prbs);
  prach_grant_resources.ch         = grant_info::channel::prach;
  prach_grant_resources.scs        = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs;
  if (prach_cfg.duration == 0) {
    prach_grant_resources.symbols = {prach_cfg.starting_symbol, prach_cfg.starting_symbol + 1};
  } else {
    prach_grant_resources.symbols = {prach_cfg.starting_symbol,
                                     prach_cfg.starting_symbol +
                                         prach_cfg.duration * prach_cfg.nof_occasions_within_slot};
  }
  prach_grant_resources.crbs = prach_crbs;
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

  const rach_config_common&  rrc_rach_cfg         = *cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common;
  const rach_config_generic& rrc_rach_cfg_generic = rrc_rach_cfg.rach_cfg_generic;

  // Reserve RB-symbol resources for PRACH.
  if (slot_res_grid.ul_res_grid.collides(prach_grant_resources)) {
    logger.warning("SCHED: Cannot allocate PRACH occasion. Cause: CRBs={},symbols={} already used.",
                   prach_grant_resources.crbs,
                   prach_grant_resources.symbols);
    return;
  }
  slot_res_grid.ul_res_grid.fill(prach_grant_resources);

  // Add new PRACH occasion in slot scheduler result.
  slot_res_grid.result.ul.prachs.emplace_back();
  prach_occasion_info& prach_occ = slot_res_grid.result.ul.prachs.back();
  prach_occ.format               = prach_cfg.format;
  prach_occ.start_symbol         = prach_cfg.starting_symbol;
  prach_occ.nof_prach_occasions  = prach_cfg.nof_occasions_within_slot;
  prach_occ.nof_cs               = prach_cyclic_shifts_get(
      prach_scs, rach_cfg_common().restricted_set, rrc_rach_cfg_generic.zero_correlation_zone_config);
  // TODO: How to derive indexFdRa.
  prach_occ.fd_ra_resources      = {0, rrc_rach_cfg_generic.msg1_fdm};
  prach_occ.prach_config_index   = rrc_rach_cfg_generic.prach_config_index;
  prach_occ.start_preamble_index = cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->prach_root_seq_index;
  prach_occ.nof_preamble_indexes = cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->total_nof_ra_preambles;
}
