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

#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/srslog/srslog.h"
#include <string>
#include <vector>

namespace srsran {

/// RU emulator OFH configuration parameters.
struct ru_emulator_ofh_appconfig {
  /// T2a maximum parameter for downlink Control-Plane in microseconds.
  std::chrono::microseconds T2a_max_cp_dl{500};
  /// T2a minimum parameter for downlink Control-Plane in microseconds.
  std::chrono::microseconds T2a_min_cp_dl{258};
  /// T2a maximum parameter for uplink Control-Plane in microseconds.
  std::chrono::microseconds T2a_max_cp_ul{500};
  /// T2a minimum parameter for uplink Control-Plane in microseconds.
  std::chrono::microseconds T2a_min_cp_ul{285};
  /// T2a maximum parameter for downlink User-Plane in microseconds.
  std::chrono::microseconds T2a_max_up{300};
  /// T2a minimum parameter for downlink User-Plane in microseconds.
  std::chrono::microseconds T2a_min_up{85};
  /// Ethernet network interface name or PCI bus identifier.
  std::string network_interface;
  /// RU emulator MAC address.
  std::string ru_mac_address;
  /// Distributed Unit MAC address.
  std::string du_mac_address;
  /// V-LAN Tag control information field.
  unsigned vlan_tag;
  /// Promiscuous mode flag.
  bool enable_promiscuous = false;
  /// RU Uplink ports.
  std::vector<unsigned> ru_ul_port_id = {0, 1};
  /// RU Downlink ports.
  std::vector<unsigned> ru_dl_port_id = {0, 1, 2, 3};
  /// RU PRACH ports.
  std::vector<unsigned> ru_prach_port_id = {4, 5};
  /// RU emulator operating bandwidth.
  bs_channel_bandwidth bandwidth = srsran::bs_channel_bandwidth::MHz100;
  /// Uplink compression method.
  std::string ul_compr_method = "bfp";
  /// Uplink compression bitwidth.
  unsigned ul_compr_bitwidth = 9;
};

/// RU emulator logging parameters.
struct ru_emulator_log_appconfig {
  /// Log level
  srslog::basic_levels level = srslog::basic_levels::info;
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
