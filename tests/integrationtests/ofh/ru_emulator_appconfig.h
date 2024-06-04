/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/optional.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include <string>
#include <vector>

namespace srsran {

/// RU emulator OFH configuration parameters.
struct ru_emulator_ofh_appconfig {
  /// Ethernet network interface name or PCI bus identifier.
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
  std::optional<ru_emulator_dpdk_appconfig> dpdk_config;
};

} // namespace srsran
