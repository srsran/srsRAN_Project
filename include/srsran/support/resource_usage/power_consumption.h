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
