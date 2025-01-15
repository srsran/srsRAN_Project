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

#include <cstdint>

namespace srsran {
namespace ether {

/// Ethernet type for the eCPRI.
constexpr uint16_t ECPRI_ETH_TYPE = 0xaefe;

/// Maximum length of Ethernet Jumbo frame.
constexpr unsigned MAX_ETH_FRAME_LENGTH = 9600;

/// Minimal Ethernet frame length.
constexpr unsigned MIN_ETH_FRAME_LENGTH = 64;

} // namespace ether
} // namespace srsran
