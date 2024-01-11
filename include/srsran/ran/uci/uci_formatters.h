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
