/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
constexpr size_t MAX_UP_MESSAGES_PER_SYMBOL = MAX_NOF_SUPPORTED_EAXC * 2;

} // namespace ofh
} // namespace srsran
