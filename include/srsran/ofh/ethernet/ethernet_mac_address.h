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

#include <array>
#include <cstdint>

namespace srsran {
namespace ether {

/// Size in bytes of an Ethernet MAC address.
static constexpr unsigned ETH_ADDR_LEN = 6;

/// Type alias for an Ethernet MAC address.
using mac_address = std::array<uint8_t, ETH_ADDR_LEN>;

} // namespace ether
} // namespace srsran
