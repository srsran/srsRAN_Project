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

#include <string>

namespace srsran {
namespace ether {

/// Configuration for the Ethernet receiver.
struct receiver_config {
  /// Ethernet interface name or identifier.
  std::string interface;
  /// Promiscuous mode flag.
  bool is_promiscuous_mode_enabled;
  /// If set to true, metrics are enabled in the Ethernet receiver.
  bool are_metrics_enabled = false;
};

} // namespace ether
} // namespace srsran
