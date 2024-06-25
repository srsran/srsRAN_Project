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

#include "srsran/adt/bf16.h"
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

/// \brief Complex based on 16-bit floating point.
/// \note This type is meant for storage purposes only, no operations other than equality comparison are allowed.
struct cbf16_t {
  bf16_t real;
  bf16_t imag;

  cbf16_t(float real_ = 0.0F, float imag_ = 0.0F) : real(to_bf16(real_)), imag(to_bf16(imag_)) {}

  cbf16_t(cf_t value) : cbf16_t(value.real(), value.imag()) {}

  cbf16_t(std::complex<double> value) : real(to_bf16(value.real())), imag(to_bf16(value.imag())) {}

  bool operator==(cbf16_t other) const { return (real == other.real) && (imag == other.imag); }

  bool operator!=(cbf16_t other) const { return !(*this == other); }
};

inline ci8_t to_ci8(cf_t value)
{
  return detail::convert<float, int8_t>(value);
}

inline ci16_t to_ci16(cf_t value)
{
  return detail::convert<float, int16_t>(value);
}

inline cf_t to_cf(cbf16_t value)
{
  return {to_float(value.real), to_float(value.imag)};
}

inline cbf16_t to_cbf16(cf_t value)
{
  return cbf16_t(value);
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

/// FMT formatter of cbf16_t type.
template <>
struct formatter<srsran::cbf16_t> {
  formatter_template<srsran::cf_t> cf_formatter;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return cf_formatter.parse(ctx);
  }

  template <typename FormatContext>
  auto format(srsran::cbf16_t value, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return cf_formatter.format(srsran::to_cf(value), ctx);
  }
};

} // namespace fmt
