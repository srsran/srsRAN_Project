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

#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "lib/scheduler/cell/resource_grid.h"
#include "lib/scheduler/common_scheduling/ra_scheduler.h"
#include "lib/scheduler/support/config_helpers.h"
#include "lib/scheduler/support/pdsch/pdsch_default_time_allocation.h"
#include "scheduler_output_test_helpers.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/ran/resource_allocation/resource_allocation_frequency.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

void srsran::assert_tdd_pattern_consistency(const cell_configuration& cell_cfg,
                                            slot_point                sl_tx,
                                            const sched_result&       result)
{
  if (not cell_cfg.tdd_cfg_common.has_value()) {
    return;
  }
  ofdm_symbol_range dl_symbols = get_active_tdd_dl_symbols(
      *cell_cfg.tdd_cfg_common, sl_tx.to_uint(), cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.cp);
  if (dl_symbols.empty()) {
    ASSERT_TRUE(result.dl.dl_pdcchs.empty());
    ASSERT_TRUE(result.dl.ul_pdcchs.empty());
    ASSERT_TRUE(result.dl.bc.ssb_info.empty());
    ASSERT_TRUE(result.dl.bc.sibs.empty());
    ASSERT_TRUE(result.dl.rar_grants.empty());
    ASSERT_TRUE(result.dl.ue_grants.empty());
    ASSERT_TRUE(result.dl.paging_grants.empty());
    ASSERT_TRUE(result.dl.csi_rs.empty());
  } else if (dl_symbols.length() != get_nsymb_per_slot(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.cp)) {
    // Partial slot case.
    for (const auto& sib : result.dl.bc.sibs) {
      ASSERT_TRUE(dl_symbols.contains(sib.pdsch_cfg.symbols));
    }
    for (const auto& rar : result.dl.rar_grants) {
      ASSERT_TRUE(dl_symbols.contains(rar.pdsch_cfg.symbols));
    }
    for (const auto& ue_grant : result.dl.ue_grants) {
      ASSERT_TRUE(dl_symbols.contains(ue_grant.pdsch_cfg.symbols));
    }
    for (const auto& paging_grant : result.dl.paging_grants) {
      ASSERT_TRUE(dl_symbols.contains(paging_grant.pdsch_cfg.symbols));
    }
    for (const auto& csi_rs : result.dl.csi_rs) {
      ASSERT_TRUE(dl_symbols.contains(csi_rs.symbol0));
    }
  }

  ofdm_symbol_range ul_symbols = get_active_tdd_ul_symbols(
      *cell_cfg.tdd_cfg_common, sl_tx.to_uint(), cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.cp);
  if (ul_symbols.empty()) {
    ASSERT_TRUE(result.ul.pucchs.empty());
    ASSERT_TRUE(result.ul.puschs.empty());
    ASSERT_TRUE(result.ul.prachs.empty());
  } else if (dl_symbols.length() != get_nsymb_per_slot(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.cp)) {
    for (const auto& ue_grant : result.ul.puschs) {
      ASSERT_TRUE(ul_symbols.contains(ue_grant.pusch_cfg.symbols));
    }
    for (const auto& pucch : result.ul.pucchs) {
      ASSERT_TRUE(ul_symbols.contains(pucch.resources.symbols));
    }
  }
}

void srsran::assert_pdcch_pdsch_common_consistency(const cell_configuration&   cell_cfg,
                                                   const pdcch_dl_information& pdcch,
                                                   const pdsch_information&    pdsch)
{
  TESTASSERT_EQ(pdcch.ctx.rnti, pdsch.rnti);
  TESTASSERT(*pdcch.ctx.bwp_cfg == *pdsch.bwp_cfg);
  TESTASSERT(*pdcch.ctx.coreset_cfg == *pdsch.coreset_cfg);
  bwp_configuration bwp_cfg = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params;
  // See TS 38.214, 5.1.2.2.2, Downlink resource allocation type 1.
  if (cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.has_value()) {
    bwp_cfg.crbs = get_coreset0_crbs(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common);
  }
  const crb_interval cs_zero_crbs = get_coreset0_crbs(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common);

  uint8_t  time_assignment = 0;
  uint8_t  freq_assignment = 0;
  unsigned N_rb_dl_bwp     = 0;
  switch (pdcch.dci.type) {
    case dci_dl_rnti_config_type::si_f1_0: {
      TESTASSERT_EQ(pdcch.ctx.rnti, rnti_t::SI_RNTI);
      time_assignment = pdcch.dci.si_f1_0.time_resource;
      freq_assignment = pdcch.dci.si_f1_0.frequency_resource;
      N_rb_dl_bwp     = pdcch.dci.si_f1_0.N_rb_dl_bwp;
      TESTASSERT_EQ(N_rb_dl_bwp, cs_zero_crbs.length());
      break;
    }
    case dci_dl_rnti_config_type::ra_f1_0: {
      time_assignment = pdcch.dci.ra_f1_0.time_resource;
      TESTASSERT(time_assignment < cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.size());
      freq_assignment = pdcch.dci.ra_f1_0.frequency_resource;
      N_rb_dl_bwp     = pdcch.dci.ra_f1_0.N_rb_dl_bwp;
      TESTASSERT_EQ(N_rb_dl_bwp, bwp_cfg.crbs.length());
    } break;
    case dci_dl_rnti_config_type::tc_rnti_f1_0: {
      time_assignment = pdcch.dci.tc_rnti_f1_0.time_resource;
      freq_assignment = pdcch.dci.tc_rnti_f1_0.frequency_resource;
      N_rb_dl_bwp     = pdcch.dci.tc_rnti_f1_0.N_rb_dl_bwp;
      TESTASSERT_EQ(N_rb_dl_bwp, cs_zero_crbs.length());
    } break;
    case dci_dl_rnti_config_type::p_rnti_f1_0: {
      time_assignment = pdcch.dci.p_rnti_f1_0.time_resource;
      freq_assignment = pdcch.dci.p_rnti_f1_0.frequency_resource;
      N_rb_dl_bwp     = pdcch.dci.p_rnti_f1_0.N_rb_dl_bwp;
      TESTASSERT_EQ(N_rb_dl_bwp, cs_zero_crbs.length());
    } break;
    default:
      srsran_terminate("DCI type not supported");
  }
  ofdm_symbol_range symbols =
      cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[time_assignment].symbols;
  TESTASSERT(symbols == pdsch.symbols, "Mismatch of time-domain resource assignment and PDSCH symbols");

  uint8_t pdsch_freq_resource = ra_frequency_type1_get_riv(
      ra_frequency_type1_configuration{N_rb_dl_bwp, pdsch.rbs.type1().start(), pdsch.rbs.type1().length()});
  TESTASSERT_EQ(pdsch_freq_resource, freq_assignment, "DCI frequency resource does not match PDSCH PRBs");
}

void srsran::assert_pdcch_pdsch_common_consistency(const cell_configuration&      cell_cfg,
                                                   const cell_resource_allocator& cell_res_grid)
{
  span<const pdcch_dl_information> pdcchs = cell_res_grid[0].result.dl.dl_pdcchs;
  for (const pdcch_dl_information& pdcch : pdcchs) {
    const pdsch_information* linked_pdsch = nullptr;
    switch (pdcch.dci.type) {
      case dci_dl_rnti_config_type::si_f1_0: {
        const auto&     sibs = cell_res_grid[0].result.dl.bc.sibs;
        sib_information sib;
        auto            it = std::find_if(
            sibs.begin(), sibs.end(), [&pdcch](const auto& sib_) { return sib_.pdsch_cfg.rnti == pdcch.ctx.rnti; });
        TESTASSERT(it != sibs.end());
        linked_pdsch = &it->pdsch_cfg;
      } break;
      case dci_dl_rnti_config_type::ra_f1_0: {
        uint8_t k0 =
            cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[pdcch.dci.ra_f1_0.time_resource].k0;
        const auto& rars = cell_res_grid[k0].result.dl.rar_grants;
        auto        it   = std::find_if(
            rars.begin(), rars.end(), [&pdcch](const auto& rar) { return rar.pdsch_cfg.rnti == pdcch.ctx.rnti; });
        TESTASSERT(it != rars.end());
        linked_pdsch = &it->pdsch_cfg;
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
        linked_pdsch = &it->pdsch_cfg;
      } break;
      case dci_dl_rnti_config_type::p_rnti_f1_0: {
        uint8_t k0 =
            cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[pdcch.dci.p_rnti_f1_0.time_resource].k0;
        const auto& pg_grants = cell_res_grid[k0].result.dl.paging_grants;
        auto        it        = std::find_if(pg_grants.begin(), pg_grants.end(), [&pdcch](const auto& grant) {
          return grant.pdsch_cfg.rnti == pdcch.ctx.rnti;
        });
        TESTASSERT(it != pg_grants.end());
        linked_pdsch = &it->pdsch_cfg;
      } break;
      default:
        srsran_terminate("DCI type not supported");
    }
    assert_pdcch_pdsch_common_consistency(cell_cfg, pdcch, *linked_pdsch);
  }
}

void srsran::test_pdsch_sib_consistency(const cell_configuration& cell_cfg, span<const sib_information> sibs)
{
  bool has_coreset0 = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.has_value();
  if (not has_coreset0) {
    ASSERT_TRUE(sibs.empty()) << fmt::format("SIB1 cannot be scheduled without CORESET#0");
    return;
  }

  bwp_configuration effective_init_bwp_cfg = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params;
  effective_init_bwp_cfg.crbs              = get_coreset0_crbs(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common);

  for (const sib_information& sib : sibs) {
    ASSERT_EQ(sib.pdsch_cfg.rnti, rnti_t::SI_RNTI);
    ASSERT_EQ(sib.pdsch_cfg.dci_fmt, dci_dl_format::f1_0);
    ASSERT_TRUE(sib.pdsch_cfg.rbs.is_type1());
    ASSERT_EQ(sib.pdsch_cfg.coreset_cfg->id, to_coreset_id(0));
    ASSERT_EQ(sib.pdsch_cfg.ss_set_type, search_space_set_type::type0);
    ASSERT_EQ(sib.pdsch_cfg.codewords.size(), 1);
    ASSERT_EQ(sib.pdsch_cfg.codewords[0].mcs_table, pdsch_mcs_table::qam64);
    vrb_interval vrbs = sib.pdsch_cfg.rbs.type1();
    ASSERT_LE(vrbs.stop(), effective_init_bwp_cfg.crbs.length())
        << fmt::format("PRB grant falls outside CORESET#0 RB boundaries");
  }
}

void srsran::test_pdsch_rar_consistency(const cell_configuration& cell_cfg, span<const rar_information> rars)
{
  std::set<rnti_t>                  ra_rntis;
  const search_space_configuration& ss_cfg =
      cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common
          .search_spaces[cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id];
  crb_interval      coreset0_lims = get_coreset0_crbs(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common);
  bwp_configuration init_bwp_cfg  = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params;

  for (const rar_information& rar : rars) {
    rnti_t ra_rnti = rar.pdsch_cfg.rnti;
    ASSERT_FALSE(rar.grants.empty()) << fmt::format("RAR with RA-rnti={} has no corresponding MSG3 grants", ra_rnti);
    ASSERT_EQ(rar.pdsch_cfg.dci_fmt, dci_dl_format::f1_0);
    ASSERT_TRUE(rar.pdsch_cfg.rbs.is_type1()) << "Invalid allocation type for RAR";
    ASSERT_EQ(rar.pdsch_cfg.coreset_cfg->id, ss_cfg.get_coreset_id());
    ASSERT_EQ(rar.pdsch_cfg.ss_set_type, search_space_set_type::type1);
    ASSERT_EQ(rar.pdsch_cfg.codewords.size(), 1);
    ASSERT_EQ(rar.pdsch_cfg.codewords[0].mcs_table, pdsch_mcs_table::qam64);

    const prb_interval rar_vrbs = {
        rar.pdsch_cfg.rbs.type1().start() + rar.pdsch_cfg.coreset_cfg->get_coreset_start_crb(),
        rar.pdsch_cfg.rbs.type1().stop() + rar.pdsch_cfg.coreset_cfg->get_coreset_start_crb()};
    crb_interval rar_crbs = prb_to_crb(init_bwp_cfg, rar_vrbs);
    TESTASSERT(coreset0_lims.contains(rar_crbs), "RAR outside of initial active DL BWP RB limits");

    TESTASSERT(not ra_rntis.count(ra_rnti), "Repeated RA-rnti={} detected", ra_rnti);
    ra_rntis.emplace(ra_rnti);
  }
}

/// \brief Tests the validity of the parameters chosen for the PDCCHs using common search spaces. Checks include:
/// - PDSCH time resource chosen (k0 and symbols) fall in DL symbols
/// - UCI delay chosen falls in an UL slot.
static void test_pdcch_common_consistency(const cell_configuration&        cell_cfg,
                                          slot_point                       pdcch_slot,
                                          span<const pdcch_dl_information> dl_pdcchs)
{
  if (not cell_cfg.tdd_cfg_common.has_value()) {
    return;
  }
  const auto& init_dl_bwp = cell_cfg.dl_cfg_common.init_dl_bwp;
  for (const pdcch_dl_information& pdcch : dl_pdcchs) {
    span<const pdsch_time_domain_resource_allocation> pdsch_td_list;
    optional<unsigned>                                time_res;
    optional<unsigned>                                k1;
    switch (pdcch.dci.type) {
      case dci_dl_rnti_config_type::si_f1_0:
        pdsch_td_list = get_si_rnti_pdsch_time_domain_list(init_dl_bwp.generic_params.cp, cell_cfg.dmrs_typeA_pos);
        time_res      = pdcch.dci.si_f1_0.time_resource;
        break;
      case dci_dl_rnti_config_type::ra_f1_0:
        pdsch_td_list = get_ra_rnti_pdsch_time_domain_list(
            init_dl_bwp.pdsch_common, init_dl_bwp.generic_params.cp, cell_cfg.dmrs_typeA_pos);
        time_res = pdcch.dci.ra_f1_0.time_resource;
        break;
      case dci_dl_rnti_config_type::tc_rnti_f1_0:
        pdsch_td_list = init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
        time_res      = pdcch.dci.tc_rnti_f1_0.time_resource;
        k1            = pdcch.dci.tc_rnti_f1_0.pdsch_harq_fb_timing_indicator + 1;
        break;
      default:
        break;
    }
    if (not time_res.has_value()) {
      // DCI likely using dedicated config.
      continue;
    }

    // Test PDSCH time resource chosen.
    ASSERT_LT(*time_res, pdsch_td_list.size());
    const pdsch_time_domain_resource_allocation& res        = pdsch_td_list[*time_res];
    const slot_point                             pdsch_slot = pdcch_slot + res.k0;
    const ofdm_symbol_range                      active_dl_symbols =
        get_active_tdd_dl_symbols(*cell_cfg.tdd_cfg_common, pdsch_slot.slot_index(), init_dl_bwp.generic_params.cp);
    ASSERT_TRUE(active_dl_symbols.contains(res.symbols)) << "PDSCH must fall in DL symbols";

    // Test HARQ delay chosen.
    if (k1.has_value()) {
      const slot_point pucch_slot = pdsch_slot + *k1;
      ASSERT_TRUE(has_active_tdd_ul_symbols(*cell_cfg.tdd_cfg_common, pucch_slot.slot_index()))
          << "PUCCH must fall in an UL slot";
    }
  }
}

void assert_rar_grant_msg3_pusch_consistency(const cell_configuration& cell_cfg,
                                             const rar_ul_grant&       rar_grant,
                                             const pusch_information&  msg3_pusch)
{
  TESTASSERT_EQ(rar_grant.temp_crnti, msg3_pusch.rnti);
  TESTASSERT(msg3_pusch.rbs.is_type1());
  TESTASSERT(not msg3_pusch.rbs.any(), "Msg3 with temp-c-rnti={} has no RBs", msg3_pusch.rnti);

  unsigned     N_rb_ul_bwp = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();
  vrb_interval vrbs        = msg3_pusch.rbs.type1();
  uint8_t      pusch_freq_resource =
      ra_frequency_type1_get_riv(ra_frequency_type1_configuration{N_rb_ul_bwp, vrbs.start(), vrbs.length()});
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

    ASSERT_EQ(rar.pdsch_cfg.codewords.size(), 1);
    const units::bytes rar_pdu_size{8}; // MAC RAR PDU subheader + length (See TS38.321, 6.1.5 and 6.2.3).
    ASSERT_GE(rar.pdsch_cfg.codewords[0].tb_size_bytes, rar_pdu_size.value() * rar.grants.size());

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
                 "Msg3 was not found for the scheduled RAR grant with tc-rnti={}",
                 rar_grant.temp_crnti);
      assert_rar_grant_msg3_pusch_consistency(cell_cfg, rar_grant, it->pusch_cfg);

      TESTASSERT(tc_rntis.count(rar_grant.temp_crnti) == 0, "Repeated TC-RNTI detected");
      tc_rntis.emplace(rar_grant.temp_crnti);
    }
  }
}

void srsran::test_dl_resource_grid_collisions(const cell_configuration& cell_cfg, const dl_sched_result& result)
{
  cell_slot_resource_grid grid(cell_cfg.dl_cfg_common.freq_info_dl.scs_carrier_list);

  std::vector<test_grant_info> dl_grants = get_dl_grants(cell_cfg, result);
  for (const test_grant_info& test_grant : dl_grants) {
    TESTASSERT(not test_grant.grant.crbs.empty(), "Resource is empty");
    TESTASSERT(not grid.collides(test_grant.grant), "Resource collision for grant with rnti={}", test_grant.rnti);
    grid.fill(test_grant.grant);
  }
}

void srsran::test_prach_opportunity_validity(const cell_configuration& cell_cfg, span<const prach_occasion_info> prachs)
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
    if (rach_cfg_common.total_nof_ra_preambles.has_value()) {
      ASSERT_EQ(rach_cfg_common.total_nof_ra_preambles.value(), prach.nof_preamble_indexes);
    }
    ASSERT_EQ(prach_cfg.nof_occasions_within_slot, prach.nof_prach_occasions);
    ASSERT_EQ(prach_cfg.starting_symbol, prach.start_symbol);
  }
}

void srsran::test_ul_resource_grid_collisions(const cell_configuration& cell_cfg, const ul_sched_result& result)
{
  cell_slot_resource_grid grid(cell_cfg.ul_cfg_common.freq_info_ul.scs_carrier_list);

  std::vector<test_grant_info> ul_grants = get_ul_grants(cell_cfg, result);
  for (const test_grant_info& test_grant : ul_grants) {
    if (test_grant.type != test_grant_info::PUCCH) {
      ASSERT_FALSE(grid.collides(test_grant.grant))
          << fmt::format("Resource collision for grant with rnti={}", test_grant.rnti);
    }
    grid.fill(test_grant.grant);
  }
}

void srsran::test_scheduler_result_consistency(const cell_configuration& cell_cfg,
                                               slot_point                sl_tx,
                                               const sched_result&       result)
{
  ASSERT_TRUE(result.success);
  ASSERT_NO_FATAL_FAILURE(assert_tdd_pattern_consistency(cell_cfg, sl_tx, result));
  ASSERT_NO_FATAL_FAILURE(test_pdsch_sib_consistency(cell_cfg, result.dl.bc.sibs));
  ASSERT_NO_FATAL_FAILURE(test_prach_opportunity_validity(cell_cfg, result.ul.prachs));
  ASSERT_NO_FATAL_FAILURE(test_pdsch_rar_consistency(cell_cfg, result.dl.rar_grants));
  ASSERT_NO_FATAL_FAILURE(test_pdcch_common_consistency(cell_cfg, sl_tx, result.dl.dl_pdcchs));
  ASSERT_NO_FATAL_FAILURE(test_dl_resource_grid_collisions(cell_cfg, result.dl));
  ASSERT_NO_FATAL_FAILURE(test_ul_resource_grid_collisions(cell_cfg, result.ul));
}

/// \brief Verifies that the cell resource grid PRBs and symbols was filled with the allocated PDSCHs.
void assert_dl_resource_grid_filled(const cell_configuration& cell_cfg, const cell_resource_allocator& cell_res_grid)
{
  std::vector<test_grant_info> dl_grants = get_dl_grants(cell_cfg, cell_res_grid[0].result.dl);
  for (const test_grant_info& test_grant : dl_grants) {
    if (test_grant.type != srsran::test_grant_info::DL_PDCCH) {
      TESTASSERT(cell_res_grid[0].dl_res_grid.all_set(test_grant.grant),
                 "The allocation with rnti={}, type={}, crbs={} was not registered in the cell resource grid",
                 test_grant.rnti,
                 test_grant.type,
                 test_grant.grant.crbs);
    }
  }
}

/// \brief Verifies that the cell resource grid PRBs and symbols was filled with the allocated PUCCHs.
bool srsran::assert_ul_resource_grid_filled(const cell_configuration&      cell_cfg,
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
    if (test_grant.type == srsran::test_grant_info::UE_UL || test_grant.type == srsran::test_grant_info::PUCCH) {
      if (not cell_res_grid[tx_delay].ul_res_grid.all_set(test_grant.grant)) {
        return false;
      }
    }
  }
  return true;
}

bool srsran::test_res_grid_has_re_set(const cell_resource_allocator& cell_res_grid, grant_info grant, unsigned tx_delay)
{
  return cell_res_grid[tx_delay].ul_res_grid.all_set(grant);
}

void srsran::test_scheduler_result_consistency(const cell_configuration&      cell_cfg,
                                               const cell_resource_allocator& cell_res_grid)
{
  ASSERT_NO_FATAL_FAILURE(test_scheduler_result_consistency(cell_cfg, cell_res_grid[0].slot, cell_res_grid[0].result));
  assert_pdcch_pdsch_common_consistency(cell_cfg, cell_res_grid);
  assert_dl_resource_grid_filled(cell_cfg, cell_res_grid);
  assert_rar_grant_msg3_pusch_consistency(cell_cfg, cell_res_grid);
}
