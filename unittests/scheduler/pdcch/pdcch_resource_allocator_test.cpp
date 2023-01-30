/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../test_utils/config_generators.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>
#include <random>
#include <unordered_map>

using namespace srsgnb;

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
      sched_cell_configuration_request_message msg = test_helpers::make_default_sched_cell_configuration_request()) :
    cell_cfg{msg}
  {
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

  sched_ue_creation_request_message
  create_ue_cfg(rnti_t                             rnti,
                search_space_configuration::type_t ss2_type      = search_space_configuration::type_t::ue_dedicated,
                optional<unsigned>                 cs1_n_id_dmrs = {})
  {
    sched_ue_creation_request_message ue_creation_req = test_helpers::create_default_sched_ue_creation_request();
    ue_creation_req.crnti                             = rnti;
    ue_creation_req.cfg.cells[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg->coresets[0].pdcch_dmrs_scrambling_id =
        cs1_n_id_dmrs;
    ue_creation_req.cfg.cells[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces[0].type = ss2_type;
    return ue_creation_req;
  }

  void verify_pdcch_context(const dci_context_information&    pdcch_ctx,
                            rnti_t                            rnti,
                            const coreset_configuration&      cs_cfg,
                            const search_space_configuration& ss_cfg)
  {
    ASSERT_EQ(pdcch_ctx.rnti, rnti);
    ASSERT_EQ(pdcch_ctx.coreset_cfg, &cs_cfg);
    ASSERT_EQ(pdcch_ctx.n_id_pdcch_dmrs,
              cs_cfg.pdcch_dmrs_scrambling_id.has_value() ? *cs_cfg.pdcch_dmrs_scrambling_id : cell_cfg.pci)
        << "Invalid N_{ID} (see TS38.211, 7.4.1.3.1)";
    ASSERT_EQ(pdcch_ctx.n_rnti_pdcch_data,
              cs_cfg.pdcch_dmrs_scrambling_id.has_value() and
                      ss_cfg.type == search_space_configuration::type_t::ue_dedicated
                  ? rnti
                  : 0)
        << "Invalid n_{RNTI} (see TS38.211, 7.3.2.3)";
    unsigned expected_n_id =
        cs_cfg.pdcch_dmrs_scrambling_id.has_value() and ss_cfg.type == search_space_configuration::type_t::ue_dedicated
            ? *cs_cfg.pdcch_dmrs_scrambling_id
            : cell_cfg.pci;
    ASSERT_EQ(pdcch_ctx.n_id_pdcch_data, expected_n_id) << "Invalid n_{ID} (see TS38.211, 7.3.2.3)";
  }

  srslog::basic_logger&   logger = srslog::fetch_basic_logger("SCHED");
  cell_configuration      cell_cfg;
  cell_resource_allocator res_grid{cell_cfg};

  pdcch_resource_allocator_impl pdcch_sch{cell_cfg};

  slot_point next_slot{0, test_rgen::uniform_int<unsigned>(0, 10239)};

  std::unordered_map<rnti_t, test_ue> test_ues;
};

class pdcch_resource_allocator_tester : public base_pdcch_resource_allocator_tester, public ::testing::Test
{};

TEST_F(pdcch_resource_allocator_tester, no_pdcch_allocation)
{
  ASSERT_TRUE(res_grid[0].result.dl.dl_pdcchs.empty());
  ASSERT_TRUE(res_grid[0].result.dl.ul_pdcchs.empty());
}

TEST_F(pdcch_resource_allocator_tester, single_pdcch_sib1_allocation)
{
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

TEST_F(pdcch_resource_allocator_tester, single_pdcch_rar_allocation)
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

struct test_scrambling_params {
  search_space_id                    ss_id;
  search_space_configuration::type_t ss2_type;
  optional<unsigned>                 cs1_pdcch_dmrs_scrambling_id;
};

class ue_pdcch_resource_allocator_scrambling_tester : public base_pdcch_resource_allocator_tester,
                                                      public ::testing::TestWithParam<test_scrambling_params>
{
protected:
  ue_pdcch_resource_allocator_scrambling_tester() : params(GetParam()) {}

  test_scrambling_params params;
};

TEST_F(pdcch_resource_allocator_tester, when_no_pdcch_space_for_rar_then_allocation_fails)
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

TEST_P(ue_pdcch_resource_allocator_scrambling_tester, single_crnti_dl_pdcch_allocation)
{
  rnti_t   rnti = to_rnti(0x4601 + test_rgen::uniform_int<unsigned>(0, 1000));
  test_ue* u    = this->add_ue(create_ue_cfg(rnti, params.ss2_type, params.cs1_pdcch_dmrs_scrambling_id));

  const search_space_configuration& ss_cfg = u->cfg->search_space(params.ss_id);
  coreset_id                        cs_id  = ss_cfg.cs_id;

  pdcch_dl_information* pdcch =
      pdcch_sch.alloc_dl_pdcch_ue(res_grid[0], rnti, *u->cfg, to_bwp_id(0), params.ss_id, aggregation_level::n4);

  ASSERT_TRUE(res_grid[0].result.dl.ul_pdcchs.empty());
  ASSERT_EQ(res_grid[0].result.dl.dl_pdcchs.size(), 1);
  ASSERT_EQ(pdcch, &res_grid[0].result.dl.dl_pdcchs[0]) << "Returned PDCCH ptr does not match allocated ptr";
  ASSERT_EQ(pdcch->ctx.bwp_cfg, &cell_cfg.dl_cfg_common.init_dl_bwp.generic_params);
  ASSERT_NO_FATAL_FAILURE(verify_pdcch_context(pdcch->ctx, rnti, u->cfg->coreset(cs_id), ss_cfg));
  ASSERT_EQ(pdcch->ctx.cces.aggr_lvl, aggregation_level::n4);
  ASSERT_EQ(pdcch->ctx.starting_symbol, 0);
}

TEST_P(ue_pdcch_resource_allocator_scrambling_tester, single_crnti_ul_pdcch_allocation)
{
  rnti_t   rnti = to_rnti(0x4601 + test_rgen::uniform_int<unsigned>(0, 1000));
  test_ue* u    = this->add_ue(create_ue_cfg(rnti, params.ss2_type, params.cs1_pdcch_dmrs_scrambling_id));
  const search_space_configuration& ss_cfg = u->cfg->search_space(params.ss_id);

  pdcch_ul_information* pdcch =
      pdcch_sch.alloc_ul_pdcch_ue(res_grid[0], rnti, *u->cfg, to_bwp_id(0), params.ss_id, aggregation_level::n4);

  ASSERT_TRUE(res_grid[0].result.dl.dl_pdcchs.empty());
  ASSERT_EQ(res_grid[0].result.dl.ul_pdcchs.size(), 1);
  ASSERT_EQ(pdcch, &res_grid[0].result.dl.ul_pdcchs[0]) << "Returned PDCCH ptr does not match allocated ptr";
  ASSERT_EQ(pdcch->ctx.bwp_cfg, &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params);
  ASSERT_NO_FATAL_FAILURE(verify_pdcch_context(pdcch->ctx, rnti, u->cfg->coreset(ss_cfg.cs_id), ss_cfg));
  ASSERT_EQ(pdcch->ctx.cces.aggr_lvl, aggregation_level::n4);
  ASSERT_EQ(pdcch->ctx.starting_symbol, 0);
}

struct multi_ue_test_params {
  search_space_id   ss_id;
  aggregation_level aggr_lvl;
};

class multi_ue_pdcch_resource_allocator_tester : public base_pdcch_resource_allocator_tester,
                                                 public ::testing::TestWithParam<multi_ue_test_params>
{
protected:
  multi_ue_pdcch_resource_allocator_tester() : params(GetParam()) {}

  multi_ue_test_params params;
};

TEST_P(multi_ue_pdcch_resource_allocator_tester, same_crnti_dl_and_ul_pdcch_allocation)
{
  rnti_t                            rnti           = to_rnti(0x4601 + test_rgen::uniform_int<unsigned>(0, 1000));
  test_ue*                          u              = this->add_ue(create_ue_cfg(rnti));
  const search_space_configuration& ss_cfg         = u->cfg->search_space(params.ss_id);
  unsigned                          nof_candidates = ss_cfg.nof_candidates[to_aggregation_level_index(params.aggr_lvl)];

  pdcch_dl_information* dl_pdcch =
      pdcch_sch.alloc_dl_pdcch_ue(res_grid[0], rnti, *u->cfg, to_bwp_id(0), params.ss_id, params.aggr_lvl);
  pdcch_ul_information* ul_pdcch =
      pdcch_sch.alloc_ul_pdcch_ue(res_grid[0], rnti, *u->cfg, to_bwp_id(0), params.ss_id, params.aggr_lvl);

  ASSERT_EQ(res_grid[0].result.dl.dl_pdcchs.size(), 1);
  ASSERT_NO_FATAL_FAILURE(verify_pdcch_context(dl_pdcch->ctx, rnti, u->cfg->coreset(ss_cfg.cs_id), ss_cfg));
  ASSERT_EQ(res_grid[0].result.dl.ul_pdcchs.size(), nof_candidates > 1 ? 1 : 0);
  ASSERT_EQ(ul_pdcch, nof_candidates > 1 ? &res_grid[0].result.dl.ul_pdcchs[0] : nullptr);
  if (ul_pdcch != nullptr) {
    ASSERT_NO_FATAL_FAILURE(verify_pdcch_context(ul_pdcch->ctx, rnti, u->cfg->coreset(ss_cfg.cs_id), ss_cfg));
    interval<unsigned> dl_cces{dl_pdcch->ctx.cces.ncce, dl_pdcch->ctx.cces.ncce + to_nof_cces(params.aggr_lvl)};
    interval<unsigned> ul_cces{ul_pdcch->ctx.cces.ncce, ul_pdcch->ctx.cces.ncce + to_nof_cces(params.aggr_lvl)};
    ASSERT_FALSE(dl_cces.overlaps(ul_cces));
  }
}

TEST_P(multi_ue_pdcch_resource_allocator_tester, different_crnti_dl_pdcch_allocations)
{
  rnti_t                            rnti1   = to_rnti(0x4601 + test_rgen::uniform_int<unsigned>(0, 1000));
  rnti_t                            rnti2   = to_rnti((uint16_t)rnti1 + 1);
  test_ue*                          u1      = this->add_ue(create_ue_cfg(rnti1));
  test_ue*                          u2      = this->add_ue(create_ue_cfg(rnti1));
  const search_space_configuration& ss_cfg1 = u1->cfg->search_space(params.ss_id);
  const search_space_configuration& ss_cfg2 = u2->cfg->search_space(params.ss_id);
  unsigned nof_candidates                   = ss_cfg1.nof_candidates[to_aggregation_level_index(params.aggr_lvl)];

  pdcch_dl_information* dl_pdcch1 =
      pdcch_sch.alloc_dl_pdcch_ue(res_grid[0], rnti1, *u1->cfg, to_bwp_id(0), params.ss_id, params.aggr_lvl);
  pdcch_dl_information* dl_pdcch2 =
      pdcch_sch.alloc_dl_pdcch_ue(res_grid[0], rnti2, *u2->cfg, to_bwp_id(0), params.ss_id, params.aggr_lvl);

  ASSERT_EQ(res_grid[0].result.dl.dl_pdcchs.size(), nof_candidates > 1 ? 2 : 1);
  ASSERT_EQ(dl_pdcch1, &res_grid[0].result.dl.dl_pdcchs[0]);
  ASSERT_NO_FATAL_FAILURE(verify_pdcch_context(dl_pdcch1->ctx, rnti1, u1->cfg->coreset(ss_cfg1.cs_id), ss_cfg1));
  ASSERT_EQ(dl_pdcch2, nof_candidates > 1 ? &res_grid[0].result.dl.dl_pdcchs[1] : nullptr);
  if (dl_pdcch2 != nullptr) {
    ASSERT_NO_FATAL_FAILURE(verify_pdcch_context(dl_pdcch2->ctx, rnti2, u2->cfg->coreset(ss_cfg2.cs_id), ss_cfg2));
    interval<unsigned> cces1{dl_pdcch1->ctx.cces.ncce, dl_pdcch1->ctx.cces.ncce + to_nof_cces(params.aggr_lvl)};
    interval<unsigned> cces2{dl_pdcch2->ctx.cces.ncce, dl_pdcch2->ctx.cces.ncce + to_nof_cces(params.aggr_lvl)};
    ASSERT_FALSE(cces1.overlaps(cces2));
  }
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
        msg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[1].monitoring_slot_period = period;
        msg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[1].monitoring_slot_offset = offset;
        msg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[1].duration               = duration;

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
    multi_ue_pdcch_allocation_test,
    multi_ue_pdcch_resource_allocator_tester,
    testing::Values(multi_ue_test_params{.ss_id = to_search_space_id(1), .aggr_lvl = srsgnb::aggregation_level::n4},
                    multi_ue_test_params{.ss_id = to_search_space_id(2), .aggr_lvl = srsgnb::aggregation_level::n4},
                    multi_ue_test_params{.ss_id = to_search_space_id(1), .aggr_lvl = srsgnb::aggregation_level::n2},
                    multi_ue_test_params{.ss_id = to_search_space_id(2), .aggr_lvl = srsgnb::aggregation_level::n2}));
