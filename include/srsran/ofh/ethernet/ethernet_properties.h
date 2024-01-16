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
