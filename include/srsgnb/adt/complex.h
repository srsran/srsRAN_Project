/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "fmt/format.h"
#include <complex>

namespace srsgnb {

/// Type to store complex samples.
using cf_t = std::complex<float>;

} // namespace srsgnb

namespace fmt {

/// FMT formatter of cf_t type.
template <>
struct formatter<srsgnb::cf_t> {
  // Stores parsed format string.
  memory_buffer format_buffer;

  formatter()
  {
    static const string_view DEFAULT_FORMAT = "{:+f}{:+f}j";
    format_buffer.append(DEFAULT_FORMAT.begin(), DEFAULT_FORMAT.end());
  }

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    static const fmt::string_view PREAMBLE_FORMAT = "{:";

    // Skip if context is empty and use default format.
    if (ctx.begin() == ctx.end()) {
      return ctx.end();
    }

    // Store the format string.
    format_buffer.clear();
    format_buffer.append(PREAMBLE_FORMAT.begin(), PREAMBLE_FORMAT.end());
    for (auto& it : ctx) {
      format_buffer.push_back(it);

      // Found the end of the context.
      if (it == '}') {
        // Replicate the format string for the imaginary part.
        format_buffer.append(format_buffer.begin(), format_buffer.end());
        format_buffer.push_back('j');
        return &it;
      }
    }

    // No end of context was found.
    return ctx.end();
  }

  template <typename FormatContext>
  auto format(srsgnb::cf_t value, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    const string_view format_str = string_view(format_buffer.data(), format_buffer.size());
    return format_to(ctx.out(), format_str, value.real(), value.imag());
  }
};

} // namespace fmt
