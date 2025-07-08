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
