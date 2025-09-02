/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  // synchronization point: wait until all workers have started and acquired a token.
  ev.wait();
  ASSERT_EQ(count, nof_tokens);

  for (int i = 0; i < nof_tokens; i++) {
    workers[i]->join();
  }
}
