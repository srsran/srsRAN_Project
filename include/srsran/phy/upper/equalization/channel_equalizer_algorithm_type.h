/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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