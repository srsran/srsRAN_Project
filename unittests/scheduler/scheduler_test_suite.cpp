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
#include "lib/scheduler/support/config_helpers.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

void srsgnb::test_pdcch_pdsch_common_consistency(const cell_configuration&   cell_cfg,
                                                 const pdcch_dl_information& pdcch,
                                                 const pdsch_information&  pdsch)
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

void srsgnb::test_pdsch_sib_consistency(const cell_configuration&   cell_cfg,
                                        span<const sib_information> sibs,
                                        prb_bitmap*                 used_rbs)
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
    if (used_rbs != nullptr) {
      crb_interval crbs = prb_to_crb(effective_init_bwp_cfg, prbs);
      TESTASSERT(not used_rbs->any(crbs.start(), crbs.stop()), "RB Collision detected between SIB1 and another alloc");
      used_rbs->fill(crbs.start(), crbs.stop());
    }
    if (sib.pdcch_cfg != nullptr) {
      test_pdcch_pdsch_common_consistency(cell_cfg, *sib.pdcch_cfg, sib.pdsch_cfg);
    }
  }
}

void srsgnb::test_scheduler_result_consistency(const cell_configuration& cell_cfg, const sched_result& result)
{
  prb_bitmap used_crbs(cell_cfg.nof_dl_prbs);
  test_pdsch_sib_consistency(cell_cfg, result.dl.bc.sibs, &used_crbs);
}
