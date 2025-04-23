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
