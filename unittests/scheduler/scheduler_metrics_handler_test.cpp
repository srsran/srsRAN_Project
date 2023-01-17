/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/logging/scheduler_metrics_handler.h"
#include <gtest/gtest.h>

using namespace srsgnb;

TEST(scheduler_metrics_handler_test, metrics_sent_with_defined_periodicity)
{
  unsigned                             period_msec = 10;
  blocking_queue<scheduler_ue_metrics> metric_queue{16};
  scheduler_metrics_handler            metrics(std::chrono::milliseconds{period_msec}, &metric_queue);

  metrics.handle_ue_creation(to_du_ue_index(0), to_rnti(0x4601), pci_t{0});
  metrics.handle_dl_harq_ack(to_du_ue_index(0), true);

  sched_result sched_res;
  slot_point   sl_tx{0, 0};
  for (unsigned i = 0; i != period_msec; ++i) {
    ASSERT_TRUE(metric_queue.empty());
    metrics.push(sl_tx, sched_res);
    ++sl_tx;
  }
  ASSERT_FALSE(metric_queue.empty());

  // Check resulting metrics.
  scheduler_ue_metrics ue_metrics;
  ASSERT_TRUE(metric_queue.try_pop(ue_metrics));
  ASSERT_EQ(ue_metrics.rnti, to_rnti(0x4601));
  ASSERT_EQ(ue_metrics.dl_nof_ok, 1);
  ASSERT_EQ(ue_metrics.dl_nof_nok, 0);
  ASSERT_TRUE(metric_queue.empty());
}