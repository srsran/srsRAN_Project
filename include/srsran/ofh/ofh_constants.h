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

/// Maximum number of Control-Plane message per OFDM symbol.
constexpr size_t MAX_CP_MESSAGES_PER_SYMBOL = 2;

/// Maximum number of User-Plane message per OFDM symbol.
/// \note Segmentation is not supported yet (when a User-Plane packet doesn't fit into a single Ethernet frame it gets
/// segmented in two frames).
constexpr size_t MAX_UP_MESSAGES_PER_SYMBOL = 2;

/// Maximum number of supported eAxC. Implementation defined.
constexpr unsigned MAX_NOF_SUPPORTED_EAXC = 4;

} // namespace ofh
} // namespace srsran
