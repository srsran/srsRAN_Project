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
#include <cstdint>
#include <type_traits>

namespace srsran {

/// \remark See TS 38.331 - RNTI-Value and TS 38.321, Table 7.1-1: RNTI Values. Values: (0..65535)
enum class rnti_t : uint16_t {
  INVALID_RNTI = 0x0,
  MIN_CRNTI    = 0x1,
  // ...
  MAX_CRNTI         = 0xffef,
  MIN_RESERVED_RNTI = 0xfff0,
  // ...
  MAX_RESERVED_RNTI = 0xfffd,
  P_RNTI            = 0xfffe,
  SI_RNTI           = 0xffff
};

/// Checks whether RNTI value corresponds to a C-RNTI value.
constexpr bool is_crnti(rnti_t rnti)
{
  return rnti <= rnti_t::MAX_CRNTI and rnti >= rnti_t::MIN_CRNTI;
}

/// Converts integer to RNTI value.
constexpr rnti_t to_rnti(std::underlying_type_t<rnti_t> number)
{
  return static_cast<rnti_t>(number);
}

/// Converts RNTI value to integer.
constexpr inline uint16_t to_value(rnti_t rnti)
{
  return static_cast<uint16_t>(rnti);
}

} // namespace srsran

// Formatters
namespace fmt {
template <>
struct formatter<srsran::rnti_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rnti_t rnti, FormatContext& ctx)
  {
    return format_to(ctx.out(), "{:#x}", to_value(rnti));
  }
};

} // namespace fmt
