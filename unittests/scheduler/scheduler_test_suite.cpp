/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "scheduler_test_suite.h"
#include "lib/scheduler/cell/resource_grid.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_config_helpers.h"
#include "lib/scheduler/support/config_helpers.h"
#include "srsgnb/ran/prach/prach_configuration.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

void srsgnb::test_pdcch_pdsch_common_consistency(const cell_configuration&   cell_cfg,
                                                 const pdcch_dl_information& pdcch,
                                                 const pdsch_information&    pdsch)
{
  TESTASSERT_EQ(pdcch.ctx.rnti, pdsch.rnti);
  TESTASSERT(pdcch.ctx.bwp_cfg == pdsch.bwp_cfg);

  uint8_t time_assignment = 0;
  switch (pdcch.dci.type) {
    case dci_dl_rnti_config_type::si_f1_0: {
      time_assignment = pdcch.dci.si_f1_0.time_resource;
      break;
    }
    case dci_dl_rnti_config_type::ra_f1_0: {
      time_assignment = pdcch.dci.ra_f1_0.time_resource;
    } break;
    default:
      srsran_terminate("not supported");
  }
  ofdm_symbol_range symbols =
      cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[time_assignment].symbols;
  TESTASSERT(symbols == pdsch.symbols, "Mismatch of time-domain resource assignment and PDSCH symbols");
}

void srsgnb::test_pdsch_sib_consistency(const cell_configuration& cell_cfg, span<const sib_information> sibs)
{
  bool has_coreset0 = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.has_value();
  if (not has_coreset0) {
    TESTASSERT(sibs.empty(), "SIB1 cannot be scheduled without CORESET#0");
    return;
  }

  bwp_configuration effective_init_bwp_cfg = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params;
  effective_init_bwp_cfg.crbs              = get_coreset0_crbs(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common);

  for (const sib_information& sib : sibs) {
    TESTASSERT(sib.pdsch_cfg.prbs.is_alloc_type1());
    prb_interval prbs = sib.pdsch_cfg.prbs.prbs();
    TESTASSERT(prbs.stop() <= effective_init_bwp_cfg.crbs.length(), "PRB grant falls outside CORESET#0 RB boundaries");
    if (sib.pdcch_cfg != nullptr) {
      test_pdcch_pdsch_common_consistency(cell_cfg, *sib.pdcch_cfg, sib.pdsch_cfg);
    }
  }
}

void srsgnb::test_dl_resource_grid_collisions(const cell_configuration& cell_cfg, const dl_sched_result& result)
{
  cell_slot_resource_grid grid(cell_cfg.dl_cfg_common.freq_info_dl.scs_carrier_list);

  // Register SSB.
  for (const ssb_information& ssb : result.bc.ssb_info) {
    crb_interval crbs = ssb.crbs;
    grant_info   grant{grant_info::channel::ssb, cell_cfg.ssb_cfg.scs, ssb.symbols, crbs};
    TESTASSERT(not grid.collides(grant));
    grid.fill(grant);
  }

  // Fill DL PDCCHs.
  for (const pdcch_dl_information& pdcch : result.dl_pdcchs) {
    const bwp_configuration&     bwp_cfg = *pdcch.ctx.bwp_cfg;
    const coreset_configuration& cs_cfg  = *pdcch.ctx.coreset_cfg;
    prb_index_list               pdcch_prbs =
        cce_to_prb_mapping(bwp_cfg, cs_cfg, cell_cfg.pci, pdcch.ctx.cces.aggr_lvl, pdcch.ctx.cces.ncce);
    for (unsigned prb : pdcch_prbs) {
      unsigned   crb = prb_to_crb(bwp_cfg, prb);
      grant_info grant{
          grant_info::channel::cch, bwp_cfg.scs, ofdm_symbol_range{0U, (uint8_t)cs_cfg.duration}, {crb, crb + 1}};
      TESTASSERT(not grid.collides(grant));
      grid.fill(grant);
    }
  }

  // Fill UL PDCCHs.
  for (const pdcch_ul_information& pdcch : result.ul_pdcchs) {
    const bwp_configuration&     bwp_cfg = *pdcch.ctx.bwp_cfg;
    const coreset_configuration& cs_cfg  = *pdcch.ctx.coreset_cfg;
    prb_index_list               pdcch_prbs =
        cce_to_prb_mapping(bwp_cfg, cs_cfg, cell_cfg.pci, pdcch.ctx.cces.aggr_lvl, pdcch.ctx.cces.ncce);
    for (unsigned prb : pdcch_prbs) {
      unsigned   crb = prb_to_crb(bwp_cfg, prb);
      grant_info grant{
          grant_info::channel::cch, bwp_cfg.scs, ofdm_symbol_range{0U, (uint8_t)cs_cfg.duration}, {crb, crb + 1}};
      TESTASSERT(not grid.collides(grant));
      grid.fill(grant);
    }
  }

  // Register SIB1 PDSCH.
  for (const sib_information& sib : result.bc.sibs) {
    bwp_configuration effective_init_bwp_cfg = *sib.pdsch_cfg.bwp_cfg;
    effective_init_bwp_cfg.crbs              = get_coreset0_crbs(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common);

    prb_interval prbs = sib.pdsch_cfg.prbs.prbs();
    crb_interval crbs = prb_to_crb(effective_init_bwp_cfg, prbs);
    grant_info   grant{grant_info::channel::sch, sib.pdsch_cfg.bwp_cfg->scs, sib.pdsch_cfg.symbols, crbs};
    TESTASSERT(not grid.collides(grant));
    grid.fill(grant);
  }

  // Register RAR PDSCHs.
  for (const rar_information& rar : result.rar_grants) {
    crb_interval crbs = prb_to_crb(*rar.pdsch_cfg.bwp_cfg, rar.pdsch_cfg.prbs.prbs());
    grant_info   grant{grant_info::channel::sch, rar.pdsch_cfg.bwp_cfg->scs, rar.pdsch_cfg.symbols, crbs};
    TESTASSERT(not grid.collides(grant));
    grid.fill(grant);
  }

  // Register UE PDSCHs.
  for (const dl_msg_alloc& ue_pdsch : result.ue_grants) {
    crb_interval crbs = prb_to_crb(*ue_pdsch.pdsch_cfg.bwp_cfg, ue_pdsch.pdsch_cfg.prbs.prbs());
    grant_info   grant{grant_info::channel::sch, ue_pdsch.pdsch_cfg.bwp_cfg->scs, ue_pdsch.pdsch_cfg.symbols, crbs};
    TESTASSERT(not grid.collides(grant));
    grid.fill(grant);
  }
}

void srsgnb::test_ul_resource_grid_collisions(const cell_configuration& cell_cfg, const ul_sched_result& result)
{
  cell_slot_resource_grid grid(cell_cfg.ul_cfg_common.freq_info_ul.scs_carrier_list);

  // Fill PRACHs.
  if (not result.prachs.empty()) {
    prach_configuration prach_cfg = prach_configuration_get(
        frequency_range::FR1,
        cell_cfg.paired_spectrum ? duplex_mode::FDD : duplex_mode::TDD,
        cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index);

    for (const prach_occasion_info& prach : result.prachs) {
      ofdm_symbol_range symbols{prach.start_symbol, (uint8_t)(prach.start_symbol + prach_cfg.duration)};
      unsigned prb_start = cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_frequency_start;
      prb_interval prbs{prb_start, prb_start + 6}; // TODO: Derive nof RBs.
      crb_interval crbs = prb_to_crb(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params, prbs);
      grant_info grant{grant_info::channel::sch, cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs, symbols, crbs};
      TESTASSERT(not grid.collides(grant));
      grid.fill(grant);
    }
  }

  // Fill PUSCHs.
  for (const ul_sched_info& pusch : result.puschs) {
    const bwp_configuration& bwp_cfg = *pusch.pusch_cfg.bwp_cfg;
    crb_interval             crbs    = prb_to_crb(bwp_cfg, pusch.pusch_cfg.prbs.prbs());
    grant_info grant{grant_info::channel::sch, pusch.pusch_cfg.bwp_cfg->scs, pusch.pusch_cfg.symbols, crbs};
    TESTASSERT(not grid.collides(grant));
    grid.fill(grant);
  }
}

void srsgnb::test_scheduler_result_consistency(const cell_configuration& cell_cfg, const sched_result& result)
{
  test_pdsch_sib_consistency(cell_cfg, result.dl.bc.sibs);
  test_dl_resource_grid_collisions(cell_cfg, result.dl);
  test_ul_resource_grid_collisions(cell_cfg, result.ul);
}
