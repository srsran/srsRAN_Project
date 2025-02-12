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
