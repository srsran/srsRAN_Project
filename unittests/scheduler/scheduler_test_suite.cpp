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
#include "scheduler_output_test_helpers.h"
#include "srsgnb/ran/prach/prach_configuration.h"
#include "srsgnb/support/error_handling.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

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
      srsgnb_terminate("not supported");
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

  std::vector<test_grant_info> dl_grants = get_dl_grants(cell_cfg, result);
  for (const test_grant_info& test_grant : dl_grants) {
    TESTASSERT(not grid.collides(test_grant.grant), "Resource collision for grant with rnti={:#x}", test_grant.rnti);
    grid.fill(test_grant.grant);
  }
}

void srsgnb::test_prach_opportunity_validity(const cell_configuration& cell_cfg, span<const prach_occasion_info> prachs)
{
  if (prachs.empty()) {
    return;
  }
  const rach_config_common& rach_cfg_common = *cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common;
  const prach_configuration prach_cfg =
      prach_configuration_get(frequency_range::FR1,
                              cell_cfg.paired_spectrum ? duplex_mode::FDD : duplex_mode::TDD,
                              rach_cfg_common.rach_cfg_generic.prach_config_index);

  for (const prach_occasion_info& prach : prachs) {
    // Check if the PRACH matches cell configuration.
    ASSERT_EQ(prach_cfg.format, prach.format);
    if (prach.start_preamble_index != 255) {
      ASSERT_EQ(prach.start_preamble_index, rach_cfg_common.prach_root_seq_index);
    }
    ASSERT_EQ(rach_cfg_common.total_nof_ra_preambles, prach.nof_preamble_indexes);
    ASSERT_EQ(prach_cfg.nof_occasions_within_slot, prach.nof_prach_occasions);
    ASSERT_EQ(prach_cfg.starting_symbol, prach.start_symbol);
  }
}

void srsgnb::test_ul_resource_grid_collisions(const cell_configuration& cell_cfg, const ul_sched_result& result)
{
  cell_slot_resource_grid grid(cell_cfg.ul_cfg_common.freq_info_ul.scs_carrier_list);

  std::vector<test_grant_info> ul_grants = get_ul_grants(cell_cfg, result);
  for (const test_grant_info& test_grant : ul_grants) {
    TESTASSERT(not grid.collides(test_grant.grant), "Resource collision for grant with rnti={:#x}", test_grant.rnti);
    grid.fill(test_grant.grant);
  }
}

void srsgnb::test_scheduler_result_consistency(const cell_configuration& cell_cfg, const sched_result& result)
{
  test_pdsch_sib_consistency(cell_cfg, result.dl.bc.sibs);
  test_prach_opportunity_validity(cell_cfg, result.ul.prachs);
  test_dl_resource_grid_collisions(cell_cfg, result.dl);
  test_ul_resource_grid_collisions(cell_cfg, result.ul);
}
