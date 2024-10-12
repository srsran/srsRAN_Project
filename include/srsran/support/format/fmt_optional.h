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

#include "fmt/format.h"
#include <optional>

namespace fmt {

/// Default formatter for std::optional<T>
template <typename T>
struct formatter<std::optional<T>> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::optional<T>& optval, FormatContext& ctx)
  {
    if (!optval.has_value()) {
      return format_to(ctx.out(), "{{na}}");
    }
    return format_to(ctx.out(), "{}", optval.value());
  }
};

} // namespace fmt
