/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace srsran {
namespace detail {

// Barrier implementation based on sleep.
struct queue_sleep_barrier {
public:
  queue_sleep_barrier(std::chrono::microseconds sleep_time_ = std::chrono::microseconds{0}) : sleep_time(sleep_time_) {}

  void request_stop() { running = false; }

  void notify_push()
  {
    // do nothing.
  }

  void notify_pop()
  {
    // do nothing.
  }

  template <typename HasSpace>
  void wait_push(std::unique_lock<std::mutex>& lock, const HasSpace& has_space)
  {
    while (running.load(std::memory_order_relaxed) and not has_space()) {
      lock.unlock();
      std::this_thread::sleep_for(sleep_time);
      lock.lock();
    }
  }

  template <typename HasElem>
  void wait_pop(std::unique_lock<std::mutex>& lock, const HasElem& has_elem)
  {
    while (running.load(std::memory_order_relaxed) and not has_elem()) {
      lock.unlock();
      std::this_thread::sleep_for(sleep_time);
      lock.lock();
    }
  }

  bool is_running() const { return running.load(std::memory_order_relaxed); }

private:
  std::atomic<bool>         running{true};
  std::chrono::microseconds sleep_time;
};

// Barrier implementation based on condition variable.
struct queue_cond_var_barrier {
public:
  void request_stop()
  {
    running = false;
    notify_push();
    notify_pop();
  }

  void notify_push() { cvar_push.notify_one(); }

  void notify_pop() { cvar_pop.notify_one(); }

  template <typename HasSpace>
  void wait_push(std::unique_lock<std::mutex>& lock, const HasSpace& has_space)
  {
    cvar_pop.wait(lock, [this, &has_space]() { return not running.load(std::memory_order_relaxed) or has_space(); });
  }

  template <typename HasElem>
  void wait_pop(std::unique_lock<std::mutex>& lock, const HasElem& has_elem)
  {
    cvar_push.wait(lock, [this, &has_elem]() { return not running.load(std::memory_order_relaxed) or has_elem(); });
  }

  bool is_running() const { return running.load(std::memory_order_relaxed); }

private:
  std::atomic<bool>       running{true};
  std::condition_variable cvar_pop, cvar_push;
};

} // namespace detail
} // namespace srsran