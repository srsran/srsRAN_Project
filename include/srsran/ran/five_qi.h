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

#include "srsran/support/srsran_assert.h"
#include "fmt/format.h"

namespace srsran {

// See TS 38.463: Five QI valid values: (0..255)
constexpr static uint16_t MAX_FIVEQI = 255;

/// \brief Five QI.
enum class five_qi_t : uint16_t { min = 0, max = MAX_FIVEQI, invalid = MAX_FIVEQI + 1 };

/// Convert Five QI type to integer.
constexpr inline uint16_t five_qi_to_uint(five_qi_t five_qi)
{
  return static_cast<uint16_t>(five_qi);
}

/// Convert integer to Five QI type.
constexpr inline five_qi_t uint_to_five_qi(uint16_t five_qi)
{
  srsran_assert(
      five_qi < MAX_FIVEQI, "Invalid 5QI {} - Must be between [{}..{}]", five_qi, five_qi_t::min, five_qi_t::max);
  return static_cast<five_qi_t>(five_qi);
}

} // namespace srsran

// Formatters
namespace fmt {
template <>
struct formatter<srsran::five_qi_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::five_qi_t& five_qi, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "5QI={:#x}", five_qi_to_uint(five_qi));
  }
};

} // namespace fmt
