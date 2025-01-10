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

namespace srsran {

/// \brief Global NodeB CU UP ID used to uniquely identify the gNB-CU-UP at least within a gNB-CU-CP.
/// \remark See TS 38.463 Section 9.3.1.15: GNB-CU-UP-ID valid values: (0..2^36-1).
enum class gnb_cu_up_id_t : uint64_t { min = 0, max = 68719476735, invalid = max + 1 };

/// Converts an integer to a GNB-CU-UP-ID type.
constexpr gnb_cu_up_id_t uint_to_gnb_cu_up_id(uint64_t id)
{
  return static_cast<gnb_cu_up_id_t>(id);
}

/// Converts a GNB-CU-UP-ID to an integer.
constexpr uint64_t gnb_cu_up_id_to_uint(gnb_cu_up_id_t gnb_cu_up_id)
{
  return static_cast<uint64_t>(gnb_cu_up_id);
}

} // namespace srsran

namespace fmt {

// gnb_cu_up_id_t formatter
template <>
struct formatter<srsran::gnb_cu_up_id_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::gnb_cu_up_id_t o, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "{}", srsran::gnb_cu_up_id_to_uint(o));
  }
};

} // namespace fmt
