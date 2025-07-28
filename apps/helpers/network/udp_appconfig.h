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

#include <optional>
#include <string>

namespace srsran {

/// UDP specific configuration of an UDP gateway.
struct udp_appconfig {
  /// Maximum amount of messages RX in a single syscall.
  unsigned rx_max_msgs = 256;
  /// Batched queue size.
  unsigned tx_qsize = 4096;
  /// Maximum amount of messages TX in a single syscall.
  unsigned tx_max_msgs = 256;
  /// Maximum amount of segments in a single TX SDU.
  unsigned tx_max_segments = 256;
  /// Pool accupancy threshold after which packets are dropped.
  float pool_threshold = 0.9;
  /// Differentiated Services Code Point value.
  std::optional<unsigned> dscp;
  /// External address advertised by the UDP-GW.
  std::string ext_addr = "auto";
};

} // namespace srsran
