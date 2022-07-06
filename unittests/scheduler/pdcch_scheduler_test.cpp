
#include "config_generators.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_scheduler_impl.h"
#include "srsgnb/mac/mac_configuration_helpers.h"
#include "srsgnb/support/test_utils.h"
#include <random>

using namespace srsgnb;

void test_pdcch_sched_sib1()
{
  test_delimit_logger delimiter{"Test PDCCH Allocation SIB1"};

  sched_cell_configuration_request_message msg = make_default_sched_cell_configuration_request();
  cell_configuration                       cfg{msg};
  cell_resource_allocator                  res_grid{cfg};

  pdcch_scheduler_impl pdcch_sch(cfg);

  slot_point sl_tx{0, 0};

  res_grid.slot_indication(sl_tx);
  pdcch_sch.slot_indication(sl_tx);

  // TEST: No allocations have been done yet.
  TESTASSERT(res_grid[0].result.dl.dl_pdcchs.empty());

  // Action: Allocate one SIB1.
  pdcch_dl_information* pdcch =
      pdcch_sch.alloc_pdcch_common(res_grid[0], SI_RNTI, to_search_space_id(0), aggregation_level::n4);

  // TEST: SIB1 allocation should be successful and the PDCCH contents valid.
  TESTASSERT_EQ(1, res_grid[0].result.dl.dl_pdcchs.size());
  TESTASSERT(pdcch == &res_grid[0].result.dl.dl_pdcchs[0]);
  TESTASSERT(pdcch->ctx.bwp_cfg == &cfg.dl_cfg_common.init_dl_bwp.generic_params);
  TESTASSERT(pdcch->ctx.coreset_cfg == &*cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0);
  TESTASSERT_EQ(aggregation_level::n4, pdcch->ctx.cces.aggr_lvl);
  TESTASSERT_EQ(SI_RNTI, pdcch->ctx.rnti);
  // TODO: Check CCE position is valid.
}

void test_pdcch_sched_rar()
{
  test_delimit_logger delimiter{"Test PDCCH Allocation RAR"};

  sched_cell_configuration_request_message msg = make_default_sched_cell_configuration_request();
  cell_configuration                       cfg{msg};
  cell_resource_allocator                  res_grid{cfg};

  pdcch_scheduler_impl pdcch_sch(cfg);

  slot_point sl_tx{0, 0};

  res_grid.slot_indication(sl_tx);
  pdcch_sch.slot_indication(sl_tx);

  // Action: Allocate one RAR.
  rnti_t                ra_rnti = to_rnti(1);
  pdcch_dl_information* pdcch =
      pdcch_sch.alloc_pdcch_common(res_grid[0], ra_rnti, to_search_space_id(1), aggregation_level::n4);

  // TEST: RAR allocation should be successful and the PDCCH contents valid.
  TESTASSERT_EQ(1, res_grid[0].result.dl.dl_pdcchs.size());
  TESTASSERT(pdcch == &res_grid[0].result.dl.dl_pdcchs[0]);
  TESTASSERT(pdcch->ctx.bwp_cfg == &cfg.dl_cfg_common.init_dl_bwp.generic_params);
  TESTASSERT(pdcch->ctx.coreset_cfg == &*cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0);
  TESTASSERT_EQ(aggregation_level::n4, pdcch->ctx.cces.aggr_lvl);
  TESTASSERT_EQ(ra_rnti, pdcch->ctx.rnti);

  // Action: Try allocate another RAR that fails due to lack of PDCCH resources.
  ra_rnti = to_rnti(2);
  pdcch   = pdcch_sch.alloc_pdcch_common(res_grid[0], ra_rnti, to_search_space_id(0), aggregation_level::n4);

  // TEST: RAR allocation should fail.
  TESTASSERT_EQ(1, res_grid[0].result.dl.dl_pdcchs.size());
  TESTASSERT(pdcch == nullptr);
}

void test_pdcch_sched_ue()
{
  test_delimit_logger delimiter{"Test PDCCH Allocation UE DL"};

  sched_cell_configuration_request_message msg = make_default_sched_cell_configuration_request();
  sched_ue_creation_request_message        ue_creation_msg =
      make_scheduler_ue_creation_request(test_helpers::make_default_ue_creation_request());
  cell_configuration      cfg{msg};
  cell_resource_allocator res_grid{cfg};

  ue_cell_configuration ue_cfg{cfg, ue_creation_msg.serv_cell_cfg};
  bwp_id_t              bwpid = to_bwp_id(0);

  pdcch_scheduler_impl pdcch_sch(cfg);

  slot_point sl_tx{0, 0};
  res_grid.slot_indication(sl_tx);
  pdcch_sch.slot_indication(sl_tx);

  // Action: Allocate one UE.
  rnti_t                rnti  = to_rnti(0x4601);
  pdcch_dl_information* pdcch = pdcch_sch.alloc_dl_pdcch_ue(
      res_grid[0], rnti, ue_cfg, bwpid, to_search_space_id(2), aggregation_level::n4, dci_dl_format::f1_0);

  // TEST: UE allocation should be successful and the PDCCH contents valid.
  TESTASSERT_EQ(1, res_grid[0].result.dl.dl_pdcchs.size());
  TESTASSERT(pdcch == &res_grid[0].result.dl.dl_pdcchs[0]);
  TESTASSERT_EQ(aggregation_level::n4, pdcch->ctx.cces.aggr_lvl);
  TESTASSERT_EQ(rnti, pdcch->ctx.rnti);
  TESTASSERT(pdcch->ctx.bwp_cfg == ue_cfg.dl_bwps[0]);
  TESTASSERT(pdcch->ctx.coreset_cfg == ue_cfg.dl_coresets[1]);

  // Action: Try allocate an DL PDCCH for another UE that fails due to lack of PDCCH resources.
  rnti  = to_rnti(0x4602);
  pdcch = pdcch_sch.alloc_dl_pdcch_ue(
      res_grid[0], rnti, ue_cfg, bwpid, to_search_space_id(2), aggregation_level::n4, dci_dl_format::f1_0);

  // TEST: RAR allocation should fail.
  TESTASSERT_EQ(1, res_grid[0].result.dl.dl_pdcchs.size());
  TESTASSERT(pdcch == nullptr);
}

void test_pdcch_sched_monitoring_period()
{
  test_delimit_logger delimiter{"Test PDCCH Monitoring Periodicity"};

  std::random_device rd;
  std::mt19937       rgen(rd());

  for (unsigned period : {20, 40}) {
    unsigned duration = 2;
    unsigned offset   = std::uniform_int_distribution<unsigned>{0, period - 1}(rgen);
    // Note: First slot is one slot before the PDCCH monitoring window.
    unsigned first_sl = (period * std::uniform_int_distribution<unsigned>{0, 100}(rgen) + offset - 1U) % 10240;

    sched_cell_configuration_request_message msg = make_default_sched_cell_configuration_request();
    msg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[1].monitoring_slot_period = period;
    msg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[1].monitoring_slot_offset = offset;
    msg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[1].duration               = duration;

    cell_configuration      cfg{msg};
    cell_resource_allocator res_grid{cfg};

    pdcch_scheduler_impl pdcch_sch(cfg);

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

int main()
{
  test_pdcch_sched_sib1();
  test_pdcch_sched_rar();
  test_pdcch_sched_ue();
  test_pdcch_sched_monitoring_period();
}
