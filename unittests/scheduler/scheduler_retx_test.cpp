/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief In this file, we verify the correct operation of the MAC scheduler, as a whole, in case of DL/UL HARQ
/// retransmissions. The objective is to cover and verify the integration of the scheduler building blocks.

#include "test_utils/config_generators.h"
#include "test_utils/indication_generators.h"
#include "test_utils/result_test_helpers.h"
#include "test_utils/scheduler_test_bench.h"
#include <gtest/gtest.h>

using namespace srsgnb;

struct test_params {
  unsigned nof_retxs = 0;
};

class scheduler_retx_tester : public ::testing::TestWithParam<test_params>
{
protected:
  scheduler_retx_tester() { bench.add_cell(make_default_sched_cell_configuration_request()); }

  const ul_sched_info* run_until_next_pusch_alloc(unsigned max_slot_delay)
  {
    for (unsigned i = 0; i != max_slot_delay; ++i) {
      bench.run_slot();
      const ul_sched_info* grant = find_ue_pusch(ue_rnti, bench.last_sched_res->ul.puschs);
      if (grant != nullptr) {
        return grant;
      }
    }
    return nullptr;
  }

  void notify_crc_ind(bool is_msg3, unsigned harq_id, bool ack)
  {
    ul_crc_indication crc_ind;
    crc_ind.cell_index = to_du_cell_index(0);
    crc_ind.crcs.resize(1);
    crc_ind.crcs[0].rnti           = to_rnti(ue_rnti);
    crc_ind.crcs[0].ue_index       = is_msg3 ? INVALID_DU_UE_INDEX : to_du_ue_index(0);
    crc_ind.crcs[0].harq_id        = harq_id;
    crc_ind.crcs[0].tb_crc_success = ack;
    bench.sched->handle_crc_indication(crc_ind);
  }

  const rnti_t ue_rnti = to_rnti(0x4601);

  scheduler_test_bench bench;
};

TEST_P(scheduler_retx_tester, msg3_gets_retx_if_nacked)
{
  test_params  params          = GetParam();
  const size_t MAX_PUSCH_DELAY = 16, MAX_RETX_DELAY = 16;

  bench.sched->handle_rach_indication(
      create_rach_indication(bench.next_slot_rx(), {create_preamble(0, this->ue_rnti)}));

  const ul_sched_info* grant = run_until_next_pusch_alloc(MAX_PUSCH_DELAY);
  ASSERT_NE(grant, nullptr) << "No Msg3 was scheduled";
  ASSERT_EQ(grant->pusch_cfg.rnti, ue_rnti);
  ASSERT_TRUE(grant->pusch_cfg.new_data);
  const ul_sched_info msg3_newtx_grant = *grant;

  // Run until CRC slot.
  slot_point crc_slot = bench.last_result_slot();
  while (bench.next_slot_rx() != crc_slot) {
    bench.run_slot();
    ASSERT_EQ(find_ue_pusch(this->ue_rnti, *bench.last_sched_res), nullptr);
  }

  // Notify ACK/NACK.
  notify_crc_ind(true, msg3_newtx_grant.pusch_cfg.harq_id, params.nof_retxs == 0);

  for (unsigned nof_retx_count = 0; nof_retx_count != params.nof_retxs; ++nof_retx_count) {
    // Ensure Msg3 retx is scheduled.
    grant = run_until_next_pusch_alloc(MAX_RETX_DELAY);
    ASSERT_NE(grant, nullptr) << "No Msg3 retx was scheduled";
    ASSERT_EQ(grant->pusch_cfg.rnti, ue_rnti);
    ASSERT_FALSE(grant->pusch_cfg.new_data);
    ASSERT_EQ(grant->pusch_cfg.harq_id, msg3_newtx_grant.pusch_cfg.harq_id);

    // Run until CRC slot.
    crc_slot = bench.last_result_slot();
    while (bench.next_slot_rx() != crc_slot) {
      bench.run_slot();
      ASSERT_EQ(find_ue_pusch(this->ue_rnti, *bench.last_sched_res), nullptr);
    }

    // Notify ACK/NACK.
    notify_crc_ind(true, 0, params.nof_retxs - nof_retx_count - 1 == 0);
  }

  // No Msg3 retx should be scheduled after the HARQ is ACKed.
  grant = run_until_next_pusch_alloc(MAX_RETX_DELAY);
  ASSERT_EQ(grant, nullptr) << "Msg3 HARQ should be empty";
}

INSTANTIATE_TEST_SUITE_P(msg3_retx,
                         scheduler_retx_tester,
                         testing::Values(test_params{0}, test_params{1}, test_params{2}, test_params{3}));

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
