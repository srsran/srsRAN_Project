/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../test_utils/config_generators.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_config_helpers.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "srsran/ran/pdcch/pdcch_candidates.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <random>
#include <unordered_map>

using namespace srsran;

enum class alloc_type { si_rnti, ra_rnti, dl_crnti, ul_crnti };
const char* to_string(alloc_type a)
{
  switch (a) {
    case alloc_type::si_rnti:
      return "si-rnti";
    case alloc_type::ra_rnti:
      return "ra-rnti";
    case alloc_type::dl_crnti:
      return "DL c-rnti";
    case alloc_type::ul_crnti:
      return "UL c-rnti";
    default:
      break;
  }
  return "invalid";
}

using cell_bw = bs_channel_bandwidth_fr1;

namespace pdcch_test {

class base_pdcch_resource_allocator_tester
{
protected:
  struct test_ue {
    rnti_t                                 rnti;
    std::unique_ptr<ue_cell_configuration> cfg;

    test_ue(const cell_configuration& cell_cfg, const sched_ue_creation_request_message& req) :
      rnti(req.crnti), cfg(std::make_unique<ue_cell_configuration>(cell_cfg, req.cfg.cells[0].serv_cell_cfg))
    {
    }
  };

  base_pdcch_resource_allocator_tester(
      sched_cell_configuration_request_message msg     = test_helpers::make_default_sched_cell_configuration_request(),
      cell_config_dedicated                    ue_cell = test_helpers::create_test_initial_ue_spcell_cell_config()) :
    cell_cfg{msg}, default_ue_cfg{cell_cfg, ue_cell.serv_cell_cfg}
  {
    test_logger.set_level(srslog::basic_levels::debug);
    srslog::init();

    run_slot();
  }

  void run_slot()
  {
    logger.set_context(next_slot.sfn(), next_slot.slot_index());
    res_grid.slot_indication(next_slot);
    pdcch_sch.slot_indication(next_slot);
    ++next_slot;
  }

  test_ue* add_ue(const sched_ue_creation_request_message& ue_creation_req)
  {
    return &test_ues.insert(std::make_pair(ue_creation_req.crnti, test_ue{cell_cfg, ue_creation_req})).first->second;
  }

  sched_ue_creation_request_message create_ue_cfg(rnti_t rnti)
  {
    sched_ue_creation_request_message ue_creation_req = test_helpers::create_default_sched_ue_creation_request();
    ue_creation_req.crnti                             = rnti;
    ue_creation_req.starts_in_fallback                = false;
    ue_creation_req.cfg.cells[0].serv_cell_cfg        = default_ue_cfg.cfg_dedicated();
    return ue_creation_req;
  }

  void verify_pdcch_context(const dci_context_information& pdcch_ctx, const test_ue& u, search_space_id ss_id) const
  {
    ASSERT_EQ(pdcch_ctx.rnti, u.rnti);
    const search_space_configuration& ss_cfg = *u.cfg->search_space(ss_id).cfg;
    const coreset_configuration&      cs_cfg = u.cfg->coreset(ss_cfg.get_coreset_id());
    ASSERT_EQ(pdcch_ctx.coreset_cfg, &cs_cfg);
    ASSERT_EQ(pdcch_ctx.n_id_pdcch_dmrs,
              cs_cfg.pdcch_dmrs_scrambling_id.has_value() ? *cs_cfg.pdcch_dmrs_scrambling_id : cell_cfg.pci)
        << "Invalid N_{ID} (see TS38.211, 7.4.1.3.1)";
    ASSERT_EQ(pdcch_ctx.n_rnti_pdcch_data,
              cs_cfg.pdcch_dmrs_scrambling_id.has_value() and (not ss_cfg.is_common_search_space()) ? u.rnti : 0)
        << "Invalid n_{RNTI} (see TS38.211, 7.3.2.3)";
    unsigned expected_n_id = cs_cfg.pdcch_dmrs_scrambling_id.has_value() and (not ss_cfg.is_common_search_space())
                                 ? *cs_cfg.pdcch_dmrs_scrambling_id
                                 : cell_cfg.pci;
    ASSERT_EQ(pdcch_ctx.n_id_pdcch_data, expected_n_id) << "Invalid n_{ID} (see TS38.211, 7.3.2.3)";

    auto ncce_candidates =
        get_ue_pdcch_candidates((next_slot - 1).slot_index(), u.rnti, cs_cfg, ss_cfg, pdcch_ctx.cces.aggr_lvl);
    ASSERT_TRUE(std::any_of(ncce_candidates.begin(), ncce_candidates.end(), [&pdcch_ctx](auto ncce) {
      return ncce == pdcch_ctx.cces.ncce;
    })) << "Chosen ncce is not in the list of candidates";
  }

  pdcch_candidate_list get_common_pdcch_candidates(const coreset_configuration&      cs_cfg,
                                                   const search_space_configuration& ss_cfg,
                                                   aggregation_level                 aggr_lvl) const
  {
    return pdcch_candidates_common_ss_get_lowest_cce(pdcch_candidates_common_ss_configuration{
        aggr_lvl, ss_cfg.get_nof_candidates()[to_aggregation_level_index(aggr_lvl)], cs_cfg.get_nof_cces()});
  }

  pdcch_candidate_list get_ue_pdcch_candidates(unsigned                          slot_index,
                                               rnti_t                            rnti,
                                               const coreset_configuration&      cs_cfg,
                                               const search_space_configuration& ss_cfg,
                                               aggregation_level                 aggr_lvl) const
  {
    if (ss_cfg.is_common_search_space()) {
      return get_common_pdcch_candidates(cs_cfg, ss_cfg, aggr_lvl);
    }
    return pdcch_candidates_ue_ss_get_lowest_cce(
        pdcch_candidates_ue_ss_configuration{aggr_lvl,
                                             ss_cfg.get_nof_candidates()[to_aggregation_level_index(aggr_lvl)],
                                             cs_cfg.get_nof_cces(),
                                             cs_cfg.id,
                                             rnti,
                                             slot_index});
  }

  bool pdcchs_collide(const dci_context_information& pdcch_ctx1, const dci_context_information& pdcch_ctx2) const
  {
    prb_index_list pdcch_prbs1 = cce_to_prb_mapping(
        *pdcch_ctx1.bwp_cfg, *pdcch_ctx1.coreset_cfg, cell_cfg.pci, pdcch_ctx1.cces.aggr_lvl, pdcch_ctx1.cces.ncce);
    prb_index_list pdcch_prbs2 = cce_to_prb_mapping(
        *pdcch_ctx2.bwp_cfg, *pdcch_ctx2.coreset_cfg, cell_cfg.pci, pdcch_ctx2.cces.aggr_lvl, pdcch_ctx2.cces.ncce);
    for (unsigned prb1 : pdcch_prbs1) {
      if (std::find(pdcch_prbs2.begin(), pdcch_prbs2.end(), prb1) != pdcch_prbs2.end()) {
        return true;
      }
    }
    return false;
  }

  bool collisions_found() const
  {
    const auto& dl_pdcchs = res_grid[0].result.dl.dl_pdcchs;
    const auto& ul_pdcchs = res_grid[0].result.dl.ul_pdcchs;
    for (auto it = dl_pdcchs.begin(); it != dl_pdcchs.end(); ++it) {
      for (auto it2 = it + 1; it2 != dl_pdcchs.end(); ++it2) {
        if (pdcchs_collide(it->ctx, it2->ctx)) {
          return true;
        }
      }
      for (auto it2 = ul_pdcchs.begin(); it2 != ul_pdcchs.end(); ++it2) {
        if (pdcchs_collide(it->ctx, it2->ctx)) {
          return true;
        }
      }
    }
    return false;
  }

  optional<unsigned> find_available_ncce(const bwp_configuration&          bwp_cfg,
                                         const coreset_configuration&      cs_cfg,
                                         const search_space_configuration& ss_cfg,
                                         aggregation_level                 aggr_lvl)
  {
    auto ncce_candidates = this->get_common_pdcch_candidates(cs_cfg, ss_cfg, aggr_lvl);
    for (auto ncce : ncce_candidates) {
      prb_index_list pdcch_prbs = cce_to_prb_mapping(bwp_cfg, cs_cfg, cell_cfg.pci, aggr_lvl, ncce);
      bool           success    = true;
      for (unsigned prb : pdcch_prbs) {
        unsigned crb = prb_to_crb(bwp_cfg, prb);
        if (res_grid[0].dl_res_grid.collides(bwp_cfg.scs, {0, cs_cfg.duration}, crb_interval{crb, crb + 1})) {
          success = false;
          break;
        }
      }
      if (success) {
        return ncce;
      }
    }
    return nullopt;
  }

  void print_cfg()
  {
    fmt::memory_buffer fmtbuf;
    fmt::format_to(fmtbuf, "\nTest config:");
    fmt::format_to(fmtbuf, "\n- initial BWP: RBs={}", cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.crbs);
    const auto& cs0_cfg = default_ue_cfg.coreset(to_coreset_id(0));
    fmt::format_to(fmtbuf, "\n- CORESET#0: RBs={}, duration={}", cs0_cfg.coreset0_crbs(), cs0_cfg.duration);
    const auto& cs1_cfg = default_ue_cfg.coreset(to_coreset_id(1));
    fmt::format_to(fmtbuf, "\n- CORESET#1: RBs={}, duration={}", get_coreset_crbs(cs1_cfg), cs1_cfg.duration);
    fmt::format_to(
        fmtbuf,
        "\n- SearchSpace#0: nof_candidates={}",
        fmt::join(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[0].get_nof_candidates(), ", "));
    fmt::format_to(
        fmtbuf,
        "\n- SearchSpace#1: nof_candidates={}",
        fmt::join(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[1].get_nof_candidates(), ", "));
    fmt::format_to(fmtbuf,
                   "\n- SearchSpace#2: nof_candidates={}",
                   fmt::join(default_ue_cfg.search_space(to_search_space_id(2)).cfg->get_nof_candidates(), ", "));
    test_logger.info("{}", to_string(fmtbuf));
  }

  srslog::basic_logger& logger      = srslog::fetch_basic_logger("SCHED");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
  cell_configuration    cell_cfg;
  ue_cell_configuration default_ue_cfg;

  cell_resource_allocator res_grid{cell_cfg};

  pdcch_resource_allocator_impl pdcch_sch{cell_cfg};

  slot_point next_slot{0, test_rgen::uniform_int<unsigned>(0, 10239)};

  std::unordered_map<rnti_t, test_ue> test_ues;
};

class common_pdcch_allocator_tester : public base_pdcch_resource_allocator_tester, public ::testing::Test
{};

struct test_scrambling_params {
  search_space_id                    ss_id;
  search_space_configuration::type_t ss2_type;
  optional<unsigned>                 cs1_pdcch_dmrs_scrambling_id;
};

// Dummy function overload of template <typename T> void testing::internal::PrintTo(const T& value, ::std::ostream* os).
// This prevents valgrind from complaining about uninitialized variables.
void PrintTo(const test_scrambling_params& value, ::std::ostream* os)
{
  return;
}
} // namespace pdcch_test

using namespace pdcch_test;

TEST_F(common_pdcch_allocator_tester, no_pdcch_allocation)
{
  ASSERT_TRUE(res_grid[0].result.dl.dl_pdcchs.empty());
  ASSERT_TRUE(res_grid[0].result.dl.ul_pdcchs.empty());
}

TEST_F(common_pdcch_allocator_tester, single_pdcch_sib1_allocation)
{
  // Run until PDCCH monitoring occasion for SIB1 is active (i.e. every 20ms)
  while (not is_pdcch_monitoring_active(next_slot, cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[0])) {
    run_slot();
  }
  // Since we schedule SIB1 on (n0 + 1)th slot we need to run once more.
  run_slot();

  pdcch_dl_information* pdcch =
      pdcch_sch.alloc_pdcch_common(res_grid[0], SI_RNTI, to_search_space_id(0), aggregation_level::n4);

  ASSERT_TRUE(res_grid[0].result.dl.ul_pdcchs.empty());
  ASSERT_EQ(res_grid[0].result.dl.dl_pdcchs.size(), 1);
  ASSERT_EQ(pdcch, &res_grid[0].result.dl.dl_pdcchs[0]) << "Returned PDCCH ptr does not match allocated ptr";
  ASSERT_EQ(SI_RNTI, pdcch->ctx.rnti);
  ASSERT_EQ(pdcch->ctx.bwp_cfg, &cell_cfg.dl_cfg_common.init_dl_bwp.generic_params);
  ASSERT_EQ(pdcch->ctx.coreset_cfg, &*cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0);
  ASSERT_EQ(pdcch->ctx.n_id_pdcch_dmrs, cell_cfg.pci) << "Invalid N_{ID} (see TS38.211, 7.4.1.3.1)";
  ASSERT_EQ(pdcch->ctx.n_rnti_pdcch_data, 0) << "Invalid n_{RNTI} (see TS38.211, 7.3.2.3)";
  ASSERT_EQ(pdcch->ctx.n_id_pdcch_data, cell_cfg.pci) << "Invalid n_{ID} (see TS38.211, 7.3.2.3)";
  ASSERT_EQ(pdcch->ctx.cces.aggr_lvl, aggregation_level::n4);
  ASSERT_EQ(pdcch->ctx.cces.ncce, 0);
  ASSERT_EQ(pdcch->ctx.starting_symbol, 0);
}

TEST_F(common_pdcch_allocator_tester, single_pdcch_rar_allocation)
{
  rnti_t                ra_rnti = to_rnti(test_rgen::uniform_int<unsigned>(1, 9));
  pdcch_dl_information* pdcch =
      pdcch_sch.alloc_pdcch_common(res_grid[0], ra_rnti, to_search_space_id(1), aggregation_level::n4);

  ASSERT_TRUE(res_grid[0].result.dl.ul_pdcchs.empty());
  ASSERT_EQ(res_grid[0].result.dl.dl_pdcchs.size(), 1);
  ASSERT_EQ(pdcch, &res_grid[0].result.dl.dl_pdcchs[0]) << "Returned PDCCH ptr does not match allocated ptr";
  ASSERT_EQ(ra_rnti, pdcch->ctx.rnti);
  ASSERT_EQ(pdcch->ctx.bwp_cfg, &cell_cfg.dl_cfg_common.init_dl_bwp.generic_params);
  ASSERT_EQ(pdcch->ctx.coreset_cfg, &*cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0);
  ASSERT_EQ(pdcch->ctx.n_id_pdcch_dmrs, cell_cfg.pci) << "Invalid N_{ID} (see TS38.211, 7.4.1.3.1)";
  ASSERT_EQ(pdcch->ctx.n_rnti_pdcch_data, 0) << "Invalid n_{RNTI} (see TS38.211, 7.3.2.3)";
  ASSERT_EQ(pdcch->ctx.n_id_pdcch_data, cell_cfg.pci) << "Invalid n_{ID} (see TS38.211, 7.3.2.3)";
  ASSERT_EQ(pdcch->ctx.cces.aggr_lvl, aggregation_level::n4);
  ASSERT_EQ(pdcch->ctx.cces.ncce, 0);
  ASSERT_EQ(pdcch->ctx.starting_symbol, 0);
}

TEST_F(common_pdcch_allocator_tester, when_no_pdcch_space_for_rar_then_allocation_fails)
{
  rnti_t                ra_rnti1 = to_rnti(test_rgen::uniform_int<unsigned>(1, 9));
  rnti_t                ra_rnti2 = to_rnti(test_rgen::uniform_int<unsigned>(1, 9));
  pdcch_dl_information* pdcch1 =
      pdcch_sch.alloc_pdcch_common(res_grid[0], ra_rnti1, to_search_space_id(1), aggregation_level::n4);
  pdcch_dl_information* pdcch2 =
      pdcch_sch.alloc_pdcch_common(res_grid[0], ra_rnti2, to_search_space_id(1), aggregation_level::n4);

  ASSERT_EQ(1, res_grid[0].result.dl.dl_pdcchs.size());
  ASSERT_EQ(pdcch1, &res_grid[0].result.dl.dl_pdcchs[0]);
  ASSERT_EQ(pdcch1->ctx.rnti, ra_rnti1);
  ASSERT_EQ(pdcch2, nullptr);
}

class ue_pdcch_resource_allocator_scrambling_tester : public base_pdcch_resource_allocator_tester,
                                                      public ::testing::TestWithParam<test_scrambling_params>
{
protected:
  ue_pdcch_resource_allocator_scrambling_tester() : params(GetParam()) {}

  sched_ue_creation_request_message
  create_ue_cfg(rnti_t                             rnti,
                search_space_configuration::type_t ss2_type      = search_space_configuration::type_t::ue_dedicated,
                optional<unsigned>                 cs1_n_id_dmrs = {})
  {
    auto ue_creation_req = base_pdcch_resource_allocator_tester::create_ue_cfg(rnti);
    ue_creation_req.cfg.cells[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg->coresets[0].pdcch_dmrs_scrambling_id =
        cs1_n_id_dmrs;
    if (ss2_type == srsran::search_space_type::common) {
      ue_creation_req.cfg.cells[0]
          .serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces[0]
          .set_non_ss0_monitored_dci_formats(search_space_configuration::common_dci_format{.f0_0_and_f1_0 = true});
    } else {
      ue_creation_req.cfg.cells[0]
          .serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces[0]
          .set_non_ss0_monitored_dci_formats(search_space_configuration::ue_specific_dci_format::f0_1_and_1_1);
    }
    return ue_creation_req;
  }

  test_scrambling_params params;
};

TEST_P(ue_pdcch_resource_allocator_scrambling_tester, single_crnti_dl_pdcch_allocation)
{
  rnti_t   rnti = to_rnti(0x4601 + test_rgen::uniform_int<unsigned>(0, 1000));
  test_ue* u    = this->add_ue(create_ue_cfg(rnti, params.ss2_type, params.cs1_pdcch_dmrs_scrambling_id));

  pdcch_dl_information* pdcch =
      pdcch_sch.alloc_dl_pdcch_ue(res_grid[0], rnti, *u->cfg, params.ss_id, aggregation_level::n4);

  ASSERT_TRUE(res_grid[0].result.dl.ul_pdcchs.empty());
  ASSERT_EQ(res_grid[0].result.dl.dl_pdcchs.size(), 1);
  ASSERT_EQ(pdcch, &res_grid[0].result.dl.dl_pdcchs[0]) << "Returned PDCCH ptr does not match allocated ptr";
  ASSERT_EQ(pdcch->ctx.bwp_cfg, &cell_cfg.dl_cfg_common.init_dl_bwp.generic_params);
  ASSERT_NO_FATAL_FAILURE(verify_pdcch_context(pdcch->ctx, *u, params.ss_id));
  ASSERT_EQ(pdcch->ctx.cces.aggr_lvl, aggregation_level::n4);
  ASSERT_EQ(pdcch->ctx.starting_symbol, 0);
}

TEST_P(ue_pdcch_resource_allocator_scrambling_tester, single_crnti_ul_pdcch_allocation)
{
  rnti_t   rnti = to_rnti(0x4601 + test_rgen::uniform_int<unsigned>(0, 1000));
  test_ue* u    = this->add_ue(create_ue_cfg(rnti, params.ss2_type, params.cs1_pdcch_dmrs_scrambling_id));

  pdcch_ul_information* pdcch =
      pdcch_sch.alloc_ul_pdcch_ue(res_grid[0], rnti, *u->cfg, params.ss_id, aggregation_level::n4);

  ASSERT_TRUE(res_grid[0].result.dl.dl_pdcchs.empty());
  ASSERT_EQ(res_grid[0].result.dl.ul_pdcchs.size(), 1);
  ASSERT_EQ(pdcch, &res_grid[0].result.dl.ul_pdcchs[0]) << "Returned PDCCH ptr does not match allocated ptr";
  ASSERT_EQ(pdcch->ctx.bwp_cfg, &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params);
  ASSERT_NO_FATAL_FAILURE(verify_pdcch_context(pdcch->ctx, *u, params.ss_id));
  ASSERT_EQ(pdcch->ctx.cces.aggr_lvl, aggregation_level::n4);
  ASSERT_EQ(pdcch->ctx.starting_symbol, 0);
}

TEST(pdcch_resource_allocator_test, monitoring_period)
{
  for (unsigned period : {20, 40}) {
    for (unsigned duration = 1; duration < period - 1; ++duration) {
      for (unsigned offset = 0; offset < period - 1; ++offset) {
        // Note: First slot is one slot before the PDCCH monitoring window.
        unsigned randint  = test_rgen::uniform_int<unsigned>(0, 10239);
        unsigned first_sl = (((randint / period) * period) + offset + 10239) % 10240;

        sched_cell_configuration_request_message msg = test_helpers::make_default_sched_cell_configuration_request();
        msg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[1].set_non_ss0_monitoring_slot_periodicity(period);
        msg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[1].set_non_ss0_monitoring_slot_offset(offset,
                                                                                                       msg.scs_common);
        msg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[1].set_non_ss0_duration(duration);

        cell_configuration      cfg{msg};
        cell_resource_allocator res_grid{cfg};

        pdcch_resource_allocator_impl pdcch_sch(cfg);

        // Action: Attempt allocation of one RAR outside PDCCH monitoring window.
        slot_point sl_tx{0, first_sl};

        // Action: Attempt to allocate PDCCHs outside and inside PDCCH monitoring window.
        rnti_t            ra_rnti = to_rnti(1);
        std::vector<bool> expected_result(duration + 2, true);
        expected_result[0]     = false;
        expected_result.back() = false;
        for (unsigned i = 0; i < duration + 2; ++i) {
          res_grid.slot_indication(sl_tx);
          pdcch_sch.slot_indication(sl_tx);

          pdcch_dl_information* pdcch =
              pdcch_sch.alloc_pdcch_common(res_grid[0], ra_rnti, to_search_space_id(1), aggregation_level::n4);

          if (expected_result[i]) {
            // Inside PDCCH monitoring window.
            TESTASSERT(pdcch != nullptr);
            TESTASSERT(pdcch->ctx.rnti == ra_rnti);
          } else {
            // Outside PDCCH monitoring window.
            TESTASSERT(pdcch == nullptr);
          }

          sl_tx++;
        }
      }
    }
  }
}

struct multi_alloc_test_params {
  struct alloc {
    alloc_type         type;
    rnti_t             rnti;
    aggregation_level  aggr_lvl;
    search_space_id    ss_id; // C-RNTI only
    optional<unsigned> expected_ncce;
  };

  bs_channel_bandwidth_fr1         cell_bw;
  optional<std::array<uint8_t, 5>> ss2_nof_candidates;
  std::vector<alloc>               allocs;
};

template <>
struct fmt::formatter<multi_alloc_test_params> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }
  template <typename FormatContext>
  auto format(const multi_alloc_test_params& params, FormatContext& ctx)
  {
    fmt::format_to(ctx.out(), "bw={}MHz allocs=[", bs_channel_bandwidth_to_MHz(params.cell_bw));
    for (const auto& a : params.allocs) {
      fmt::format_to(ctx.out(),
                     "{}{{{}={:#x} al={}, ss_id={} -> cce={}}}",
                     &a == params.allocs.data() ? "" : ", ",
                     to_string(a.type),
                     a.rnti,
                     to_nof_cces(a.aggr_lvl),
                     a.ss_id,
                     a.expected_ncce);
    }
    return format_to(ctx.out(), "]");
  }
};

// Dummy function overload of template <typename T> void testing::internal::PrintTo(const T& value, ::std::ostream* os).
// This prevents valgrind from complaining about uninitialized variables.
void PrintTo(const multi_alloc_test_params& value, ::std::ostream* os)
{
  return;
}

class multi_alloc_pdcch_resource_allocator_tester : public base_pdcch_resource_allocator_tester,
                                                    public ::testing::TestWithParam<multi_alloc_test_params>
{
  using super_type = base_pdcch_resource_allocator_tester;

protected:
  multi_alloc_pdcch_resource_allocator_tester() :
    base_pdcch_resource_allocator_tester(
        [params = GetParam()]() {
          sched_cell_configuration_request_message msg = test_helpers::make_default_sched_cell_configuration_request(
              cell_config_builder_params{.channel_bw_mhz = params.cell_bw});
          return msg;
        }(),
        [params = GetParam()]() {
          cell_config_dedicated ue_cell = test_helpers::create_test_initial_ue_spcell_cell_config(
              cell_config_builder_params{.channel_bw_mhz = params.cell_bw});
          if (params.ss2_nof_candidates.has_value()) {
            ue_cell.serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces[0].set_non_ss0_nof_candidates(
                *params.ss2_nof_candidates);
          }
          return ue_cell;
        }()),
    params(GetParam())
  {
  }

  sched_ue_creation_request_message create_ue_cfg(rnti_t rnti)
  {
    auto ue_creation_req = super_type::create_ue_cfg(rnti);
    ue_creation_req.cfg.cells[0]
        .serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces[0]
        .set_non_ss0_monitored_dci_formats(search_space_configuration::ue_specific_dci_format::f0_1_and_1_1);
    cell_config_builder_params builder_params{};
    builder_params.channel_bw_mhz = params.cell_bw;
    ue_creation_req.cfg.cells[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg->coresets[0] =
        config_helpers::make_default_coreset_config(builder_params);
    return ue_creation_req;
  }

  const dci_context_information* allocate_pdcch(multi_alloc_test_params::alloc alloc)
  {
    test_ue* u = nullptr;
    if (alloc.type == alloc_type::dl_crnti or alloc.type == alloc_type::ul_crnti) {
      if (this->test_ues.count(alloc.rnti) == 0) {
        u = this->add_ue(create_ue_cfg(alloc.rnti));
      } else {
        u = &this->test_ues.at(alloc.rnti);
      }
    }

    switch (alloc.type) {
      case alloc_type::si_rnti: {
        // Run until PDCCH monitoring occasion for SIB1 is active (i.e. every 20ms)
        while (not is_pdcch_monitoring_active(next_slot,
                                              cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[0])) {
          run_slot();
        }
        // Since we schedule SIB1 on (n0 + 1)th slot we need to run once more.
        run_slot();
        pdcch_dl_information* sib_pdcch =
            pdcch_sch.alloc_pdcch_common(res_grid[0], SI_RNTI, to_search_space_id(0), alloc.aggr_lvl);
        return sib_pdcch != nullptr ? &sib_pdcch->ctx : nullptr;
      } break;
      case alloc_type::ra_rnti: {
        pdcch_dl_information* rar_pdcch =
            pdcch_sch.alloc_pdcch_common(res_grid[0], alloc.rnti, to_search_space_id(1), alloc.aggr_lvl);
        return rar_pdcch != nullptr ? &rar_pdcch->ctx : nullptr;

      } break;
      case alloc_type::dl_crnti: {
        pdcch_dl_information* dl_pdcch =
            pdcch_sch.alloc_dl_pdcch_ue(res_grid[0], alloc.rnti, *u->cfg, alloc.ss_id, alloc.aggr_lvl);
        return dl_pdcch != nullptr ? &dl_pdcch->ctx : nullptr;

      } break;
      case alloc_type::ul_crnti: {
        pdcch_ul_information* ul_pdcch =
            pdcch_sch.alloc_ul_pdcch_ue(res_grid[0], alloc.rnti, *u->cfg, alloc.ss_id, alloc.aggr_lvl);
        return ul_pdcch != nullptr ? &ul_pdcch->ctx : nullptr;

      } break;
      default:
        report_fatal_error("Invalid RNTI type");
    }
    return nullptr;
  }

  const dci_context_information* find_pdcch_alloc(multi_alloc_test_params::alloc alloc) const
  {
    if (alloc.type == alloc_type::ul_crnti) {
      auto it = std::find_if(res_grid[0].result.dl.ul_pdcchs.begin(),
                             res_grid[0].result.dl.ul_pdcchs.end(),
                             [&alloc](const auto& pdcch) { return pdcch.ctx.rnti == alloc.rnti; });
      return it != res_grid[0].result.dl.ul_pdcchs.end() ? &it->ctx : nullptr;
    }
    auto it = std::find_if(res_grid[0].result.dl.dl_pdcchs.begin(),
                           res_grid[0].result.dl.dl_pdcchs.end(),
                           [&alloc](const auto& pdcch) { return pdcch.ctx.rnti == alloc.rnti; });
    return it != res_grid[0].result.dl.dl_pdcchs.end() ? &it->ctx : nullptr;
  }

  multi_alloc_test_params params;
};

TEST_P(multi_alloc_pdcch_resource_allocator_tester, pdcch_allocation_outcome)
{
  test_logger.info("Test params: {}", params);
  print_cfg();
  srslog::flush();

  for (const multi_alloc_test_params::alloc& a : params.allocs) {
    this->allocate_pdcch(a);
  }

  ASSERT_FALSE(collisions_found());
  for (const multi_alloc_test_params::alloc& a : params.allocs) {
    const dci_context_information* ctx = this->find_pdcch_alloc(a);
    ASSERT_EQ(ctx != nullptr, a.expected_ncce.has_value())
        << fmt::format("For rnti={:#x} expected cce={}", a.rnti, a.expected_ncce);
    if (ctx != nullptr) {
      if (a.type == alloc_type::dl_crnti or a.type == alloc_type::ul_crnti) {
        ASSERT_NO_FATAL_FAILURE(verify_pdcch_context(*ctx, test_ues.at(a.rnti), a.ss_id));
      }
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
    n_id_scrambling_derivation_test,
    ue_pdcch_resource_allocator_scrambling_tester,
    testing::Values(test_scrambling_params{.ss_id                        = to_search_space_id(1),
                                           .ss2_type                     = search_space_configuration::type_t::common,
                                           .cs1_pdcch_dmrs_scrambling_id = nullopt},
                    test_scrambling_params{.ss_id                        = to_search_space_id(2),
                                           .ss2_type                     = search_space_configuration::type_t::common,
                                           .cs1_pdcch_dmrs_scrambling_id = nullopt},
                    test_scrambling_params{.ss_id                        = to_search_space_id(2),
                                           .ss2_type                     = search_space_configuration::type_t::common,
                                           .cs1_pdcch_dmrs_scrambling_id = 5},
                    test_scrambling_params{.ss_id    = to_search_space_id(2),
                                           .ss2_type = search_space_configuration::type_t::ue_dedicated,
                                           .cs1_pdcch_dmrs_scrambling_id = nullopt},
                    test_scrambling_params{.ss_id    = to_search_space_id(2),
                                           .ss2_type = search_space_configuration::type_t::ue_dedicated,
                                           .cs1_pdcch_dmrs_scrambling_id = 5}));

INSTANTIATE_TEST_SUITE_P(
    multi_alloc_pdcch_allocation_test,
    multi_alloc_pdcch_resource_allocator_tester,
    testing::Values(
        // clang-format off
  //    allocation type        RNTI            aggregation level      search space        expected NCCE (nullopt=no alloc)
  multi_alloc_test_params{cell_bw::MHz10, nullopt,
    {{alloc_type::dl_crnti, to_rnti(0x4601), aggregation_level::n4, to_search_space_id(2), 0}}},
  multi_alloc_test_params{cell_bw::MHz10, nullopt,
    {{alloc_type::dl_crnti, to_rnti(0x4601), aggregation_level::n4, to_search_space_id(2), 0},
     {alloc_type::ul_crnti, to_rnti(0x4601), aggregation_level::n4, to_search_space_id(2), 4}}},
  multi_alloc_test_params{cell_bw::MHz10, nullopt,
    {{alloc_type::si_rnti,  SI_RNTI,         aggregation_level::n4, to_search_space_id(0), 0},
     {alloc_type::ul_crnti, to_rnti(0x4601), aggregation_level::n4, to_search_space_id(1), nullopt}}},
  multi_alloc_test_params{cell_bw::MHz10, nullopt,
    {{alloc_type::si_rnti,  SI_RNTI,         aggregation_level::n4, to_search_space_id(0), 0},
     {alloc_type::ul_crnti, to_rnti(0x4601), aggregation_level::n4, to_search_space_id(2), 4}}},
  multi_alloc_test_params{cell_bw::MHz20, nullopt,
    {{alloc_type::si_rnti,  SI_RNTI,         aggregation_level::n4, to_search_space_id(0), 0},
     {alloc_type::ul_crnti, to_rnti(0x4601), aggregation_level::n4, to_search_space_id(2), 8}}},
  multi_alloc_test_params{cell_bw::MHz10, std::array<uint8_t, 5>{0,5,0,0,0},
    {{alloc_type::dl_crnti, to_rnti(0x4601), aggregation_level::n2, to_search_space_id(2), 0},
     {alloc_type::dl_crnti, to_rnti(0x4602), aggregation_level::n2, to_search_space_id(2), 2},
     {alloc_type::dl_crnti, to_rnti(0x4603), aggregation_level::n2, to_search_space_id(2), 4},
     {alloc_type::ul_crnti, to_rnti(0x4601), aggregation_level::n2, to_search_space_id(2), 6},
     {alloc_type::ul_crnti, to_rnti(0x4602), aggregation_level::n2, to_search_space_id(2), 8}}},
  multi_alloc_test_params{cell_bw::MHz20, std::array<uint8_t, 5>{0,0,5,0,0},
    {{alloc_type::dl_crnti, to_rnti(0x4601), aggregation_level::n4, to_search_space_id(2), 0},
     {alloc_type::dl_crnti, to_rnti(0x4602), aggregation_level::n4, to_search_space_id(2), 4},
     {alloc_type::dl_crnti, to_rnti(0x4603), aggregation_level::n4, to_search_space_id(2), 8},
     {alloc_type::dl_crnti, to_rnti(0x4604), aggregation_level::n4, to_search_space_id(2), 12},
     {alloc_type::dl_crnti, to_rnti(0x4605), aggregation_level::n4, to_search_space_id(2), 14}}}
));
// clang-format on
