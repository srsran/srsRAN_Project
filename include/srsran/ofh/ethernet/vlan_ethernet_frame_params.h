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

#include "srsran/ofh/ethernet/ethernet_mac_address.h"

namespace srsran {
namespace ether {

/// VLAN Ethernet frame header parameters.
struct vlan_frame_params {
  /// Destination MAC address.
  mac_address mac_dst_address;
  /// Source MAC address.
  mac_address mac_src_address;
  /// Tag control information field.
  uint16_t tci;
  /// Ethernet type field.
  uint16_t eth_type;
};

} // namespace ether
} // namespace srsran
