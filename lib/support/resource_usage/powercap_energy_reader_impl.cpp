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

#include "powercap_energy_reader_impl.h"
#include <charconv>
#include <fstream>

using namespace srsran;
using namespace resource_usage_utils;

/// Intel_rapl_msr and intel_rapl_common drivers support Intel CPUs,
/// as well as AMD CPUs starting from 17th generation. The drivers provide sysfs Powercap support.
static constexpr auto rapl_powercap_path      = "/sys/class/powercap/intel-rapl";
static constexpr auto rapl_powercap_pkg_path  = "/sys/class/powercap/intel-rapl/intel-rapl:0/energy_uj";
static constexpr auto rapl_powercap_core_path = "/sys/class/powercap/intel-rapl/intel-rapl:0:0/energy_uj";

energy_consumption resource_usage_utils::sysfs_powercap_reader::read_consumed_energy() const
{
  energy_consumption probe;
  // Read package energy consumption.
  read_value_from_sysfs(rapl_powercap_pkg_path, probe.package_consumed_uj);
  // Read CPU cores power consumption.
  read_value_from_sysfs(rapl_powercap_core_path, probe.cpu_core_consumed_uj);

  return probe;
}

uint64_t resource_usage_utils::sysfs_powercap_reader::parse_uint64(const std::string& value) const
{
  uint64_t result{};
  auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), result);

  if (ec != std::errc() || ptr != (value.data() + value.size())) {
    return 0;
  }
  return result;
}

void resource_usage_utils::sysfs_powercap_reader::read_value_from_sysfs(const std::string& sysfs_path,
                                                                        uint64_t&          value_uj) const
{
  std::ifstream file(sysfs_path);
  if (file.is_open()) {
    std::string read_joules;
    std::getline(file, read_joules);
    value_uj = parse_uint64(read_joules);
  }
}

std::unique_ptr<energy_consumption_reader>
resource_usage_utils::build_sysfs_powercap_reader(srslog::basic_logger& logger)
{
  // Checks whether the RAPL Powercap sysfs is available.
  std::ifstream rapl_sysfs(rapl_powercap_path);
  if (!rapl_sysfs.is_open()) {
    logger.warning("Energy consumption utils: Powercap sysfs is not available.");
    return nullptr;
  }

  return std::make_unique<sysfs_powercap_reader>();
}
