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

namespace srsran {
namespace uci_constants {

/// \brief Maximum number of UCI bits that can be multiplexed in a single message.
///
/// The value is deduced from the maximum number of bits that can be encoded with the polar encoder as per TS38.212
/// Section 5.2.1.
static constexpr unsigned MAX_NOF_PAYLOAD_BITS = 1706;

/// \brief Maximum number of HARQ bits that can be multiplexed in a single message.
///
/// The value is deduced from the maximum number of bits that can be encoded with the polar encoder as per TS38.212
/// Section 5.2.1.
static constexpr unsigned MAX_NOF_HARQ_BITS = MAX_NOF_PAYLOAD_BITS;

/// \brief Maximum number of CSI Part 1/CSI Part 2 bits that can be multiplexed in a single message.
///
/// The value is deduced from the maximum number of bits that can be encoded with the polar encoder as per TS38.212
/// Section 5.2.1.
static constexpr unsigned MAX_NOF_CSI_PART1_OR_PART2_BITS = MAX_NOF_PAYLOAD_BITS;

} // namespace uci_constants
} // namespace srsran
