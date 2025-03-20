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

#include "srsran/adt/noop_functor.h"
#include <atomic>
#include <chrono>
#include <memory>

namespace srsran {
namespace ofh {

template <typename T>
inline void update_minmax(T new_value, std::atomic<T>& max, std::atomic<T>& min)
{
  auto current_min = min.load(std::memory_order_relaxed);
  auto current_max = max.load(std::memory_order_relaxed);
  while (new_value < current_min && !min.compare_exchange_weak(current_min, new_value)) {
  }
  while (new_value > current_max && !max.compare_exchange_weak(current_max, new_value)) {
  }
}

/// Helper class measuring execution time if metrics are enabled.
class time_execution_measurer
{
public:
  /// The constructor starts measuring execution time of a code-block if metrics are enabled, or does nothing otherwise.
  explicit time_execution_measurer(bool is_enabled_) : is_enabled(is_enabled_)
  {
    if (is_enabled) {
      start_tp = std::chrono::high_resolution_clock::now();
    }
  }

  /// Calculates and returns the execution time of a code-block if metrics are enabled, or does nothing otherwise.
  std::chrono::nanoseconds stop()
  {
    if (is_enabled) {
      auto stop_tp = std::chrono::high_resolution_clock::now();
      return std::chrono::duration_cast<std::chrono::nanoseconds>(stop_tp - start_tp);
    }

    return {};
  }

private:
  /// The measurer measures the execution time if the flag is set to true, otherwise no operation is performed.
  const bool is_enabled;

  /// Time point marking the start of a new measurement.
  std::chrono::high_resolution_clock::time_point start_tp = {};
};

} // namespace ofh
} // namespace srsran
