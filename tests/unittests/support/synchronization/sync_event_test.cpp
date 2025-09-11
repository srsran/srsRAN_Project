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

#include "srsran/support/synchronization/sync_event.h"
#include <gtest/gtest.h>
#include <thread>

using namespace srsran;

class sync_event_test : public ::testing::Test
{
protected:
  std::vector<std::unique_ptr<std::thread>> workers;
  sync_event                                ev;
};

TEST_F(sync_event_test, wait_with_no_tokens_is_noop)
{
  ev.wait();
}

TEST_F(sync_event_test, wait_blocks_until_token_is_reset)
{
  auto token = ev.get_token();

  // Call wait from another thread to avoid deadlock.
  std::atomic<bool> finished{false};
  workers.push_back(std::make_unique<std::thread>([this, &finished]() {
    ev.wait();
    finished = true;
  }));

  // Wait is blocked.
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  ASSERT_FALSE(finished);

  // Now wait() will unlock.
  token.reset();
  workers[0]->join();
  ASSERT_TRUE(finished);
}

TEST_F(sync_event_test, wait_blocks_until_multiple_tokens_are_reset)
{
  std::atomic<int> count{0};
  int              nof_tokens = 10;
  for (int i = 0; i < nof_tokens; i++) {
    workers.push_back(std::make_unique<std::thread>([&count, token = ev.get_token()]() { count++; }));
  }

  // Synchronization point: wait until all workers have started and reset their token.
  ev.wait();
  ASSERT_EQ(count, nof_tokens);

  for (int i = 0; i < nof_tokens; i++) {
    workers[i]->join();
  }
}

TEST_F(sync_event_test, multiple_wait_block_until_all_tokens_are_reset)
{
  std::vector<scoped_sync_token> tokens;
  const int                      nof_tokens  = 10;
  const int                      nof_waiters = 5;

  for (int i = 0; i < nof_tokens; i++) {
    tokens.emplace_back(ev.get_token());
  }
  std::vector<std::atomic<bool>> finished(nof_waiters);
  for (int i = 0; i < nof_waiters; i++) {
    workers.push_back(std::make_unique<std::thread>([this, &finished, i]() {
      ev.wait();
      finished[i] = true;
    }));
  }

  for (int i = 0; i != nof_waiters; i++) {
    ASSERT_FALSE(finished[i]) << "No waiter should have finished yet";
  }

  // All except one observer are reset.
  for (int i = 0; i < nof_tokens - 1; i++) {
    tokens[i].reset();
  }
  for (int i = 0; i != nof_waiters; i++) {
    ASSERT_FALSE(finished[i]) << "No stopper should have finished yet";
  }

  // All stoppers should unlock now.
  tokens[nof_tokens - 1].reset();
  for (int i = 0; i != nof_waiters; i++) {
    workers[i]->join();
    ASSERT_TRUE(finished[i]) << "All stoppers should have finished now";
  }
}

TEST(sync_event_dtor_test, event_concurrent_destruction)
{
  std::optional<sync_event> ev;
  ev.emplace();

  std::thread worker([token = ev->get_token()]() {});

  // Ensure no race condition in dtor.
  ev.reset();

  // Reuse memory.
  ev.emplace();
  auto token = ev->get_token();

  worker.join();
}
