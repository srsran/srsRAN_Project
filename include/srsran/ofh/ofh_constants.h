/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include <cstddef>

namespace srsran {
namespace ofh {

/// Open Fronthaul message type.
enum class message_type { control_plane, user_plane, num_ofh_types };

/// Maximum number of supported eAxC. Implementation defined.
constexpr unsigned MAX_NOF_SUPPORTED_EAXC = 4;

/// Maximum allowed value for eAxC ID.
constexpr size_t MAX_SUPPORTED_EAXC_ID_VALUE = 32;

/// Maximum number of Control-Plane message per OFDM symbol (reserved for UL and PRACH messages).
constexpr size_t MAX_CP_MESSAGES_PER_SYMBOL = MAX_NOF_SUPPORTED_EAXC * 2;

/// Maximum number of User-Plane message per OFDM symbol.
/// \note User-Plane packet might be segmented in 2 Ethernet packets, assuming usage of Ethernet jumbo frames.
constexpr size_t MAX_UP_MESSAGES_PER_SYMBOL = 2;

} // namespace ofh
} // namespace srsran
