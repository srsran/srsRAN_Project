/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "fmt/format.h"
#include <complex>

namespace srsran {

namespace detail {

template <typename SrcType, typename DstType>
std::complex<DstType> convert(std::complex<SrcType> value)
{
  return {static_cast<DstType>(value.real()), static_cast<DstType>(value.imag())};
}

} // namespace detail

/// Type to store complex samples.
using cf_t = std::complex<float>;

using ci8_t = std::complex<int8_t>;

using ci16_t = std::complex<int16_t>;

inline ci8_t to_ci8(cf_t value)
{
  return detail::convert<float, int8_t>(value);
}

inline ci16_t to_ci16(cf_t value)
{
  return detail::convert<float, int16_t>(value);
}

inline cf_t to_cf(ci8_t value)
{
  return detail::convert<int8_t, float>(value);
}

inline cf_t to_cf(cf_t value)
{
  return value;
}

inline cf_t to_cf(ci16_t value)
{
  return detail::convert<int16_t, float>(value);
}

/// Checks if T is compatible with a complex floating point.
template <typename T>
struct is_complex : std::false_type {
};

template <typename T>
struct is_complex<std::complex<T>> : std::true_type {
};

template <typename T>
struct is_complex<const std::complex<T>> : std::true_type {
};

} // namespace srsran

namespace fmt {

/// FMT formatter of cf_t type.
template <typename ComplexType>
struct formatter_template {
  // Stores parsed format string.
  memory_buffer format_buffer;

  formatter_template()
  {
    static const string_view DEFAULT_FORMAT =
        (std::is_same<ComplexType, srsran::cf_t>::value) ? "{:+f}{:+f}j" : "{:+d}{:+d}j";
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
  auto format(ComplexType value, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    const string_view format_str = string_view(format_buffer.data(), format_buffer.size());
    return format_to(ctx.out(), format_str, value.real(), value.imag());
  }
};

template <>
struct formatter<srsran::cf_t> : public formatter_template<srsran::cf_t> {
};
template <>
struct formatter<srsran::ci8_t> : public formatter_template<srsran::ci8_t> {
};
template <>
struct formatter<srsran::ci16_t> : public formatter_template<srsran::ci16_t> {
};

} // namespace fmt
