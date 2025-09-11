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

#include "srsran/support/executors/inline_task_executor.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/io/io_broker_factory.h"
#include "srsran/support/io/io_timer_source.h"
#include "srsran/support/timers.h"
#include <gtest/gtest.h>
#include <optional>

using namespace srsran;

class io_timer_source_test : public ::testing::Test
{
public:
  manual_task_worker         worker{16};
  timer_manager              timers{16};
  std::unique_ptr<io_broker> broker = create_io_broker(io_broker_type::epoll);

  ~io_timer_source_test() override { stop(); }

  void start() { source.emplace(timers, *broker, worker, std::chrono::milliseconds{1}); }

  void stop()
  {
    // We need to stop from a different thread than the one ticking.
    std::atomic<bool> closed{false};
    task_worker       stopper_helper{"STOP_WORKER", 128};
    stopper_helper.push_task_blocking([this, &closed]() {
      source.reset();
      closed = true;
    });

    while (not closed) {
      worker.run_pending_tasks();
      std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
  }

  std::optional<io_timer_source> source;
};

TEST_F(io_timer_source_test, timer_gets_ticked_when_source_starts)
{
  std::chrono::milliseconds run_duration{100};
  std::chrono::milliseconds timer_period{5};

  unique_timer          t = timers.create_unique_timer(worker);
  std::atomic<unsigned> count{0};
  t.set(timer_period, [&count, &t](timer_id_t tid) {
    count++;
    t.run();
  });
  t.run();

  // The io timer source is not yet running.
  std::this_thread::sleep_for(std::chrono::milliseconds{10});
  worker.run_pending_tasks();
  ASSERT_EQ(count, 0);

  start();
  for (unsigned i = 0; i != run_duration.count(); i++) {
    std::this_thread::sleep_for(std::chrono::milliseconds{1});
    worker.run_pending_tasks();
  }
  ASSERT_GT(count, 1);
  fmt::print("Tick count: expected={} actual={}\n", run_duration / timer_period, count.load());

  // Stop io timer source.
  stop();
  worker.run_pending_tasks(); // ensures no pending tasks before we reset count

  count = 0;
  std::this_thread::sleep_for(std::chrono::milliseconds{10});
  worker.run_pending_tasks();
  ASSERT_EQ(count, 0);
}
