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
#include "lib/scheduler/common_scheduling/ra_scheduler.h"
#include "lib/scheduler/support/config_helpers.h"
#include "scheduler_output_test_helpers.h"
#include "srsgnb/ran/prach/prach_configuration.h"
#include "srsgnb/ran/resource_allocation/resource_allocation_frequency.h"
#include "srsgnb/support/error_handling.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;

void srsgnb::assert_pdcch_pdsch_common_consistency(const cell_configuration&   cell_cfg,
                                                   const pdcch_dl_information& pdcch,
                                                   const pdsch_information&    pdsch)
{
  TESTASSERT_EQ(pdcch.ctx.rnti, pdsch.rnti);
  TESTASSERT(*pdcch.ctx.bwp_cfg == *pdsch.bwp_cfg);
  crb_interval coreset0_rb_lims = get_coreset0_crbs(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common);

  uint8_t  time_assignment = 0;
  uint8_t  freq_assignment = 0;
  unsigned N_rb_dl_bwp     = 0;
  switch (pdcch.dci.type) {
    case dci_dl_rnti_config_type::si_f1_0: {
      TESTASSERT_EQ(pdcch.ctx.rnti, SI_RNTI);
      time_assignment = pdcch.dci.si_f1_0.time_resource;
      freq_assignment = pdcch.dci.si_f1_0.frequency_resource;
      N_rb_dl_bwp     = pdcch.dci.si_f1_0.N_rb_dl_bwp;
      TESTASSERT_EQ(N_rb_dl_bwp, coreset0_rb_lims.length());
      break;
    }
    case dci_dl_rnti_config_type::ra_f1_0: {
      time_assignment = pdcch.dci.ra_f1_0.time_resource;
      TESTASSERT(time_assignment < cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.size());
      freq_assignment = pdcch.dci.ra_f1_0.frequency_resource;
      N_rb_dl_bwp     = pdcch.dci.ra_f1_0.N_rb_dl_bwp;
      TESTASSERT_EQ(N_rb_dl_bwp, coreset0_rb_lims.length());
    } break;
    case dci_dl_rnti_config_type::tc_rnti_f1_0: {
      time_assignment = pdcch.dci.tc_rnti_f1_0.time_resource;
      freq_assignment = pdcch.dci.tc_rnti_f1_0.frequency_resource;
      N_rb_dl_bwp     = pdcch.dci.tc_rnti_f1_0.N_rb_dl_bwp;
      TESTASSERT_EQ(N_rb_dl_bwp, coreset0_rb_lims.length());
    } break;
    default:
      srsgnb_terminate("DCI type not supported");
  }
  ofdm_symbol_range symbols =
      cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[time_assignment].symbols;
  TESTASSERT(symbols == pdsch.symbols, "Mismatch of time-domain resource assignment and PDSCH symbols");

  uint8_t pdsch_freq_resource = ra_frequency_type1_get_riv(
      ra_frequency_type1_configuration{N_rb_dl_bwp, pdsch.prbs.prbs().start(), pdsch.prbs.prbs().length()});
  TESTASSERT_EQ(pdsch_freq_resource, freq_assignment, "DCI frequency resource does not match PDSCH PRBs");
}

void srsgnb::assert_pdcch_pdsch_common_consistency(const cell_configuration&      cell_cfg,
                                                   const cell_resource_allocator& cell_res_grid)
{
  span<const pdcch_dl_information> pdcchs = cell_res_grid[0].result.dl.dl_pdcchs;
  for (const pdcch_dl_information& pdcch : pdcchs) {
    switch (pdcch.dci.type) {
      case dci_dl_rnti_config_type::si_f1_0: {
        const auto&     sibs = cell_res_grid[0].result.dl.bc.sibs;
        sib_information sib;
        auto            it = std::find_if(
            sibs.begin(), sibs.end(), [&pdcch](const auto& sib_) { return sib_.pdsch_cfg.rnti == pdcch.ctx.rnti; });
        TESTASSERT(it != sibs.end());
        assert_pdcch_pdsch_common_consistency(cell_cfg, pdcch, it->pdsch_cfg);
      } break;
      case dci_dl_rnti_config_type::ra_f1_0: {
        uint8_t k0 =
            cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[pdcch.dci.ra_f1_0.time_resource].k0;
        const auto& rars = cell_res_grid[k0].result.dl.rar_grants;
        auto        it   = std::find_if(
            rars.begin(), rars.end(), [&pdcch](const auto& rar) { return rar.pdsch_cfg.rnti == pdcch.ctx.rnti; });
        TESTASSERT(it != rars.end());
        assert_pdcch_pdsch_common_consistency(cell_cfg, pdcch, it->pdsch_cfg);
      } break;
      case dci_dl_rnti_config_type::tc_rnti_f1_0: {
        uint8_t k0 =
            cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[pdcch.dci.tc_rnti_f1_0.time_resource]
                .k0;
        const auto& ue_grants = cell_res_grid[k0].result.dl.ue_grants;
        auto        it        = std::find_if(ue_grants.begin(), ue_grants.end(), [&pdcch](const auto& grant) {
          return grant.pdsch_cfg.rnti == pdcch.ctx.rnti;
        });
        TESTASSERT(it != ue_grants.end());
        assert_pdcch_pdsch_common_consistency(cell_cfg, pdcch, it->pdsch_cfg);
      } break;
      default:
        srsgnb_terminate("DCI type not supported");
    }
  }
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
  }
}

void srsgnb::test_pdsch_rar_consistency(const cell_configuration& cell_cfg, span<const rar_information> rars)
{
  std::set<rnti_t>  ra_rntis;
  crb_interval      coreset0_lims          = get_coreset0_crbs(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common);
  bwp_configuration effective_init_bwp_cfg = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params;
  effective_init_bwp_cfg.crbs              = coreset0_lims;

  for (const rar_information& rar : rars) {
    rnti_t ra_rnti = rar.pdsch_cfg.rnti;
    TESTASSERT(not rar.grants.empty(), "RAR with RA-RNTI={:#x} has no corresponding MSG3 grants", ra_rnti);
    TESTASSERT(rar.pdsch_cfg.prbs.is_alloc_type1(), "Invalid allocation type for RAR");

    crb_interval rar_crbs = prb_to_crb(effective_init_bwp_cfg, rar.pdsch_cfg.prbs.prbs());
    TESTASSERT(coreset0_lims.contains(rar_crbs), "RAR outside of initial active DL BWP RB limits");

    TESTASSERT(not ra_rntis.count(ra_rnti), "Repeated RA-RNTI={:#x} detected", ra_rnti);
    ra_rntis.emplace(ra_rnti);
  }
}

void assert_rar_grant_msg3_pusch_consistency(const cell_configuration& cell_cfg,
                                             const rar_ul_grant&       rar_grant,
                                             const pusch_information&  msg3_pusch)
{
  TESTASSERT_EQ(rar_grant.temp_crnti, msg3_pusch.rnti);
  TESTASSERT(msg3_pusch.prbs.is_alloc_type1());
  TESTASSERT(not msg3_pusch.prbs.prbs().empty(), "Msg3 with temp-CRNTI={:#x} has no RBs", msg3_pusch.rnti);

  unsigned     N_rb_ul_bwp = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();
  prb_interval prbs        = msg3_pusch.prbs.prbs();
  uint8_t      pusch_freq_resource =
      ra_frequency_type1_get_riv(ra_frequency_type1_configuration{N_rb_ul_bwp, prbs.start(), prbs.length()});
  TESTASSERT_EQ(rar_grant.freq_resource_assignment,
                pusch_freq_resource,
                "Mismatch between RAR grant frequency assignment and corresponding Msg3 PUSCH PRBs");
}

void assert_rar_grant_msg3_pusch_consistency(const cell_configuration&      cell_cfg,
                                             const cell_resource_allocator& res_grid)
{
  std::set<rnti_t> tc_rntis;
  const auto&      pusch_td_list = cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;

  span<const pdcch_dl_information> pdcchs = res_grid[0].result.dl.dl_pdcchs;
  for (const pdcch_dl_information& pdcch : pdcchs) {
    if (pdcch.dci.type != dci_dl_rnti_config_type::ra_f1_0) {
      continue;
    }

    // For a given PDCCH for a RAR, search for the respective RAR PDSCH.
    uint8_t k0 =
        cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[pdcch.dci.ra_f1_0.time_resource].k0;
    span<const rar_information> rars   = res_grid[k0].result.dl.rar_grants;
    auto                        rar_it = std::find_if(
        rars.begin(), rars.end(), [&pdcch](const auto& rar) { return rar.pdsch_cfg.rnti == pdcch.ctx.rnti; });
    TESTASSERT(rar_it != rars.end());
    const rar_information& rar = *rar_it;

    // For all RAR grants within the same RAR, check that they are consistent with the respective Msg3 PUSCHs.
    for (const rar_ul_grant& rar_grant : rar.grants) {
      TESTASSERT(rar_grant.time_resource_assignment < pusch_td_list.size());
      uint8_t k2 = get_msg3_delay(pusch_td_list[rar_grant.time_resource_assignment],
                                  cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs);

      span<const ul_sched_info> ul_grants = res_grid[k2].result.ul.puschs;
      auto it = std::find_if(ul_grants.begin(), ul_grants.end(), [&rar_grant](const auto& ulgrant) {
        return ulgrant.pusch_cfg.rnti == rar_grant.temp_crnti;
      });
      TESTASSERT(it != ul_grants.end(),
                 "Msg3 was not found for the scheduled RAR grant with TC-RNTI={:#x}",
                 rar_grant.temp_crnti);
      assert_rar_grant_msg3_pusch_consistency(cell_cfg, rar_grant, it->pusch_cfg);

      TESTASSERT(tc_rntis.count(rar_grant.temp_crnti) == 0, "Repeated TC-RNTI detected");
      tc_rntis.emplace(rar_grant.temp_crnti);
    }
  }
}

void srsgnb::test_dl_resource_grid_collisions(const cell_configuration& cell_cfg, const dl_sched_result& result)
{
  cell_slot_resource_grid grid(cell_cfg.dl_cfg_common.freq_info_dl.scs_carrier_list);

  std::vector<test_grant_info> dl_grants = get_dl_grants(cell_cfg, result);
  for (const test_grant_info& test_grant : dl_grants) {
    TESTASSERT(not test_grant.grant.crbs.empty(), "Resource is empty");
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
      ASSERT_EQ(0, prach.start_preamble_index);
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
  test_pdsch_rar_consistency(cell_cfg, result.dl.rar_grants);
  test_dl_resource_grid_collisions(cell_cfg, result.dl);
  test_ul_resource_grid_collisions(cell_cfg, result.ul);
}

/// \brief Verifies that the cell resource grid PRBs and symbols was filled with the allocated PDSCHs.
void assert_dl_resource_grid_filled(const cell_configuration& cell_cfg, const cell_resource_allocator& cell_res_grid)
{
  std::vector<test_grant_info> dl_grants = get_dl_grants(cell_cfg, cell_res_grid[0].result.dl);
  for (const test_grant_info& test_grant : dl_grants) {
    if (test_grant.type != srsgnb::test_grant_info::DL_PDCCH) {
      TESTASSERT(cell_res_grid[0].dl_res_grid.all_set(test_grant.grant),
                 "The allocation with rnti={:#x} was not registered in the cell resource grid",
                 test_grant.rnti);
    }
  }
}

/// \brief Verifies that the cell resource grid PRBs and symbols was filled with the allocated PUCCHs.
bool srsgnb::assert_ul_resource_grid_filled(const cell_configuration&      cell_cfg,
                                            const cell_resource_allocator& cell_res_grid,
                                            unsigned                       tx_delay,
                                            bool                           expect_grants)
{
  // The function get_ul_grants() returns 2 test_grant_info per pucch_info if intra_slot_freq_hopping is enabled.
  std::vector<test_grant_info> ul_grants = get_ul_grants(cell_cfg, cell_res_grid[tx_delay].result.ul);
  if (expect_grants and ul_grants.empty()) {
    return false;
  }
  for (const test_grant_info& test_grant : ul_grants) {
    if (test_grant.type == srsgnb::test_grant_info::UE_UL || test_grant.type == srsgnb::test_grant_info::PUCCH) {
      if (not cell_res_grid[tx_delay].ul_res_grid.all_set(test_grant.grant)) {
        return false;
      }
    }
  }
  return true;
}

void srsgnb::test_scheduler_result_consistency(const cell_configuration&      cell_cfg,
                                               const cell_resource_allocator& cell_res_grid)
{
  test_scheduler_result_consistency(cell_cfg, cell_res_grid[0].result);
  assert_pdcch_pdsch_common_consistency(cell_cfg, cell_res_grid);
  assert_dl_resource_grid_filled(cell_cfg, cell_res_grid);
  assert_rar_grant_msg3_pusch_consistency(cell_cfg, cell_res_grid);
}
