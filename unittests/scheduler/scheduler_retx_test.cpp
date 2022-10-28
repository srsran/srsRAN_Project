/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "utils/config_generators.h"
#include "utils/indication_generators.h"
#include "utils/result_test_helpers.h"
#include "utils/scheduler_test_bench.h"
#include <gtest/gtest.h>

using namespace srsgnb;

class scheduler_retx_tester : public ::testing::Test
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

TEST_F(scheduler_retx_tester, msg3_gets_retx_if_nacked)
{
  bench.sched->handle_rach_indication(
      create_rach_indication(bench.next_slot_rx(), {create_preamble(0, this->ue_rnti)}));

  const size_t         MAX_PUSCH_DELAY = 32;
  const ul_sched_info* grant           = run_until_next_pusch_alloc(MAX_PUSCH_DELAY);
  ASSERT_NE(grant, nullptr) << "No Msg3 was scheduled";

  slot_point crc_slot = bench.last_result_slot();
  while (bench.next_slot_rx() != crc_slot) {
    bench.run_slot();
  }

  // Notify NACK.
  notify_crc_ind(true, 0, false);

  // Ensure retx is scheduled.
  const size_t MAX_RETX_DELAY = 32;
  grant                       = run_until_next_pusch_alloc(MAX_RETX_DELAY);
  ASSERT_NE(grant, nullptr) << "No Msg3 retx was scheduled";
}
