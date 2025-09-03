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

#include "srsran/support/synchronization/sync_event.h"
#include <atomic>
#include <cstdint>
#include <utility>

namespace srsran {

/// \brief Observer of calls to stop() on the associated stop_event_source.
///
/// When it gets reset or destroyed, it notifies the stop_event_source that one observer is gone.
class stop_event_token
{
  static constexpr uint32_t stop_bit = 1U << 31U;

public:
  stop_event_token() = default;
  stop_event_token(const intrusive_ptr<scoped_sync_detail::state>& state_) : state_ptr(state_) { inc_token(); }
  stop_event_token(const stop_event_token& other) : state_ptr(other.state_ptr) { inc_token(); }
  stop_event_token(stop_event_token&& other) noexcept : state_ptr(std::move(other.state_ptr)) {}
  ~stop_event_token() { reset(); }
  stop_event_token& operator=(const stop_event_token& other)
  {
    if (this != &other) {
      reset();
      state_ptr = other.state_ptr;
      inc_token();
    }
    return *this;
  }
  stop_event_token& operator=(stop_event_token&& other) noexcept
  {
    reset();
    state_ptr = std::move(other.state_ptr);
    return *this;
  }

  /// Checks if the associated sync_event has been stopped.
  [[nodiscard]] bool stop_requested() const
  {
    return state_ptr == nullptr or (state_ptr->token_count.load(std::memory_order_acquire) & stop_bit) > 0;
  }

  /// Destroys the observer.
  void reset()
  {
    if (state_ptr != nullptr) {
      auto cur = state_ptr->token_count.fetch_sub(1, std::memory_order_acq_rel) - 1;
      if (cur == stop_bit) {
        // Stop has been requested and count reached zero. Wake all stoppers.
        futex_util::wake_all(state_ptr->token_count);
      }
      state_ptr.reset();
    }
  }

private:
  void inc_token()
  {
    if (state_ptr != nullptr and state_ptr->token_count.fetch_add(1, std::memory_order_relaxed) & stop_bit) {
      // Stop was already requested. Release token.
      reset();
    }
  }

  intrusive_ptr<scoped_sync_detail::state> state_ptr;
};

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
  ~stop_event_source() { stop(); }

  /// Creates a new observer of stop() requests.
  stop_event_token get_token() { return stop_event_token{state_ptr}; }

  /// Requests all tokens to stop and blocks until all tokens are reset.
  void stop()
  {
    // Mark as stopped.
    auto cur = state_ptr->token_count.fetch_or(stop_bit, std::memory_order_acq_rel) | stop_bit;

    // Block waiting until all observers are gone.
    while (cur > stop_bit) {
      futex_util::wait(state_ptr->token_count, cur);
      cur = state_ptr->token_count.load(std::memory_order_acquire);
    }
  }

  /// Resets stop_requested back to false. If there are tokens yet to complete an earlier stop(), this call will block.
  void reset()
  {
    auto cur = state_ptr->token_count.load(std::memory_order_acquire);
    while (cur >= stop_bit) {
      // In case a stop is on-going (cur > stop_bit), we need to wait until it completes before resetting.
      while (cur > stop_bit) {
        futex_util::wait(state_ptr->token_count, cur);
        cur = state_ptr->token_count.load(std::memory_order_acquire);
      }
      // cur <= stop_bit, which means that the stop() either completed or was not requested.

      // In case cur == stop_bit, we try to reset the stop request.
      if (cur == stop_bit and state_ptr->token_count.compare_exchange_strong(
                                  cur, 0, std::memory_order_acq_rel, std::memory_order_relaxed)) {
        // Successfully reset stop request.
        return;
      }
    }
  }

  [[nodiscard]] bool stop_was_requested() const
  {
    return state_ptr->token_count.load(std::memory_order_acquire) >= stop_bit;
  }

  [[nodiscard]] uint32_t nof_tokens_approx() const
  {
    return state_ptr->token_count.load(std::memory_order_relaxed) & count_mask;
  }

private:
  /// State variable composed by 1 MSB bit for signalling stop and 31 LSB bits for counting observers.
  intrusive_ptr<scoped_sync_detail::state> state_ptr{new scoped_sync_detail::state()};
};

} // namespace srsran
