/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/adt/bounded_integer.h"
#include "fmt/format.h"

namespace srsran {

/// \brief Allocation and Retention Priority Level. See TS 38.473 Allocation and Retention Priority. Values: (0..15).
struct arp_prio_level_t : public bounded_integer<uint8_t, 0, 15> {
  using bounded_integer::bounded_integer;
};

} // namespace srsran

// Formatters
namespace fmt {
template <>
struct formatter<srsran::arp_prio_level_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::arp_prio_level_t& arp_prio_level, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "ARP Priority Level={:#x}", arp_prio_level.value());
  }
};

} // namespace fmt
