/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/logging/scheduler_result_logger.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "lib/scheduler/scheduler_impl.h"
#include "lib/scheduler/ue_scheduling/ue_cell_grid_allocator.h"
#include "lib/scheduler/ue_scheduling/ue_srb0_scheduler.h"
#include "test_utils/dummy_test_components.h"
#include "unittests/scheduler/test_utils/config_generators.h"
#include "unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "srsgnb/ran/duplex_mode.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>
#include <unordered_map>

using namespace srsgnb;

using dl_bsr_lc_report_list = static_vector<dl_buffer_state_indication_message, MAX_NOF_RB_LCIDS>;

struct sched_test_ue {
  rnti_t                 crnti;
  ul_bsr_lcg_report_list ul_bsr_list;
  dl_bsr_lc_report_list  dl_bsr_list;
};

unsigned allocate_rnti()
{
  // Randomly chosen RNTI start.
  static unsigned rnti_start = 0x4601;
  return rnti_start++;
}

// Parameters to be passed to test.
struct multiple_ue_test_params {
  uint8_t     k0;
  uint8_t     k1;
  uint16_t    nof_ues;
  uint16_t    min_buffer_size_in_bytes;
  uint16_t    max_buffer_size_in_bytes;
  duplex_mode duplx_mode;
};

// Helper class to initialize and store relevant objects for the test and provide helper methods.
struct test_bench {
  // Maximum number of slots to run per UE in order to validate the results of scheduler. Implementation defined.
  static constexpr unsigned max_test_run_slots_per_ue = 80;

  scheduler_expert_config                          expert_cfg;
  cell_configuration                               cell_cfg;
  sched_cfg_dummy_notifier                         cfg_notif;
  scheduler_ue_metrics_dummy_notifier              metric_notif;
  scheduler_impl                                   sch;
  std::unordered_map<du_ue_index_t, sched_test_ue> ues;
  const sched_result*                              sched_res{nullptr};

  explicit test_bench(const scheduler_expert_config&                  expert_cfg_,
                      const sched_cell_configuration_request_message& cell_req =
                          test_helpers::make_default_sched_cell_configuration_request()) :
    expert_cfg{expert_cfg_}, cell_cfg{cell_req}, sch{scheduler_config{expert_cfg, cfg_notif, metric_notif}}
  {
    sch.handle_cell_configuration_request(cell_req);
  }
};

class multiple_ue_sched_tester : public ::testing::TestWithParam<multiple_ue_test_params>
{
protected:
  slot_point              next_slot{0, 0};
  srslog::basic_logger&   mac_logger  = srslog::fetch_basic_logger("MAC", true);
  srslog::basic_logger&   test_logger = srslog::fetch_basic_logger("TEST", true);
  optional<test_bench>    bench;
  multiple_ue_test_params params;
  // We use this value to account for the case when the PDSCH or PUSCH is allocated several slots in advance.
  unsigned max_k_value = 0;

  multiple_ue_sched_tester() : params{GetParam()} {};

  ~multiple_ue_sched_tester() override
  {
    // Log pending allocations before finishing test.
    for (unsigned i = 0; i != max_k_value; ++i) {
      run_slot();
    }
    srslog::flush();
  }

  void setup_sched(const scheduler_expert_config&                  expert_cfg,
                   const sched_cell_configuration_request_message& msg =
                       test_helpers::make_default_sched_cell_configuration_request())
  {
    const auto& dl_lst = msg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
    for (const auto& pdsch : dl_lst) {
      if (pdsch.k0 > max_k_value) {
        max_k_value = pdsch.k0;
      }
    }
    const auto& ul_lst = msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
    for (const auto& pusch : ul_lst) {
      if (pusch.k2 > max_k_value) {
        max_k_value = pusch.k2;
      }
    }

    bench.emplace(expert_cfg, msg);

    // Initialize.
    mac_logger.set_context(next_slot.sfn(), next_slot.slot_index());
    test_logger.set_context(next_slot.sfn(), next_slot.slot_index());
    bench->sched_res = bench->sch.slot_indication(next_slot, to_du_cell_index(0));
  }

  void run_slot()
  {
    next_slot++;

    mac_logger.set_context(next_slot.sfn(), next_slot.slot_index());
    test_logger.set_context(next_slot.sfn(), next_slot.slot_index());

    bench->sched_res = bench->sch.slot_indication(next_slot, to_du_cell_index(0));

    // Check sched result consistency.
    test_scheduler_result_consistency(bench->cell_cfg, *bench->sched_res);
  }

  static scheduler_expert_config create_expert_config(sch_mcs_index max_msg4_mcs_index)
  {
    auto cfg            = config_helpers::make_default_scheduler_expert_config();
    cfg.ue.max_msg4_mcs = max_msg4_mcs_index;
    return cfg;
  }

  sched_cell_configuration_request_message create_random_cell_config_request() const
  {
    sched_cell_configuration_request_message msg = test_helpers::make_default_sched_cell_configuration_request();
    msg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[0].k0 = params.k0;

    if (params.duplx_mode == duplex_mode::TDD) {
      msg.tdd_ul_dl_cfg_common = config_helpers::make_default_tdd_ul_dl_config_common();
    }
    return msg;
  }

  unsigned pdsch_tbs_scheduled_bytes_per_lc(const sched_test_ue& u, lcid_t lcid)
  {
    unsigned total_cw_tb_size_bytes = 0;
    for (const auto& grant : bench->sched_res->dl.ue_grants) {
      if (grant.pdsch_cfg.rnti != u.crnti) {
        continue;
      }
      for (const auto& tb : grant.tb_list) {
        for (const auto& s_pdu : tb.lc_chs_to_sched) {
          if (s_pdu.lcid == lcid) {
            total_cw_tb_size_bytes += s_pdu.sched_bytes;
          }
        }
      }
    }
    return total_cw_tb_size_bytes;
  }

  unsigned pusch_tbs_scheduled_bytes(const sched_test_ue& u)
  {
    unsigned total_cw_tb_size_bytes = 0;
    for (const auto& grant : bench->sched_res->ul.puschs) {
      if (grant.pusch_cfg.rnti != u.crnti) {
        continue;
      }
      total_cw_tb_size_bytes += grant.pusch_cfg.tb_size_bytes;
    }
    return total_cw_tb_size_bytes;
  }

  void add_ue(du_ue_index_t ue_index, lcid_t lcid_, lcg_id_t lcgid_)
  {
    auto ue_creation_req     = test_helpers::create_default_sched_ue_creation_request();
    ue_creation_req.ue_index = ue_index;
    ue_creation_req.crnti    = to_rnti(allocate_rnti());
    ue_creation_req.cfg.cells[0].serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.value().dl_data_to_ul_ack[0] =
        params.k1;

    auto it = std::find_if(ue_creation_req.cfg.lc_config_list.begin(),
                           ue_creation_req.cfg.lc_config_list.end(),
                           [lcid_](const auto& l) { return l.lcid == lcid_; });
    if (it == ue_creation_req.cfg.lc_config_list.end()) {
      ue_creation_req.cfg.lc_config_list.push_back(config_helpers::create_default_logical_channel_config(lcid_));
      it = ue_creation_req.cfg.lc_config_list.end() - 1;
    }
    it->lc_group = lcgid_;

    bench->sch.handle_ue_creation_request(ue_creation_req);

    bench->ues[ue_index] = sched_test_ue{ue_creation_req.crnti, {}};
  }

  sched_test_ue& get_ue(du_ue_index_t ue_idx) { return bench->ues.at(ue_idx); }

  void push_buffer_state_to_dl_ue(du_ue_index_t ue_index, unsigned buffer_size, lcid_t lcid)
  {
    auto& test_ue = get_ue(ue_index);
    // Notification from upper layers of DL buffer state.
    dl_buffer_state_indication_message msg{ue_index, test_ue.crnti, lcid, buffer_size};

    // Store to keep track of DL buffer status.
    test_ue.dl_bsr_list.push_back(msg);

    bench->sch.handle_dl_buffer_state_indication(msg);
  }

  void notify_ul_bsr_from_ue(du_ue_index_t ue_index, unsigned buffer_size, lcg_id_t lcg_id)
  {
    auto& test_ue = get_ue(ue_index);
    // Assumptions:
    // - Only one LCG is assumed to have data to send.
    // - BSR is Short BSR.
    ul_bsr_indication_message msg{to_du_cell_index(0), ue_index, test_ue.crnti, bsr_format::SHORT_BSR, {}};
    msg.reported_lcgs.push_back(ul_bsr_lcg_report{lcg_id, buffer_size});

    // Store to keep track of current buffer status in UE.
    test_ue.ul_bsr_list.push_back(ul_bsr_lcg_report{lcg_id, buffer_size});

    bench->sch.handle_ul_bsr_indication(msg);
  }

  bool ue_is_allocated_pdsch(const sched_test_ue& u)
  {
    return std::any_of(bench->sched_res->dl.ue_grants.begin(),
                       bench->sched_res->dl.ue_grants.end(),
                       [&u](const auto& grant) { return grant.pdsch_cfg.rnti == u.crnti; });
  }

  bool ue_is_allocated_pusch(const sched_test_ue& u)
  {
    return std::any_of(bench->sched_res->ul.puschs.begin(), bench->sched_res->ul.puschs.end(), [&u](const auto& grant) {
      return grant.pusch_cfg.rnti == u.crnti;
    });
  }
};

TEST_P(multiple_ue_sched_tester, dl_buffer_state_indication_test)
{
  // Used to track BSR 0.
  std::map<unsigned, bool> is_bsr_zero_sent;

  setup_sched(create_expert_config(10), create_random_cell_config_request());
  // Add UE(s) and notify to each UE a DL buffer status indication of random size between min and max defined in params.
  // Assumption: LCID is DRB1.
  for (unsigned idx = 0; idx < params.nof_ues; idx++) {
    // Initialize.
    is_bsr_zero_sent[idx] = false;

    add_ue(to_du_ue_index(idx), LCID_MIN_DRB, static_cast<lcg_id_t>(0));

    push_buffer_state_to_dl_ue(
        to_du_ue_index(idx),
        test_rgen::uniform_int<unsigned>(params.min_buffer_size_in_bytes, params.max_buffer_size_in_bytes),
        LCID_MIN_DRB);
  }

  for (unsigned i = 0; i != params.nof_ues * test_bench::max_test_run_slots_per_ue; ++i) {
    run_slot();
    for (unsigned idx = 0; idx < params.nof_ues; idx++) {
      auto& test_ue = get_ue(to_du_ue_index(idx));
      if (is_bsr_zero_sent[idx]) {
        ASSERT_FALSE(ue_is_allocated_pdsch(test_ue))
            << fmt::format("Condition failed for UE with CRNTI=0x{:x}", test_ue.crnti);
        continue;
      }

      const unsigned tbs_sched_bytes = pdsch_tbs_scheduled_bytes_per_lc(test_ue, LCID_MIN_DRB);
      if (tbs_sched_bytes == 0 && test_ue.dl_bsr_list.back().bs == 0 && not is_bsr_zero_sent[idx]) {
        is_bsr_zero_sent[idx] = true;
        // Notify buffer status of 0 to ensure scheduler does not schedule further for this UE.
        push_buffer_state_to_dl_ue(to_du_ue_index(idx), 0, LCID_MIN_DRB);
      }

      if (tbs_sched_bytes > test_ue.dl_bsr_list.back().bs) {
        // Accounting for MAC headers.
        test_ue.dl_bsr_list.back().bs = 0;
      } else {
        test_ue.dl_bsr_list.back().bs -= tbs_sched_bytes;
      }
    }
  }

  for (unsigned idx = 0; idx < params.nof_ues; idx++) {
    const auto& test_ue = get_ue(to_du_ue_index(idx));
    ASSERT_EQ(test_ue.dl_bsr_list.back().bs, 0);
  }
}

TEST_P(multiple_ue_sched_tester, ul_buffer_state_indication_test)
{
  // Used to track BSR 0.
  std::map<unsigned, bool> is_bsr_zero_sent;

  setup_sched(create_expert_config(10), create_random_cell_config_request());
  // Add UE(s) and notify UL BSR from UE of random size between min and max defined in params.
  // Assumption: LCID is DRB1.
  for (unsigned idx = 0; idx < params.nof_ues; idx++) {
    // Initialize.
    is_bsr_zero_sent[idx] = false;

    add_ue(to_du_ue_index(idx), LCID_MIN_DRB, static_cast<lcg_id_t>(0));

    notify_ul_bsr_from_ue(
        to_du_ue_index(idx),
        test_rgen::uniform_int<unsigned>(params.min_buffer_size_in_bytes, params.max_buffer_size_in_bytes),
        static_cast<lcg_id_t>(0));
  }

  for (unsigned i = 0; i != params.nof_ues * test_bench::max_test_run_slots_per_ue; ++i) {
    run_slot();
    for (unsigned idx = 0; idx < params.nof_ues; idx++) {
      auto& test_ue = get_ue(to_du_ue_index(idx));

      if (is_bsr_zero_sent[idx]) {
        ASSERT_FALSE(ue_is_allocated_pusch(test_ue))
            << fmt::format("Condition failed for UE with CRNTI=0x{:x}", test_ue.crnti);
        continue;
      }

      const unsigned tbs_sched_bytes = pusch_tbs_scheduled_bytes(test_ue);
      if (tbs_sched_bytes == 0 && test_ue.ul_bsr_list.back().nof_bytes == 0 && not is_bsr_zero_sent[idx]) {
        is_bsr_zero_sent[idx] = true;
        // Notify BSR 0 to ensure scheduler does not schedule further for this UE.
        notify_ul_bsr_from_ue(to_du_ue_index(idx), 0, static_cast<lcg_id_t>(0));
      }

      if (tbs_sched_bytes > test_ue.ul_bsr_list.back().nof_bytes) {
        // Accounting for MAC headers.
        test_ue.ul_bsr_list.back().nof_bytes = 0;
      } else {
        test_ue.ul_bsr_list.back().nof_bytes -= tbs_sched_bytes;
      }
    }
  }

  for (unsigned idx = 0; idx < params.nof_ues; idx++) {
    const auto& test_ue = get_ue(to_du_ue_index(idx));
    ASSERT_EQ(test_ue.ul_bsr_list.back().nof_bytes, 0);
  }
}

TEST_P(multiple_ue_sched_tester, not_scheduled_when_buffer_status_zero)
{
  setup_sched(create_expert_config(10), create_random_cell_config_request());
  // Add UE(s) and notify UL BSR + DL Buffer status with zero value.
  // Assumption: LCID is DRB1.
  for (unsigned idx = 0; idx < params.nof_ues; idx++) {
    add_ue(to_du_ue_index(idx), LCID_MIN_DRB, static_cast<lcg_id_t>(0));

    notify_ul_bsr_from_ue(to_du_ue_index(idx), 0, static_cast<lcg_id_t>(0));
    push_buffer_state_to_dl_ue(to_du_ue_index(idx), 0, LCID_MIN_DRB);
  }

  for (unsigned i = 0; i != params.nof_ues * test_bench::max_test_run_slots_per_ue; ++i) {
    run_slot();
    for (unsigned idx = 0; idx < params.nof_ues; idx++) {
      auto& test_ue = get_ue(to_du_ue_index(idx));
      ASSERT_FALSE(ue_is_allocated_pusch(test_ue));
      ASSERT_FALSE(ue_is_allocated_pdsch(test_ue));
    }
  }
}

INSTANTIATE_TEST_SUITE_P(multiple_ue_sched_tester,
                         multiple_ue_sched_tester,
                         testing::Values(multiple_ue_test_params{.k0                       = 1,
                                                                 .k1                       = 4,
                                                                 .nof_ues                  = 3,
                                                                 .min_buffer_size_in_bytes = 1000,
                                                                 .max_buffer_size_in_bytes = 3000,
                                                                 .duplx_mode               = duplex_mode::FDD},
                                         multiple_ue_test_params{.k0                       = 2,
                                                                 .k1                       = 5,
                                                                 .nof_ues                  = 3,
                                                                 .min_buffer_size_in_bytes = 1000,
                                                                 .max_buffer_size_in_bytes = 3000,
                                                                 .duplx_mode               = duplex_mode::FDD},
                                         multiple_ue_test_params{.k0                       = 15,
                                                                 .k1                       = 4,
                                                                 .nof_ues                  = 5,
                                                                 .min_buffer_size_in_bytes = 2000,
                                                                 .max_buffer_size_in_bytes = 3000,
                                                                 .duplx_mode               = duplex_mode::TDD},
                                         multiple_ue_test_params{.k0                       = 2,
                                                                 .k1                       = 5,
                                                                 .nof_ues                  = 2,
                                                                 .min_buffer_size_in_bytes = 1000,
                                                                 .max_buffer_size_in_bytes = 3000,
                                                                 .duplx_mode               = duplex_mode::TDD}));

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("MAC", true).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
