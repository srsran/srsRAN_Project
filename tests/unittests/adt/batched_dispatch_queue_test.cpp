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

#include "srsran/adt/batched_dispatch_queue.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;

class base_batched_dispatch_queue_test
{
protected:
  base_batched_dispatch_queue_test(unsigned qsize, const std::function<void(span<const int>)>& func) :
    worker(qsize), queue(qsize, worker, logger, func)
  {
    logger.set_level(srslog::basic_levels::debug);
    srslog::init();
  }

  srslog::basic_logger&       logger = srslog::fetch_basic_logger("ALL");
  manual_task_worker          worker;
  batched_dispatch_queue<int> queue;
};

class batched_dispatch_queue_test : public base_batched_dispatch_queue_test, public ::testing::Test
{
public:
  batched_dispatch_queue_test() :
    base_batched_dispatch_queue_test(16, [this](span<const int> values) {
      popped_values.insert(popped_values.end(), values.begin(), values.end());
    })
  {
  }

  std::vector<int> popped_values;
};

TEST_F(batched_dispatch_queue_test, when_first_event_is_enqueued_then_it_dispatches_task)
{
  ASSERT_EQ(queue.size_approx(), 0);
  ASSERT_TRUE(queue.try_push(5));
  ASSERT_EQ(queue.size_approx(), 1);

  ASSERT_TRUE(popped_values.empty());
  worker.run_pending_tasks();
  ASSERT_EQ(popped_values.size(), 1);
  ASSERT_EQ(popped_values[0], 5);
}

TEST_F(batched_dispatch_queue_test,
       when_several_values_are_enqueued_then_one_task_is_dispatched_then_handles_all_enqueued_values)
{
  ASSERT_EQ(queue.size_approx(), 0);
  ASSERT_TRUE(queue.try_push(5));
  ASSERT_TRUE(queue.try_push(6));
  ASSERT_TRUE(queue.try_push(7));
  ASSERT_EQ(queue.size_approx(), 3);

  ASSERT_TRUE(popped_values.empty());
  worker.run_next_blocking();
  ASSERT_EQ(popped_values.size(), 3);
  ASSERT_EQ(popped_values[0], 5);
  ASSERT_EQ(popped_values[1], 6);
  ASSERT_EQ(popped_values[2], 7);
}

TEST_F(batched_dispatch_queue_test, when_queue_is_full_then_push_fails)
{
  for (unsigned i = 0; i != 16; ++i) {
    ASSERT_TRUE(queue.try_push(i));
  }
  ASSERT_FALSE(queue.try_push(16));
}

TEST_F(batched_dispatch_queue_test, when_queue_is_stopped_then_previously_enqueued_values_are_not_handled)
{
  ASSERT_TRUE(queue.try_push(5));
  ASSERT_TRUE(queue.try_push(6));
  queue.stop();

  ASSERT_TRUE(popped_values.empty());
  worker.run_pending_tasks();
  ASSERT_TRUE(popped_values.empty());
}
