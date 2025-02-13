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
#include <functional>

namespace srsran {

/// Atomically compares and updates the values of the passed atomics if the respective condition is true.
inline void update_minmax(uint64_t new_value, std::atomic<uint64_t>& max, std::atomic<uint64_t>& min)
{
  auto current_min = min.load();
  auto current_max = max.load();
  while (new_value < current_min && !min.compare_exchange_weak(current_min, new_value)) {
  }
  while (new_value > current_max && !max.compare_exchange_weak(current_max, new_value)) {
  }
}

} // namespace srsran
