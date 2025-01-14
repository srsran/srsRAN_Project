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

#include "srsran/adt/bounded_integer.h"
#include "srsran/support/srsran_assert.h"
#include "fmt/format.h"

namespace srsran {

/// \brief QoS Priority Level. See TS 38.473 Qos Priority Level. Values: (0..127).
struct qos_prio_level_t : public bounded_integer<uint8_t, 0, 127> {
  using bounded_integer::bounded_integer;
};

} // namespace srsran

// Formatters
namespace fmt {
template <>
struct formatter<srsran::qos_prio_level_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::qos_prio_level_t& qos_prio_level, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "QoS Priority Level={:#x}", qos_prio_level.value());
  }
};

} // namespace fmt
