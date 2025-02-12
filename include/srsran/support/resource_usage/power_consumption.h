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

#include "srsran/srslog/srslog.h"

namespace srsran {
namespace resource_usage_utils {

using energy_probe_time_point = std::chrono::high_resolution_clock::time_point;

/// Energy consumption expressed in micro Joules.
struct energy_consumption {
  /// Energy consumed by CPU cores.
  uint64_t cpu_core_consumed_uj;
  /// Energy consumed by package (it can include CPU core, GPU and other components).
  uint64_t package_consumed_uj;
};

/// Calculates a diff of micro Joules taking into account possible wrap around.
uint64_t calculate_energy_diff(uint64_t current_uj, uint64_t previous_uj);

/// Helper struct used to store energy consumption at a given point of time.
struct energy_snapshot {
  energy_consumption      probe;
  energy_probe_time_point probe_time;
};

/// Interface for reading energy consumption.
class energy_consumption_reader
{
public:
  /// Default destructor.
  virtual ~energy_consumption_reader() = default;

  /// Returns consumed energy in micro Joules.
  virtual energy_consumption read_consumed_energy() const = 0;
};

/// Builds an energy consumption reader available in the system.
std::unique_ptr<energy_consumption_reader> build_energy_consumption_reader(srslog::basic_logger& logger);

} // namespace resource_usage_utils
} // namespace srsran
