/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/adt/blocking_queue.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(blocking_queue_test, blocking_push_from_main_thread_and_pop_from_another_thread)
{
  unsigned            qsize = test_rgen::uniform_int<unsigned>(1, 10000);
  blocking_queue<int> queue(qsize);

  std::atomic<int> count{0};
  std::thread      t([&queue, &count]() {
    while (true) {
      int val = queue.pop_blocking();
      if (queue.is_stopped()) {
        break;
      }
      ASSERT_EQ(val, count);
      count++;
    }
  });

  unsigned nof_objs = test_rgen::uniform_int<unsigned>(1, 100000);
  for (unsigned i = 0; i < nof_objs; ++i) {
    queue.push_blocking(i);
  }

  while ((unsigned)count != nof_objs) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  queue.stop();
  t.join();
}

TEST(blocking_queue_test, blocking_push_from_another_thread_and_pop_in_main_thread)
{
  std::thread t;

  unsigned            qsize = test_rgen::uniform_int<unsigned>(1, 10000);
  blocking_queue<int> queue(qsize);

  unsigned nof_objs = test_rgen::uniform_int<unsigned>(1, 100000);
  t                 = std::thread([&queue, nof_objs]() {
    int count = 0;
    while ((unsigned)count != nof_objs and queue.push_blocking(count++)) {
    }
  });

  for (unsigned i = 0; i < nof_objs; ++i) {
    ASSERT_EQ(queue.pop_blocking(), (int)i);
  }

  ASSERT_TRUE(queue.empty());

  queue.stop();
  t.join();
}

TEST(blocking_queue_test, blocking_push_and_pop_in_batches_in_separate_threads)
{
  std::thread t;

  std::vector<int> vec(test_rgen::uniform_int<unsigned>(1, 10000));
  for (unsigned i = 0; i != vec.size(); ++i) {
    vec[i] = i;
  }

  blocking_queue<int> queue(test_rgen::uniform_int<unsigned>(100, 1000));
  t = std::thread([&queue, &vec]() {
    for (unsigned i = 0; i < vec.size();) {
      unsigned batch_size = test_rgen::uniform_int<unsigned>(1, vec.size() - i);
      unsigned n          = queue.push_blocking(span<int>(vec).subspan(i, batch_size));
      EXPECT_LE(n, batch_size);
      i += n;
    }
  });

  std::vector<int> vec2(vec.size());
  for (unsigned i = 0; i < vec.size();) {
    unsigned batch_size = test_rgen::uniform_int<unsigned>(1, vec.size() - i);
    unsigned n          = queue.pop_blocking(vec2.begin() + i, vec2.begin() + i + batch_size);
    i += n;
  }

  ASSERT_EQ(vec2, vec);

  queue.stop();
  t.join();
}
