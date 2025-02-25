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

#include "rapl_msr_energy_reader_impl.h"
#include "srsran/adt/expected.h"
#include "srsran/support/io/unique_fd.h"
#include <fcntl.h>
#include <fstream>

using namespace srsran;
using namespace resource_usage_utils;

/// Address of the MSR interface in /dev.
static constexpr auto msr_path = "/dev/cpu/0/msr";

/// Runtime Average Power Limiting (RAPL) defines a few power planes/domains:
/// - PP0 (CPU cores),
/// - PP1 (GPU),
/// - DRAM
/// - Package (includes PP0 and PP1).

/// Intel:
/// Registers addresses are taken from Intel intel_rapl_msr and intel_rapl_common drivers.
/// Package Domain.
static constexpr uint64_t INTEL_MSR_PKG_ENERGY_STATUS = 0x611;
/// PP0 (CPU core) Domain.
static constexpr uint64_t INTEL_MSR_INTEL_PP0_ENERGY_STATUS = 0x639;
/// Register storing power, energy and time units.
static constexpr uint64_t INTEL_MSR_RAPL_POWER_UNIT = 0x606;

/// AMD:
/// Registers addresses are taken from amd_energy driver.
/// Package Domain.
static constexpr uint64_t AMD_MSR_ENERGY_PKG = 0xc001029b;
/// PP0 (CPU core) Domain.
static constexpr uint64_t AMD_MSR_ENERGY_CORE = 0xc001029a;
/// Register storing power, energy and time units.
static constexpr uint64_t AMD_MSR_RAPL_POWER_UNIT = 0xc0010299;

/// Common to Intel and AMD:
/// Offset and mask used to extract energy units.
static constexpr uint64_t ENERGY_UNIT_OFFSET = 0x08;
static constexpr uint64_t ENERGY_UNIT_MASK   = 0x1f00;
static constexpr uint64_t ENERGY_VALUE_MASK  = 0xffffffff;

/// Helper function to open the MSR interface.
static int open_msr_interface()
{
  int fd = ::open(msr_path, O_RDONLY);
  if (fd < 0) {
    srslog::fetch_basic_logger("APP").warning("Failed to open MSR register space, errno=\"{}\"", errno);

    if (errno == EIO) {
      srslog::fetch_basic_logger("APP").warning("CPU0 doesn't support MSRs");
    }
  }
  return fd;
}

/// Helper function to read an MSR with the given address.
static expected<uint64_t> read_msr_register(uint64_t reg_address)
{
  unique_fd fd(open_msr_interface());
  if (fd.is_open()) {
    return make_unexpected(default_error_t{});
  }

  uint64_t data;
  if (::pread(fd.value(), &data, sizeof(data), reg_address) != sizeof(data)) {
    srslog::fetch_basic_logger("APP").warning("Failed to read an MSR at the address {:x}", reg_address);
    return make_unexpected(default_error_t{});
  }

  return {data};
}

resource_usage_utils::rapl_msr_reader::rapl_msr_reader()
{
  cpu_vendor cpu_vendor_id = get_cpu_vendor();
  if (cpu_vendor_id == cpu_vendor::UNSUPPORTED) {
    return;
  }

  if (cpu_vendor_id == cpu_vendor::INTEL) {
    config.package_energy_register = INTEL_MSR_PKG_ENERGY_STATUS;
    config.core_energy_register    = INTEL_MSR_INTEL_PP0_ENERGY_STATUS;
    config.units_register          = INTEL_MSR_RAPL_POWER_UNIT;
  } else {
    config.package_energy_register = AMD_MSR_ENERGY_PKG;
    config.core_energy_register    = AMD_MSR_ENERGY_CORE;
    config.units_register          = AMD_MSR_RAPL_POWER_UNIT;
  }
  initialize_units();
}

energy_consumption resource_usage_utils::rapl_msr_reader::read_consumed_energy() const
{
  energy_consumption probe;
  // Read package energy consumption.
  uint32_t pkg_energy_value = read_msr_register(config.package_energy_register).value_or(0) & ENERGY_VALUE_MASK;
  probe.package_consumed_uj = static_cast<uint64_t>(static_cast<double>(pkg_energy_value) * energy_units * 1e6);
  // Read CPU cores energy consumption.
  uint32_t core_energy_value = read_msr_register(config.core_energy_register).value_or(0) & ENERGY_VALUE_MASK;
  probe.cpu_core_consumed_uj = static_cast<uint64_t>(static_cast<double>(core_energy_value) * energy_units * 1e6);

  return probe;
}

void resource_usage_utils::rapl_msr_reader::initialize_units()
{
  energy_units = 0.0f;
  auto units   = read_msr_register(config.units_register);
  if (units) {
    uint32_t energy_scaling = (units.value() & ENERGY_UNIT_MASK) >> ENERGY_UNIT_OFFSET;
    energy_units            = 1.0f / (1 << energy_scaling);
  }
}

resource_usage_utils::rapl_msr_reader::cpu_vendor resource_usage_utils::rapl_msr_reader::get_cpu_vendor() const
{
  cpu_vendor vendor_id = cpu_vendor::UNSUPPORTED;

  // Parse /proc/cpuinfo
  std::ifstream file("/proc/cpuinfo");
  std::string   line;
  while (std::getline(file, line)) {
    // Look for vendor_id.
    if (line.find("vendor_id") != std::string::npos) {
      if (line.find("GenuineIntel") != std::string::npos) {
        vendor_id = cpu_vendor::INTEL;
      } else if (line.find("AuthenticAMD") != std::string::npos) {
        vendor_id = cpu_vendor::AMD;
      }
      break;
    }
  }
  return vendor_id;
}

std::unique_ptr<energy_consumption_reader> resource_usage_utils::build_rapl_msr_reader(srslog::basic_logger& logger)
{
  std::ifstream rapl_msr(msr_path);
  if (!rapl_msr.is_open()) {
    logger.warning("Energy consumption utils: RAPL MSR interface is not available.");
    return nullptr;
  }

  return std::make_unique<rapl_msr_reader>();
}
