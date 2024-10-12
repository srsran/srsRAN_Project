/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Support formatters used to log information related to the PHY layer.

#pragma once

#include <chrono>

namespace fmt {

/// Custom formatter for \c std::chrono::nanoseconds.
template <>
struct formatter<std::chrono::nanoseconds> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::chrono::nanoseconds& nanoseconds, FormatContext& ctx)

  {
    return format_to(ctx.out(), "t={:.1f}us", static_cast<float>(nanoseconds.count()) * 1e-3F);
  }
};

} // namespace fmt
