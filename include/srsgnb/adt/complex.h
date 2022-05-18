/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_ADT_COMPLEX_H
#define SRSGNB_ADT_COMPLEX_H

#include "srsgnb/srslog/bundled/fmt/format.h"
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
  std::string format_str = "{:+f}{:+f}j";

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    // Skip if context is empty and use default format.
    if (ctx.begin() == ctx.end()) {
      return ctx.end();
    }

    // Store the format string.
    format_str = "{:";
    for (auto& it : ctx) {
      format_str.push_back(it);

      // Found the end of the context.
      if (it == '}') {
        // Replicate the format string for the imaginary part.
        format_str += format_str + "j";
        return &it;
      }
    }

    // No end of context was found.
    return ctx.end();
  }

  template <typename FormatContext>
  auto format(srsgnb::cf_t value, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), format_str, value.real(), value.imag());
  }
};

} // namespace fmt

#endif // SRSGNB_ADT_COMPLEX_H