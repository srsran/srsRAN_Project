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

#include "srsran/support/resource_usage/power_consumption.h"

namespace srsran {
namespace resource_usage_utils {

/// This class reads energy consumption using Powercap RAPL (Run Time Average Power Limiting) sysfs.
class sysfs_powercap_reader : public energy_consumption_reader
{
public:
  // See interface for documentation.
  energy_consumption read_consumed_energy() const override;

private:
  /// Converts string read from sysfs to uint64.
  uint64_t parse_uint64(const std::string& value) const;

  /// Read value from the given file and store its value in the provided variable.
  void read_value_from_sysfs(const std::string& sysfs_path, uint64_t& value_uj) const;
};

/// Creates Powercap RAPL sysfs reader if it is supported by the system.
std::unique_ptr<energy_consumption_reader> build_sysfs_powercap_reader(srslog::basic_logger& logger);

} // namespace resource_usage_utils
} // namespace srsran
