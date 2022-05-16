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
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }
  template <typename FormatContext>
  auto format(srsgnb::cf_t value, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{:+f}{:+f}j", value.real(), value.imag());
  }
};

} // namespace fmt

#endif // SRSGNB_ADT_COMPLEX_H