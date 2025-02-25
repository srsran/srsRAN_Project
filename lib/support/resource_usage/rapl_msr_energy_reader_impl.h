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

/// Class responsible for reading Run Time Average Power Limiting (RAPL) interface MSRs (model-specific registers)
/// of an x86 CPU.
class rapl_msr_reader : public energy_consumption_reader
{
  /// Enum with CPU vendor IDs.
  enum cpu_vendor { INTEL, AMD, UNSUPPORTED };

  /// Helper structure used to aggregate the MSR registers.
  struct msr_registers_config {
    uint64_t package_energy_register;
    uint64_t core_energy_register;
    uint64_t units_register;
  };

public:
  /// Constructor determines which CPU it is running on and initializes
  /// units of energy based on the scaling factor stored in one of the registers.
  rapl_msr_reader();

  // See interface for documentation.
  energy_consumption read_consumed_energy() const override;

private:
  /// Read units register and extract energy units value.
  void initialize_units();

  /// Determine CPU vendor (only Intel and AMD are supported).
  cpu_vendor get_cpu_vendor() const;

  msr_registers_config config;
  double               energy_units = 0.0;
};

/// Creates RAPL MSR reader if it is supported by the system.
std::unique_ptr<energy_consumption_reader> build_rapl_msr_reader(srslog::basic_logger& logger);

} // namespace resource_usage_utils
} // namespace srsran
