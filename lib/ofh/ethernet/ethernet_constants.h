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
