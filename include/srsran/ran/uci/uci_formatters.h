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
#include "srsran/ran/uci/uci_part2_size_description.h"
#include "srsran/support/format_utils.h"

/// Custom formatter for \c srsran::uci_part2_size_description::parameter.
template <>
struct fmt::formatter<srsran::uci_part2_size_description::parameter> {
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
  auto format(const srsran::uci_part2_size_description::parameter& value, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "offset={}", value.offset);
    helper.format_always(ctx, "width={}", value.width);
    return ctx.out();
  }
};

/// Custom formatter for \c srsran::uci_part2_size_description::entry.
template <>
struct fmt::formatter<srsran::uci_part2_size_description::entry> {
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
  auto format(const srsran::uci_part2_size_description::entry& value, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(
        ctx, "params=[{:,}]", srsran::span<const srsran::uci_part2_size_description::parameter>(value.parameters));
    helper.format_always(ctx, "map=[{:,}]", srsran::span<const uint16_t>(value.map));
    return ctx.out();
  }
};

/// Custom formatter for \c srsran::uci_part2_size_description.
template <>
struct fmt::formatter<srsran::uci_part2_size_description> {
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
  auto format(const srsran::uci_part2_size_description& description, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(
        ctx, "entries=[{:,}]", srsran::span<const srsran::uci_part2_size_description::entry>(description.entries));

    return ctx.out();
  }
};
