/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/strong_type.h"
#include "fmt/format.h"

namespace srsran {

/// \brief GTP-U Tunnel Identifier.
struct gtpu_teid_tag {};
using gtpu_teid_t =
    strong_type<uint32_t, struct gtpu_teid_tag, strong_equality, strong_equality_with<uint32_t>, strong_comparison>;

struct gtpu_teid_hasher_t {
  std::size_t operator()(const gtpu_teid_t& teid) const { return std::hash<uint32_t>()(teid.value()); }
};

struct gtpu_teid_compare_t {
  bool operator()(const gtpu_teid_t& lhs, const gtpu_teid_t& rhs) const
  {
    return std::less<>{}(lhs.value(), rhs.value());
  }
};

/// \brief Convert integer to GTP-U TEID value.
inline gtpu_teid_t int_to_gtpu_teid(uint32_t teid_val)
{
  return gtpu_teid_t{teid_val};
}

} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::gtpu_teid_t> : public formatter<std::string> {
  template <typename FormatContext>
  auto format(const srsran::gtpu_teid_t& t, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{:#08x}", t.value());
  }
};

} // namespace fmt
