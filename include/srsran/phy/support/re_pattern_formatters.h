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

#include "srsran/phy/support/re_pattern.h"
#include <fmt/format.h>

namespace fmt {

/// \brief Custom formatter for \c re_pattern.
template <>
struct formatter<srsran::re_pattern> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::re_pattern& pattern, FormatContext& ctx)
  {
    helper.format_always(
        ctx, "symb={:n}", static_cast<srsran::bounded_bitset<srsran::MAX_NSYMB_PER_SLOT>>(pattern.symbols));
    helper.format_always(ctx, "rb={:n}", pattern.prb_mask);
    helper.format_always(ctx, "re={:n}", static_cast<srsran::bounded_bitset<srsran::NRE>>(pattern.re_mask));

    return ctx.out();
  }
};

} // namespace fmt
