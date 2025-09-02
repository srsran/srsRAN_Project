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

#include <atomic>
#include <cstdint>

namespace srsran {

/// \brief Observer of calls to stop() on the associated stop_event_source.
class stop_event_observer
{
public:
  stop_event_observer()                           = default;
  stop_event_observer(const stop_event_observer&) = delete;
  stop_event_observer(stop_event_observer&& other) noexcept : count_ptr(other.count_ptr) { other.count_ptr = nullptr; }
  stop_event_observer(std::atomic<uint32_t>& observer_count_) : count_ptr(&observer_count_)
  {
    count_ptr->fetch_add(1, std::memory_order_acq_rel);
  }
  ~stop_event_observer() { reset(); }
  stop_event_observer& operator=(const stop_event_observer&) = delete;
  stop_event_observer& operator=(stop_event_observer&& other) noexcept
  {
    reset();
    count_ptr       = other.count_ptr;
    other.count_ptr = nullptr;
    return *this;
  }

  /// Checks if the associated sync_event has been stopped.
  bool stop_requested() const { return (count_ptr->load(std::memory_order_acquire) & (1U << 31U)) > 0; }

  /// Destroys the observer.
  void reset()
  {
    if (count_ptr != nullptr) {
      count_ptr->fetch_sub(1, std::memory_order_acq_rel);
      wake_all();
      count_ptr = nullptr;
    }
  }

private:
  void wake_all();

  std::atomic<uint32_t>* count_ptr = nullptr;
};

/// \brief Event to signal stop to multiple observers.
/// The stop_event blocks on stop() until all observers are gone.
class stop_event_source
{
  static constexpr uint32_t stop_bit   = 1U << 31U;
  static constexpr uint32_t count_mask = (1U << 31U) - 1;

public:
  stop_event_observer get_observer() { return stop_event_observer{state}; }

  ~stop_event_source() { stop(); }

  void stop()
  {
    // Mark as stopped.
    auto prev = state.fetch_or(stop_bit, std::memory_order_acq_rel);

    // Block waiting until all observers are gone.
    while ((prev & count_mask) > 0) {
      wait(prev);
      prev = state.load(std::memory_order_acquire);
    }
  }

private:
  void wait(uint32_t expected);

  /// State variable composed by 1 MSB bit for signalling stop and 31 LSB bits for counting observers.
  std::atomic<uint32_t> state{0};
};

} // namespace srsran
