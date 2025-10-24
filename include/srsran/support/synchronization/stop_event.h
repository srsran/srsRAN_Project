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
#include <atomic>
#include <thread>
#include <utility>

namespace srsran {

class stop_event_token;

/// \brief Event to signal stop to multiple observers.
///
/// The stop_event blocks on stop() until all observers are gone.
/// This class is similar to \c sync_event. However, it also supports the ability to notify a stop request that is
/// visible to the tokens and allows checking if a stop was requested.
class stop_event_source
{
  static constexpr uint32_t stop_bit   = 1U << 31U;
  static constexpr uint32_t count_mask = stop_bit - 1;

public:
  ~stop_event_source()
  {
    stop();
    // Polite spinning in case the token got preempted between the fetch_sub and futex wake.
    while (dtor_guard.load(std::memory_order_acquire)) {
      std::this_thread::yield();
    }
  }

  /// Creates a new observer of stop() requests.
  [[nodiscard]] stop_event_token get_token();

  /// Requests all tokens to stop and blocks until all tokens are reset.
  void stop()
  {
    // Mark as stopped.
    auto cur = token_count.fetch_or(stop_bit, std::memory_order_acq_rel) | stop_bit;

    // Block waiting until all observers are gone.
    while (cur > stop_bit) {
      futex_util::wait(token_count, cur);
      cur = token_count.load(std::memory_order_acquire);
    }
  }

  /// Resets stop_requested back to false. If there are tokens yet to complete an earlier stop(), this call will block.
  void reset()
  {
    auto cur = token_count.load(std::memory_order_acquire);
    while (cur >= stop_bit) {
      // In case a stop is ongoing (cur > stop_bit), we need to wait until it completes before resetting.
      while (cur > stop_bit) {
        futex_util::wait(token_count, cur);
        cur = token_count.load(std::memory_order_acquire);
      }
      // cur <= stop_bit, which means that the stop() either completed or was not requested.

      // In case cur == stop_bit, we try to reset the stop request.
      if (cur == stop_bit and
          token_count.compare_exchange_strong(cur, 0, std::memory_order_acq_rel, std::memory_order_relaxed)) {
        // Successfully reset stop request.
        return;
      }
    }
  }

  [[nodiscard]] bool stop_was_requested() const { return token_count.load(std::memory_order_acquire) >= stop_bit; }

  [[nodiscard]] uint32_t nof_tokens_approx() const { return token_count.load(std::memory_order_relaxed) & count_mask; }

private:
  friend stop_event_token;

  /// State variable composed by 1 MSB bit for signalling stop and 31 LSB bits for counting observers.
  std::atomic<uint32_t> token_count{0};
  /// Variable used to protect token_count from destruction when token still has to call futex wake.
  std::atomic<bool> dtor_guard{false};
};

/// \brief Observer of calls to stop() on the associated stop_event_source.
///
/// When it gets reset or destroyed, it notifies the stop_event_source that one observer is gone.
class stop_event_token
{
  static constexpr uint32_t stop_bit   = 1U << 31U;
  static constexpr uint32_t count_mask = stop_bit - 1;

public:
  stop_event_token() = default;
  explicit stop_event_token(stop_event_source& parent_) : parent(&parent_) { inc_token(); }

  stop_event_token(const stop_event_token& other) : parent(other.parent) { inc_token(); }
  stop_event_token(stop_event_token&& other) noexcept : parent(std::exchange(other.parent, nullptr)) {}

  stop_event_token& operator=(const stop_event_token& other)
  {
    if (this != &other) {
      reset();
      parent = other.parent;
      inc_token();
    }
    return *this;
  }
  stop_event_token& operator=(stop_event_token&& other) noexcept
  {
    reset();
    parent = std::exchange(other.parent, nullptr);
    return *this;
  }

  ~stop_event_token()
  {
    if (parent != nullptr) {
      auto cur = parent->token_count.fetch_sub(1, std::memory_order_acq_rel) - 1;
      if ((cur & count_mask) == 0) {
        // count reached zero.
        // Wake all stoppers.
        futex_util::wake_all(parent->token_count);
        // Update dtor guard.
        parent->dtor_guard.store(false, std::memory_order_release);
      }
      parent = nullptr;
    }
  }

  /// Checks if the associated stop_event_source has been stopped.
  [[nodiscard]] bool stop_requested() const
  {
    return (parent == nullptr) or ((parent->token_count.load(std::memory_order_acquire) & stop_bit) > 0);
  }

  /// Destroys the observer.
  void reset() { stop_event_token{}.swap(*this); }

  void swap(stop_event_token& other) noexcept { std::swap(parent, other.parent); }

private:
  void inc_token()
  {
    if (parent == nullptr) {
      return;
    }
    auto prev = parent->token_count.fetch_add(1, std::memory_order_relaxed);
    if ((prev & count_mask) == 0) {
      // Transition from 0 to 1. Update dtor guard.
      parent->dtor_guard.store(true, std::memory_order_release);
    }
    if (prev & stop_bit) {
      // Stop was already requested. Release token.
      reset();
    }
  }

  stop_event_source* parent = nullptr;
};

inline stop_event_token stop_event_source::get_token()
{
  return stop_event_token{*this};
}

} // namespace srsran
