/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/ue_scheduling/harq_process.h"
#include "srsran/scheduler/scheduler_slot_handler.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

/// Tester for different combinations of max HARQ retxs, ack wait timeouts, and k1s.
class dl_harq_process_tester : public ::testing::Test
{
protected:
  dl_harq_process_tester(unsigned max_ack_timeout_slots = 16) :
    dl_logger(srslog::fetch_basic_logger("SCHED"), to_rnti(0x4601), to_du_cell_index(0), true),
    h_dl(to_harq_id(0), dl_logger, {}, max_ack_timeout_slots)
  {
    srslog::fetch_basic_logger("SCHED").set_level(srslog::basic_levels::debug);

    srslog::init();
  }

  harq_logger     dl_logger;
  dl_harq_process h_dl;
};

TEST_F(dl_harq_process_tester, harq_starts_empty)
{
  ASSERT_TRUE(this->h_dl.empty(0));
  ASSERT_TRUE(this->h_dl.empty());
  ASSERT_FALSE(this->h_dl.has_pending_retx());
  ASSERT_FALSE(this->h_dl.has_pending_retx(0));
}

TEST_F(dl_harq_process_tester, reset_of_empty_harq_is_no_op)
{
  h_dl.reset();
  ASSERT_TRUE(h_dl.empty());
  ASSERT_TRUE(h_dl.empty(0));
  ASSERT_FALSE(h_dl.has_pending_retx());
  ASSERT_FALSE(h_dl.has_pending_retx(0));
}

/// Tester for different combinations of max HARQ retxs, ack wait timeouts, and k1s.
class dl_harq_process_param_tester : public ::testing::TestWithParam<std::tuple<unsigned, unsigned, unsigned>>
{
protected:
  dl_harq_process_param_tester() :
    max_harq_retxs(std::get<0>(GetParam())),
    max_ack_wait_slots(std::get<1>(GetParam())),
    k1(std::get<2>(GetParam())),
    dl_logger(srslog::fetch_basic_logger("SCHED"), to_rnti(0x4601), to_du_cell_index(0), true),
    h_dl(to_harq_id(0), dl_logger, {}, max_ack_wait_slots)
  {
    srslog::init();
  }

  void slot_indication()
  {
    ++sl_tx;
    srslog::fetch_basic_logger("SCHED").set_context(sl_tx.sfn(), sl_tx.slot_index());
    h_dl.slot_indication(sl_tx);
  }

  ~dl_harq_process_param_tester() { srslog::flush(); }

  const unsigned max_harq_retxs;
  const unsigned max_ack_wait_slots;
  const unsigned k1;

  harq_logger     dl_logger;
  dl_harq_process h_dl;
  slot_point      sl_tx{0, 0};
};

INSTANTIATE_TEST_SUITE_P(dl_harq_param_combine,
                         dl_harq_process_param_tester,
                         testing::Combine(testing::Values(0, 1, 2, 4),   // max_retxs
                                          testing::Values(2, 4, 6, 8),   // max_ack_wait_slots
                                          testing::Values(1, 2, 4, 6))); // k1
