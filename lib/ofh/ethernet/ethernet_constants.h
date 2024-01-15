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
#include "srsran/support/units.h"

namespace srsran {
namespace ether {

/// VLAN Tag protocol identifier.
constexpr uint16_t VLAN_TPID = 0x8100;

/// Ethernet header size.
constexpr units::bytes ETH_HEADER_SIZE{ETH_ADDR_LEN * 2 + sizeof(uint16_t)};

/// Ethernet VLAN tag size.
constexpr units::bytes ETH_VLAN_TAG_SIZE{sizeof(uint16_t) * 2};

} // namespace ether
} // namespace srsran
