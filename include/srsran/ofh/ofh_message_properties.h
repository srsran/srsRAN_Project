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

#include <cstdint>

namespace srsran {
namespace ofh {

/// Open Fronthaul data direction.
enum class data_direction : uint8_t { uplink, downlink };

/// Resource block type.
enum class rb_id_type : uint8_t { every_rb_used, every_other_rb_used };

/// Symbol increment type.
enum class symbol_incr_type : uint8_t { current_symbol_number, increment_current_symbol_number };

/// Filter index type.
enum class filter_index_type { standard_channel_filter, ul_prach_preamble_1p25khz, reserved = 8 };

/// Converts and returns the given filter type into an integer.
constexpr unsigned to_value(filter_index_type filter_type)
{
  return static_cast<unsigned>(filter_type);
}

/// Converts and returns the given index to a filter index type.
constexpr filter_index_type to_filter_index_type(unsigned index)
{
  switch (index) {
    case 0:
      return filter_index_type::standard_channel_filter;
    case 1:
      return filter_index_type::ul_prach_preamble_1p25khz;
    default:
      return filter_index_type::reserved;
  }
}

/// Open Fronthaul section types.
enum class section_type {
  /// Used for indicating idle or guard periods from O-DU to O-RU.
  type_0,
  /// Used for most Downlink and Uplink physical radio channels.
  type_1,
  /// Used for PRACH and mixed-numerology channels.
  type_3,
  /// Used for UE scheduling information.
  type_5,
  /// Used for sending channel information for a specific UE ID.
  type_6
};

} // namespace ofh
} // namespace srsran
