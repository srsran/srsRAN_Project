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

#include "fmt/format.h"
#include <cstdint>

namespace srsran {

/// PDCP NR sequence number field.
enum class pdcp_sn_size : uint8_t { size12bits = 12, size18bits = 18, invalid };
inline bool pdcp_sn_size_from_uint(pdcp_sn_size& sn_size, uint16_t num)
{
  if (num == 12) {
    sn_size = pdcp_sn_size::size12bits;
    return true;
  }
  if (num == 18) {
    sn_size = pdcp_sn_size::size18bits;
    return true;
  }
  return false;
}

/// \brief Convert PDCP SN size from enum to unsigned integer.
constexpr uint8_t pdcp_sn_size_to_uint(pdcp_sn_size sn_size)
{
  return static_cast<uint8_t>(sn_size);
}

} // namespace srsran

namespace fmt {

// SN size
template <>
struct formatter<srsran::pdcp_sn_size> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::pdcp_sn_size sn_size, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    switch (sn_size) {
      case srsran::pdcp_sn_size::invalid:
        return format_to(ctx.out(), "invalid");
      default:
        return format_to(ctx.out(), "{}", pdcp_sn_size_to_uint(sn_size));
    }
  }
};

} // namespace fmt
