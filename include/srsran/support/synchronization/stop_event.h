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
  stop_event_token(const stop_event_token& other) : count_ptr(other.count_ptr)
  {
    if (other.count_ptr != nullptr) {
      count_ptr->fetch_add(1, std::memory_order_acq_rel);
    }
  }
  stop_event_token(stop_event_token&& other) noexcept : count_ptr(std::exchange(other.count_ptr, nullptr)) {}
  stop_event_token(std::atomic<uint32_t>& observer_count_) : count_ptr(&observer_count_)
  {
    count_ptr->fetch_add(1, std::memory_order_acq_rel);
  }
  ~stop_event_token() { reset(); }
  stop_event_token& operator=(const stop_event_token& other)
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
  stop_event_token& operator=(stop_event_token&& other) noexcept
  {
    reset();
    count_ptr = std::exchange(other.count_ptr, nullptr);
    return *this;
  }

  /// Checks if the associated sync_event has been stopped.
  [[nodiscard]] bool stop_requested() const { return (count_ptr->load(std::memory_order_acquire) & stop_bit) > 0; }

  /// Destroys the observer.
  void reset()
  {
    if (count_ptr != nullptr) {
      auto cur = count_ptr->fetch_sub(1, std::memory_order_acq_rel) - 1;
      if (cur == stop_bit) {
        // Stop has been requested and count reached zero. Wake all stoppers.
        futex_util::wake_all(*count_ptr);
      }
      count_ptr = nullptr;
    }
  }

private:
  std::atomic<uint32_t>* count_ptr = nullptr;
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
  stop_event_token get_token() { return stop_event_token{state}; }

  /// Requests all tokens to stop and blocks until all tokens are reset.
  void stop()
  {
    // Mark as stopped.
    auto cur = state.fetch_or(stop_bit, std::memory_order_acq_rel) | stop_bit;

    // Block waiting until all observers are gone.
    while (cur > stop_bit) {
      wait(cur);
      cur = state.load(std::memory_order_acquire);
    }
  }

  /// Resets stop_requested back to false. If there are tokens yet to complete an earlier stop(), this call will block.
  void reset()
  {
    auto cur = state.load(std::memory_order_acquire);
    while (cur >= stop_bit) {
      // In case a stop is on-going (cur > stop_bit), we need to wait until it completes before resetting.
      while (cur > stop_bit) {
        wait(cur);
        cur = state.load(std::memory_order_acquire);
      }
      // cur <= stop_bit, which means that the stop() either completed or was not requested.

      // In case cur == stop_bit, we try to reset the stop request.
      if (cur == stop_bit and
          state.compare_exchange_weak(cur, 0, std::memory_order_acq_rel, std::memory_order_relaxed)) {
        // Successfully reset stop request.
        return;
      }
    }
  }

  [[nodiscard]] bool stop_was_requested() const { return state.load(std::memory_order_acquire) >= stop_bit; }

  [[nodiscard]] uint32_t nof_tokens_approx() const { return state.load(std::memory_order_relaxed) & count_mask; }

private:
  /// Waits until state changes from expected.
  void wait(uint32_t expected) { futex_util::wait(state, expected); }

  /// State variable composed by 1 MSB bit for signalling stop and 31 LSB bits for counting observers.
  std::atomic<uint32_t> state{0};
};

} // namespace srsran
