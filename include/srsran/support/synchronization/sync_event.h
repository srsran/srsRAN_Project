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
#include <thread>
#include <utility>

namespace srsran {

/// \brief Scoped token that notifies the associated sync_event when it gets destroyed or reset.
class scoped_sync_token
{
public:
  scoped_sync_token() = default;
  scoped_sync_token(std::atomic<uint32_t>& token_count_, std::atomic<uint32_t>& dtor_guard_) :
    token_count(&token_count_), dtor_guard(&dtor_guard_)
  {
    inc_token();
  }
  scoped_sync_token(const scoped_sync_token& other) : token_count(other.token_count), dtor_guard(other.dtor_guard)
  {
    inc_token();
  }
  scoped_sync_token(scoped_sync_token&& other) noexcept :
    token_count(std::exchange(other.token_count, nullptr)), dtor_guard(std::exchange(other.dtor_guard, nullptr))
  {
  }
  ~scoped_sync_token() { reset(); }
  scoped_sync_token& operator=(const scoped_sync_token& other)
  {
    if (this != &other) {
      reset();
      token_count = other.token_count;
      dtor_guard  = other.dtor_guard;
      inc_token();
    }
    return *this;
  }
  scoped_sync_token& operator=(scoped_sync_token&& other) noexcept
  {
    reset();
    token_count = std::exchange(other.token_count, nullptr);
    dtor_guard  = std::exchange(other.dtor_guard, nullptr);
    return *this;
  }

  /// Destroys the token and potentially unlocks sync_event::wait().
  void reset()
  {
    if (token_count != nullptr) {
      auto cur = token_count->fetch_sub(1, std::memory_order_acq_rel) - 1;
      if (cur == 0) {
        // Count is zero. Wake all waiters.
        futex_util::wake_all(*token_count);
        // Update dtor guard.
        dtor_guard->fetch_sub(1, std::memory_order_acq_rel);
      }
      token_count = nullptr;
      dtor_guard  = nullptr;
    }
  }

private:
  void inc_token()
  {
    if (token_count != nullptr) {
      if (token_count->fetch_add(1, std::memory_order_relaxed) == 0) {
        // Transition from 0 to 1. Update dtor guard.
        dtor_guard->fetch_add(1, std::memory_order_relaxed);
      }
    }
  }

  std::atomic<uint32_t>* token_count = nullptr;
  std::atomic<uint32_t>* dtor_guard  = nullptr;
};

/// \brief Synchronization event to wait until all tokens to be reset.
///
/// This class operates like a std::barrier, where the "arrive" call is performed by "tokens" in an RAII manner and
/// "wait" is performed by the \c sync_event. Contrarily to the std::barrier, the number of participants is not fixed.
class sync_event
{
public:
  ~sync_event()
  {
    wait();

    // Polite spinning in case the token got preempted between the fetch_sub and futex wake.
    while (dtor_guard.load(std::memory_order_acquire) > 0) {
      std::this_thread::yield();
    }
  }

  /// Creates a new observer of stop() requests.
  scoped_sync_token get_token() { return scoped_sync_token{token_count, dtor_guard}; }

  /// Waits for all tokens to be reset. At the end of this call, all tokens are guaranteed to be reset.
  void wait()
  {
    // Block waiting until all observers are gone.
    auto cur = token_count.load(std::memory_order_acquire);
    while (cur > 0) {
      futex_util::wait(token_count, cur);
      cur = token_count.load(std::memory_order_acquire);
    }
  }

  [[nodiscard]] uint32_t nof_tokens_approx() const { return token_count.load(std::memory_order_relaxed); }

private:
  std::atomic<uint32_t> token_count{0};
  std::atomic<uint32_t> dtor_guard{0};
};

} // namespace srsran
