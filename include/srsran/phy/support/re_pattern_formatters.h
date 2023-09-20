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
#include "srsran/phy/support/re_pattern.h"
#include "srsran/support/format_utils.h"
#include <fmt/format.h>

namespace fmt {

// \brief Custom formatter for \c re_pattern.
template <>
struct formatter<srsran::re_pattern> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::re_pattern& pattern, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(
        ctx, "symb={:n}", static_cast<srsran::bounded_bitset<srsran::MAX_NSYMB_PER_SLOT>>(pattern.symbols));
    helper.format_always(ctx, "rb={:n}", pattern.prb_mask);
    helper.format_always(ctx, "re={:n}", static_cast<srsran::bounded_bitset<srsran::NRE>>(pattern.re_mask));

    return ctx.out();
  }
};

} // namespace fmt
