/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/ran/bs_channel_bandwidth.h"
#include <string>
#include <vector>

namespace srsran {

/// RU emulator OFH configuration parameters.
struct ru_emulator_ofh_appconfig {
  /// Ethernet network interface PCI bus identifier.
  std::string network_interface;
  /// RU emulator MAC address.
  std::string ru_mac_address;
  /// Distributed Unit MAC address.
  std::string du_mac_address;
  /// V-LAN Tag control information field.
  unsigned vlan_tag;
  /// RU Uplink ports.
  std::vector<unsigned> ru_ul_port_id = {0, 1};
  /// RU emulator operating bandwidth.
  bs_channel_bandwidth_fr1 bandwidth = srsran::bs_channel_bandwidth_fr1::MHz100;
  /// Uplink compression method.
  std::string ul_compr_method = "bfp";
  /// Uplink compression bitwidth.
  unsigned ul_compr_bitwidth = 9;
};

/// RU emulator logging parameters.
struct ru_emulator_log_appconfig {
  /// Log level
  std::string level = "info";
  /// Path to log file or "stdout" to print to console.
  std::string filename = "stdout";
};

/// DPDK configuration.
struct ru_emulator_dpdk_appconfig {
  /// EAL configuration arguments.
  std::string eal_args;
};

/// RU emulator application configuration.
struct ru_emulator_appconfig {
  /// Logging configuration.
  ru_emulator_log_appconfig log_cfg;
  /// Individual RU emulators configurations.
  std::vector<ru_emulator_ofh_appconfig> ru_cfg = {{}};
  /// DPDK configuration.
  ru_emulator_dpdk_appconfig dpdk_config;
};

} // namespace srsran
