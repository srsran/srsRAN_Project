/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
