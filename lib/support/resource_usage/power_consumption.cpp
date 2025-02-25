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

#include "srsran/support/resource_usage/power_consumption.h"
#include "powercap_energy_reader_impl.h"
#include "rapl_msr_energy_reader_impl.h"

using namespace srsran;
using namespace resource_usage_utils;

namespace {

/// Dummy implementation of energy consumption reader.
class dummy_energy_consumption_reader : public energy_consumption_reader
{
  energy_consumption read_consumed_energy() const override { return {}; }
};

} // namespace

std::unique_ptr<energy_consumption_reader>
resource_usage_utils::build_energy_consumption_reader(srslog::basic_logger& logger)
{
  // First try to build Powercap energy consumption reader.
  if (auto reader = build_sysfs_powercap_reader(logger)) {
    return reader;
  }

  // Try to build RAPL MSR energy consumption reader.
  if (auto reader = build_rapl_msr_reader(logger)) {
    return reader;
  }

  logger.info("Energy consumption utils are not available.");
  return std::make_unique<dummy_energy_consumption_reader>();
}

uint64_t resource_usage_utils::calculate_energy_diff(uint64_t current_uj, uint64_t previous_uj)
{
  int64_t diff = current_uj - previous_uj;
  if (diff < 0) {
    diff = static_cast<int64_t>(UINT64_MAX - previous_uj + current_uj);
  }
  return diff;
}
