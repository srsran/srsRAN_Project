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
#include <optional>
#include <type_traits>

//: TODO: This header file will get eventually moved to support after our optional<T> implementation goes away.

namespace fmt {

/// Default formatter for std::optional<T>
template <typename T>
struct formatter<std::optional<T>> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::optional<T>& optval, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    if (!optval.has_value()) {
      return format_to(ctx.out(), "{{na}}");
    }
    return format_to(ctx.out(), "{}", optval.value());
  }
};

} // namespace fmt
