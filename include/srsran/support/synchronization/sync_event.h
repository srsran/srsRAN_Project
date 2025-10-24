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

#pragma once

#include "srsran/support/synchronization/futex_util.h"
#include <thread>
#include <utility>

namespace srsran {

class scoped_sync_token;

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
    while (dtor_guard.load(std::memory_order_acquire)) {
      std::this_thread::yield();
    }
  }

  /// Creates a new observer of wait() requests.
  [[nodiscard]] scoped_sync_token get_token();

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
  friend class scoped_sync_token;

  std::atomic<uint32_t> token_count{0};
  /// \brief Variable use to protect token_count from destruction when token still has to call futex wake.
  /// Useful reference about the issue: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2616r4.html
  std::atomic<bool> dtor_guard{false};
};

/// Scoped token that notifies the associated sync_event when it gets destroyed or reset.
class scoped_sync_token
{
public:
  scoped_sync_token() = default;
  explicit scoped_sync_token(sync_event& parent_) : parent(&parent_) { inc_token(); }

  scoped_sync_token(const scoped_sync_token& other) : parent(other.parent) { inc_token(); }

  scoped_sync_token(scoped_sync_token&& other) noexcept : parent(std::exchange(other.parent, nullptr)) {}

  scoped_sync_token& operator=(const scoped_sync_token& other)
  {
    if (this != &other) {
      reset();
      parent = other.parent;
      inc_token();
    }
    return *this;
  }
  scoped_sync_token& operator=(scoped_sync_token&& other) noexcept
  {
    reset();
    parent = std::exchange(other.parent, nullptr);
    return *this;
  }

  ~scoped_sync_token()
  {
    if (parent != nullptr) {
      auto cur = parent->token_count.fetch_sub(1, std::memory_order_acq_rel) - 1;
      if (cur == 0) {
        // Count is zero. Wake all waiters.
        futex_util::wake_all(parent->token_count);
        // Update dtor guard.
        parent->dtor_guard.store(false, std::memory_order_release);
      }
      parent = nullptr;
    }
  }

  /// Destroys the token and potentially unlocks sync_event::wait().
  void reset() { scoped_sync_token{}.swap(*this); }

  void swap(scoped_sync_token& other) noexcept { std::swap(parent, other.parent); }

private:
  void inc_token()
  {
    if (parent != nullptr) {
      if (parent->token_count.fetch_add(1, std::memory_order_relaxed) == 0) {
        // Transition from 0 to 1. Lock dtor guard.
        parent->dtor_guard.store(true, std::memory_order_release);
      }
    }
  }

  sync_event* parent = nullptr;
};

inline scoped_sync_token sync_event::get_token()
{
  return scoped_sync_token{*this};
}

} // namespace srsran
