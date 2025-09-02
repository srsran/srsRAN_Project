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

#include "srsran/support/synchronization/futex_util.h"

namespace srsran {

/// \brief Scoped token that notifies the associated sync_event when it gets destroyed or reset.
class scoped_sync_token
{
public:
  scoped_sync_token() = default;
  scoped_sync_token(const scoped_sync_token& other) : count_ptr(other.count_ptr)
  {
    if (other.count_ptr != nullptr) {
      count_ptr->fetch_add(1, std::memory_order_acq_rel);
    }
  }
  scoped_sync_token(scoped_sync_token&& other) noexcept : count_ptr(other.count_ptr) { other.count_ptr = nullptr; }
  scoped_sync_token(std::atomic<uint32_t>& observer_count_) : count_ptr(&observer_count_)
  {
    count_ptr->fetch_add(1, std::memory_order_acq_rel);
  }
  ~scoped_sync_token() { reset(); }
  scoped_sync_token& operator=(const scoped_sync_token& other)
  {
    if (this != &other) {
      reset();
      count_ptr = other.count_ptr;
      if (other.count_ptr != nullptr) {
        count_ptr->fetch_add(1, std::memory_order_acq_rel);
      }
    }
    return *this;
  }
  scoped_sync_token& operator=(scoped_sync_token&& other) noexcept
  {
    reset();
    count_ptr       = other.count_ptr;
    other.count_ptr = nullptr;
    return *this;
  }

  /// Destroys the token and potentially unlocks sync_event::wait().
  void reset()
  {
    if (count_ptr != nullptr) {
      auto cur = count_ptr->fetch_sub(1, std::memory_order_acq_rel) - 1;
      if (cur == 0) {
        // Count is zero.
        futex_util::wake_all(*count_ptr);
      }
      count_ptr = nullptr;
    }
  }

private:
  std::atomic<uint32_t>* count_ptr = nullptr;
};

/// \brief Synchronization event to wait until all tokens to be reset.
/// This class operates like a std::barrier, where the "arrive" call is performed by "tokens" in an RAII manner and
/// "wait" is performed by the \c sync_event. Contrarily to the std::barrier, the number of participants is not fixed.
class sync_event
{
public:
  ~sync_event() { wait(); }

  /// Creates a new observer of stop() requests.
  scoped_sync_token get_token() { return scoped_sync_token{count}; }

  /// Waits for all tokens to be reset.
  void wait()
  {
    // Block waiting until all observers are gone.
    auto cur = count.load(std::memory_order_acquire);
    while (cur > 0) {
      futex_util::wait(count, cur);
      cur = count.load(std::memory_order_acquire);
    }
  }

  [[nodiscard]] uint32_t nof_tokens_approx() const { return count.load(std::memory_order_relaxed); }

private:
  /// Number of current tokens.
  std::atomic<uint32_t> count{0};
};

} // namespace srsran
