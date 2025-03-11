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

namespace srsran {

/// TS 38.321, 6.1.3.1 - Buffer Status Report MAC CEs
enum class bsr_format { SHORT_BSR, LONG_BSR, SHORT_TRUNC_BSR, LONG_TRUNC_BSR };

inline const char* to_string(bsr_format bsr)
{
  static constexpr std::array<const char*, 4> names = {
      "Short BSR", "Long BSR", "Short Truncated BSR", "Long Truncated BSR"};
  auto idx = static_cast<std::size_t>(bsr);
  return idx < names.size() ? names[idx] : "Invalid BSR format";
}

} // namespace srsran
