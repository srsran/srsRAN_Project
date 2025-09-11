/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "lib/mac/mac_dl/mac_cell_processor.h"
#include "mac_test_helpers.h"
#include "srsran/mac/mac_cell_timing_context.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;

class base_mac_cell_processor_test
{
public:
  base_mac_cell_processor_test() :
    mac_cell(test_helpers::make_default_mac_cell_config(builder_params),
             sched_adapter,
             rnti_table,
             phy_notifier,
             task_worker,
             task_worker,
             task_worker,
             pcap,
             timers,
             {timer_ctrl.add_cell(to_du_cell_index(0))})
  {
    next_slot = {to_numerology_value(builder_params.scs_common), 0};
  }

  bool is_pdsch_scheduled() const
  {
    const auto& dl_res = *phy_notifier.last_sched_res->dl_res;
    return not dl_res.rar_grants.empty() or not dl_res.bc.sibs.empty() or not dl_res.ue_grants.empty() or
           not dl_res.paging_grants.empty();
  }

  test_helpers::dummy_mac_scheduler_adapter    sched_adapter;
  du_rnti_table                                rnti_table;
  test_helpers::dummy_mac_cell_result_notifier phy_notifier;
  manual_task_worker                           task_worker{128};
  null_mac_pcap                                pcap;
  timer_manager                                timers;
  test_helpers::dummy_mac_clock_controller     timer_ctrl{timers};
  cell_config_builder_params                   builder_params;
  mac_cell_processor                           mac_cell;

  slot_point next_slot;
};

struct test_params {
  unsigned nof_sib_allocated;
  unsigned nof_rar_allocated;
  unsigned nof_ue_allocated;
};

class mac_cell_processor_tester : public base_mac_cell_processor_test, public ::testing::TestWithParam<test_params>
{
protected:
  mac_cell_processor_tester()
  {
    auto params = GetParam();

    // Creates the next sched result based on test parameters.
    sched_adapter.next_sched_result.success           = true;
    sched_adapter.next_sched_result.dl.nof_dl_symbols = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
    sched_adapter.next_sched_result.dl.bc.sibs.resize(params.nof_sib_allocated);
    for (auto& sib_grant : sched_adapter.next_sched_result.dl.bc.sibs) {
      sib_grant.version        = 0;
      sib_grant.pdsch_cfg.rnti = rnti_t::SI_RNTI;
      sib_grant.pdsch_cfg.codewords.resize(1);
      sib_grant.pdsch_cfg.codewords[0].tb_size_bytes = 128;
    }
    sched_adapter.next_sched_result.dl.rar_grants.resize(params.nof_rar_allocated);
    for (auto& rar_grant : sched_adapter.next_sched_result.dl.rar_grants) {
      rar_grant.pdsch_cfg.rnti = to_rnti(0x1);
      rar_grant.pdsch_cfg.codewords.resize(1);
      rar_grant.pdsch_cfg.codewords[0].tb_size_bytes = 128;
      rar_grant.grants.resize(1);
    }
    sched_adapter.next_sched_result.dl.ue_grants.resize(params.nof_ue_allocated);
    for (auto& ue_grant : sched_adapter.next_sched_result.dl.ue_grants) {
      ue_grant.pdsch_cfg.rnti = to_rnti(0x4601);
      ue_grant.pdsch_cfg.codewords.resize(1);
      ue_grant.pdsch_cfg.codewords[0].tb_size_bytes = 128;
    }

    // Start cell.
    auto                     t = mac_cell.start();
    lazy_task_launcher<void> launcher{t};
    this->mac_cell.handle_slot_indication(mac_cell_timing_context{next_slot++});
    task_worker.run_pending_tasks();
    report_error_if_not(launcher.ready(), "Unable to start cell");
    phy_notifier = {};
  }
};

TEST_P(mac_cell_processor_tester, when_cell_is_active_then_slot_indication_triggers_scheduling)
{
  // Scheduler was notified of cell start.
  ASSERT_TRUE(sched_adapter.active);

  auto slot_tp = std::chrono::system_clock::now();
  ASSERT_FALSE(phy_notifier.is_complete);
  mac_cell.handle_slot_indication({next_slot++, slot_tp});

  ASSERT_TRUE(phy_notifier.last_sched_res.has_value());
  ASSERT_EQ(phy_notifier.last_dl_data_res.has_value(), is_pdsch_scheduled());
  ASSERT_TRUE(phy_notifier.is_complete);

  auto test_params = GetParam();
  ASSERT_EQ(phy_notifier.last_sched_res->dl_res->bc.sibs.size(), test_params.nof_sib_allocated);
  ASSERT_EQ(phy_notifier.last_sched_res->dl_res->rar_grants.size(), test_params.nof_rar_allocated);
  ASSERT_EQ(phy_notifier.last_sched_res->dl_res->ue_grants.size(), test_params.nof_ue_allocated);
}

TEST_P(mac_cell_processor_tester, when_cell_is_deactivated_then_scheduler_gets_notified)
{
  auto slot_tp = std::chrono::system_clock::now();

  ASSERT_FALSE(phy_notifier.is_complete);

  async_task<void>         t = mac_cell.stop();
  lazy_task_launcher<void> launcher{t};
  this->task_worker.run_pending_tasks();
  ASSERT_TRUE(t.ready());

  // Scheduler was notified of cell stop.
  ASSERT_FALSE(sched_adapter.active);

  // Cell continues to process slot indications.
  phy_notifier.is_complete = false;
  phy_notifier.last_sched_res.reset();
  phy_notifier.last_ul_res.reset();
  phy_notifier.last_dl_data_res.reset();
  mac_cell.handle_slot_indication({next_slot++, slot_tp});
  ASSERT_TRUE(phy_notifier.is_complete);
}

INSTANTIATE_TEST_SUITE_P(
    nof_pdschs_allocated,
    mac_cell_processor_tester,
    testing::Values(test_params{0, 0, 0}, test_params{1, 0, 0}, test_params{0, 1, 0}, test_params{0, 0, 1}));

class mac_cell_processor_ue_update_test : public base_mac_cell_processor_test, public ::testing::Test
{
protected:
  mac_cell_processor_ue_update_test()
  {
    // Creates the next sched result based on test parameters.
    sched_adapter.next_sched_result.success = true;

    // Start cell.
    auto                     t = mac_cell.start();
    lazy_task_launcher<void> launcher{t};
    this->mac_cell.handle_slot_indication(mac_cell_timing_context{next_slot++});
    task_worker.run_pending_tasks();
    report_error_if_not(launcher.ready(), "Unable to start cell");
  }

  mac_ue_create_request make_ue_create_request(du_ue_index_t ue_index)
  {
    mac_ue_create_request request;
    request.cell_index   = to_du_cell_index(0);
    request.ue_index     = ue_index;
    request.crnti        = to_rnti(0x4601 + static_cast<unsigned>(ue_index));
    request.rlf_notifier = &rlf_notifier;
    request.bearers.resize(3);
    for (unsigned i = 0; i < request.bearers.size(); ++i) {
      request.bearers[i].lcid = uint_to_lcid(i);
    }
    request.ul_ccch_msg = &msg3_buffer;
    return request;
  }

  mac_ue_delete_request make_ue_delete_request(du_ue_index_t ue_index)
  {
    mac_ue_delete_request request;
    request.cell_index = to_du_cell_index(0);
    request.ue_index   = ue_index;
    request.rnti       = to_rnti(0x4601 + static_cast<unsigned>(ue_index));
    return request;
  }

  test_helpers::dummy_ue_rlf_notifier rlf_notifier;
  byte_buffer                         msg3_buffer = byte_buffer::create({0x1, 0x2, 0x3, 0x4, 0x5, 0x6}).value();
};

TEST_F(mac_cell_processor_ue_update_test, ues_created_and_removed_in_batches)
{
  unsigned nof_batches       = 1000;
  unsigned nof_ues_per_batch = 32;
  unsigned nof_ues_constant  = 10;

  for (unsigned uidx = 0; uidx != nof_ues_constant; ++uidx) {
    // Create a new UE.
    du_ue_index_t            ue_index = to_du_ue_index(uidx);
    auto                     t        = mac_cell.add_ue(make_ue_create_request(ue_index));
    lazy_task_launcher<bool> launcher{t};
    task_worker.run_pending_tasks();
    report_error_if_not(launcher.ready(), "Unable to add UE");
    ASSERT_TRUE(launcher.result.value());
  }

  for (unsigned batch = 0; batch != nof_batches; ++batch) {
    // Create a new batch of UEs.
    for (unsigned uidx = 0; uidx != nof_ues_per_batch; ++uidx) {
      // Create a new UE.
      du_ue_index_t            ue_index = to_du_ue_index(nof_ues_constant + uidx);
      mac_ue_create_request    request  = make_ue_create_request(ue_index);
      auto                     t        = mac_cell.add_ue(request);
      lazy_task_launcher<bool> launcher{t};
      task_worker.run_pending_tasks();
      report_error_if_not(launcher.ready(), "Unable to add UE");
      ASSERT_TRUE(launcher.result.value());
    }

    // Remove batch of UEs.
    for (unsigned uidx = 0; uidx != nof_ues_per_batch; ++uidx) {
      // Check that the UE was created.
      du_ue_index_t            ue_index = to_du_ue_index(nof_ues_constant + uidx);
      auto                     t        = mac_cell.remove_ue(make_ue_delete_request(ue_index));
      lazy_task_launcher<void> launcher{t};
      task_worker.run_pending_tasks();
      report_error_if_not(launcher.ready(), "Unable to add UE");
    }
  }
}
