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

/// Channel equalizer algorithm type enumeration.
enum class channel_equalizer_algorithm_type {
  /// Zero Forcing.
  zf = 0,
  /// Minimum Mean Square Error.
  mmse
};

/// Convert channel equalizer algorithm type to string.
inline const char* to_string(channel_equalizer_algorithm_type type)
{
  switch (type) {
    case channel_equalizer_algorithm_type::zf:
      return "ZF";
    case channel_equalizer_algorithm_type::mmse:
    default:
      return "MMSE";
  }
}

} // namespace srsran