/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/mac/mac_dl/mac_cell_processor.h"
#include "mac_test_helpers.h"
#include "srsgnb/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;

struct test_params {
  unsigned nof_sib_allocated;
  unsigned nof_rar_allocated;
  unsigned nof_ue_allocated;
};

class mac_cell_processor_tester : public ::testing::TestWithParam<test_params>
{
protected:
  mac_cell_processor_tester() :
    ue_mng(rnti_table),
    mac_cell(test_helpers::make_default_mac_cell_config(),
             sched,
             ue_mng,
             phy_notifier,
             task_worker,
             task_worker,
             task_worker,
             pcap)
  {
  }

  test_helpers::dummy_mac_scheduler            sched;
  du_rnti_table                                rnti_table;
  mac_dl_ue_manager                            ue_mng;
  test_helpers::dummy_mac_cell_result_notifier phy_notifier;
  manual_task_worker                           task_worker{128};
  test_helpers::dummy_mac_pcap                 pcap;
  mac_cell_processor                           mac_cell;

  bool is_pdsch_scheduled() const
  {
    const auto& dl_res = *phy_notifier.last_sched_res->dl_res;
    return not dl_res.rar_grants.empty() or not dl_res.bc.sibs.empty() or not dl_res.ue_grants.empty();
  }

  void init(test_params params)
  {
    // Creates the next sched result based on test parameters.
    sched.next_sched_result.dl.bc.sibs.resize(params.nof_sib_allocated);
    for (auto& sib_grant : sched.next_sched_result.dl.bc.sibs) {
      sib_grant.pdsch_cfg.rnti = SI_RNTI;
      sib_grant.pdsch_cfg.codewords.resize(1);
      sib_grant.pdsch_cfg.codewords[0].tb_size_bytes = 128;
    }
    sched.next_sched_result.dl.rar_grants.resize(params.nof_rar_allocated);
    for (auto& rar_grant : sched.next_sched_result.dl.rar_grants) {
      rar_grant.pdsch_cfg.rnti = to_rnti(0x1);
      rar_grant.pdsch_cfg.codewords.resize(1);
      rar_grant.pdsch_cfg.codewords[0].tb_size_bytes = 128;
      rar_grant.grants.resize(1);
    }
    sched.next_sched_result.dl.ue_grants.resize(params.nof_ue_allocated);
    for (auto& ue_grant : sched.next_sched_result.dl.ue_grants) {
      ue_grant.pdsch_cfg.rnti = to_rnti(0x4601);
      ue_grant.pdsch_cfg.codewords.resize(1);
      ue_grant.pdsch_cfg.codewords[0].tb_size_bytes = 128;
    }
  }
};

TEST_P(mac_cell_processor_tester, when_zero_pdsch_grants_dl_data_request_is_not_notified)
{
  init(GetParam());
  slot_point sl_tx{0, 0};
  ASSERT_FALSE(phy_notifier.is_complete);
  mac_cell.handle_slot_indication(sl_tx);

  ASSERT_TRUE(phy_notifier.last_sched_res.has_value());
  ASSERT_EQ(phy_notifier.last_dl_data_res.has_value(), is_pdsch_scheduled());
  ASSERT_TRUE(phy_notifier.is_complete);
}

INSTANTIATE_TEST_SUITE_P(
    nof_pdschs_allocated,
    mac_cell_processor_tester,
    testing::Values(test_params{0, 0, 0}, test_params{1, 0, 0}, test_params{0, 1, 0}, test_params{0, 0, 1}));
