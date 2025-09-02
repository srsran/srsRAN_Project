/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/synchronization/stop_event.h"
#include <gtest/gtest.h>
#include <thread>

using namespace srsran;

class stop_event_test : public ::testing::Test
{
protected:
  std::vector<std::unique_ptr<std::thread>> workers;
  stop_event_source                         ev;
};

TEST_F(stop_event_test, no_observers_stop_is_noop)
{
  ev.stop();
}

TEST_F(stop_event_test, stop_blocks_until_observer_is_reset)
{
  auto obs = ev.get_token();

  ASSERT_FALSE(obs.stop_requested());

  // Call stop from another thread to avoid deadlock.
  std::atomic<bool> finished{false};
  workers.push_back(std::make_unique<std::thread>([this, &finished]() {
    ev.stop();
    finished = true;
  }));

  // Stop was requested, but event source did not unlock.
  while (not obs.stop_requested()) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
  ASSERT_FALSE(finished);

  // Now stop() will unlock.
  obs.reset();
  workers[0]->join();
  ASSERT_TRUE(finished);
}

TEST_F(stop_event_test, multiple_stop_block_until_all_observers_are_reset)
{
  std::vector<stop_event_token> observers;
  const int                     num_observers = 10;
  const int                     num_stoppers  = 5;

  for (int i = 0; i < num_observers; i++) {
    observers.emplace_back(ev.get_token());
  }
  std::vector<std::atomic<bool>> finished(num_stoppers);
  for (int i = 0; i < num_stoppers; i++) {
    workers.push_back(std::make_unique<std::thread>([this, &finished, i]() {
      ev.stop();
      finished[i] = true;
    }));
  }

  while (not observers[0].stop_requested()) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }

  for (int i = 0; i != num_observers; i++) {
    ASSERT_TRUE(observers[i].stop_requested()) << "All observers should be the stop() signal at this point";
  }
  for (int i = 0; i != num_stoppers; i++) {
    ASSERT_FALSE(finished[i]) << "No stopper should have finished yet";
  }

  // All except one observer are reset.
  for (int i = 0; i < num_observers - 1; i++) {
    observers[i].reset();
  }
  for (int i = 0; i != num_stoppers; i++) {
    ASSERT_FALSE(finished[i]) << "No stopper should have finished yet";
  }

  // All stoppers should unlock now.
  observers[num_observers - 1].reset();
  for (int i = 0; i != num_stoppers; i++) {
    workers[i]->join();
    ASSERT_TRUE(finished[i]) << "All stoppers should have finished now";
  }
}

TEST_F(stop_event_test, reset_on_event_not_stopped_is_noop)
{
  auto obs = ev.get_token();
  ASSERT_EQ(ev.nof_tokens_approx(), 1);
  ev.reset();
  ASSERT_EQ(ev.nof_tokens_approx(), 1);
}

TEST_F(stop_event_test, reset_on_stopped_event_makes_it_reusable)
{
  ASSERT_FALSE(ev.stop_was_requested());
  auto obs = ev.get_token();
  ASSERT_FALSE(ev.stop_was_requested());
  obs.reset();
  ASSERT_FALSE(ev.stop_was_requested());
  ev.stop();
  ASSERT_TRUE(ev.stop_was_requested());
  ev.reset();
  ASSERT_FALSE(ev.stop_was_requested());
}

TEST_F(stop_event_test, reset_waits_for_ungoing_stop)
{
  auto obs = ev.get_token();
  workers.push_back(std::make_unique<std::thread>([this]() { ev.stop(); }));
  while (not obs.stop_requested()) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }

  std::atomic<bool> reset_finished{false};
  workers.push_back(std::make_unique<std::thread>([this, &reset_finished]() {
    ev.reset();
    reset_finished = true;
  }));

  ASSERT_FALSE(reset_finished);
  obs.reset();
  workers[0]->join();
  workers[1]->join();
}
