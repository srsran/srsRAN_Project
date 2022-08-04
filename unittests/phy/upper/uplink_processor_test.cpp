/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/phy/upper/uplink_processor_single_executor_impl.h"
#include "../../support/task_executor_test_doubles.h"
#include "../support/prach_buffer_test_doubles.h"
#include "channel_processors/prach_detector_test_doubles.h"
#include "srsgnb/phy/support/prach_buffer_context.h"
#include "upper_phy_rx_results_notifier_test_doubles.h"
#include <gtest/gtest.h>

using namespace srsgnb;

namespace {
TEST(UplinkProcessor, calling_process_prach_enqueue_task)
{
  manual_task_worker_always_enqueue_tasks executor(10);
  auto                                    det      = std::make_unique<prach_detector_spy>();
  const prach_detector_spy&               detector = *det;
  upper_phy_rx_results_notifier_spy       results_notifier;
  uplink_processor_single_executor_impl   ul_processor(std::move(det), executor, results_notifier);

  prach_buffer_dummy buffer;
  ul_processor.process_prach(buffer, {}, {});

  ASSERT_FALSE(detector.has_detect_method_been_called());
  executor.run_pending_tasks();
  ASSERT_TRUE(detector.has_detect_method_been_called());
}

TEST(UplinkProcessor, after_detect_prach_is_executed_results_notifier_is_called)
{
  manual_task_worker_always_enqueue_tasks executor(10);
  auto                                    det = std::make_unique<prach_detector_spy>();
  upper_phy_rx_results_notifier_spy       results_notifier;
  uplink_processor_single_executor_impl   ul_processor(std::move(det), executor, results_notifier);

  prach_buffer_dummy buffer;
  ul_processor.process_prach(buffer, {}, {});

  ASSERT_FALSE(results_notifier.has_prach_result_been_notified());
  executor.run_pending_tasks();
  ASSERT_TRUE(results_notifier.has_prach_result_been_notified());
}

} // namespace
