/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/support/executors/detail/task_executor_utils.h"
#include "srsran/support/executors/task_executor.h"
#include <thread>

namespace srsran {

/// \brief Executor adapter that throttles the caller when the number of tasks in flight exceeds a threshold.
/// This adapter is useful when operating in non-real-time setups, where high OS priority threads can starve out other
/// threads.
template <typename Executor>
class executor_throttler final : public task_executor
{
public:
  template <typename E>
  explicit executor_throttler(E&& executor_, unsigned thres_throttle_) :
    exec(std::forward<E>(executor_)), thres_throttle(thres_throttle_)
  {
  }

  [[nodiscard]] bool execute(unique_task task) override { return dispatch_impl<true>(std::move(task)); }

  [[nodiscard]] bool defer(unique_task task) override { return dispatch_impl<false>(std::move(task)); }

private:
  template <bool IsExecute>
  bool dispatch_impl(unique_task task)
  {
    if (count.fetch_add(1, std::memory_order_relaxed) >= thres_throttle) {
      std::this_thread::yield(); // Yield to allow other tasks to execute.
    }

    auto wrapped_task = [c = &count, t = std::move(task)]() {
      // Execute task.
      t();
      // Decrement counter.
      c->fetch_sub(1, std::memory_order_relaxed);
    };

    bool success = false;
    if constexpr (IsExecute) {
      success = detail::get_task_executor_ref(exec).execute(std::move(wrapped_task));
    } else {
      success = detail::get_task_executor_ref(exec).defer(std::move(wrapped_task));
    }
    if (not success) {
      count.fetch_sub(1, std::memory_order_relaxed);
    }
    return success;
  }

  Executor              exec;
  const unsigned        thres_throttle;
  std::atomic<unsigned> count{0};
};

/// \brief Creates a unique pointer to an executor_throttler instance.
template <typename Executor>
std::unique_ptr<task_executor> make_executor_throttler_ptr(Executor&& executor, unsigned thres_throttle)
{
  return std::make_unique<executor_throttler<Executor>>(std::forward<Executor>(executor), thres_throttle);
}

} // namespace srsran
