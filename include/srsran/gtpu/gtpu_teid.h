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
constexpr gtpu_teid_t int_to_gtpu_teid(uint32_t teid_val)
{
  return gtpu_teid_t{teid_val};
}

/// \brief TEID for path management messages via GTP-U, e.g. echo request, echo response,...
constexpr gtpu_teid_t GTPU_PATH_MANAGEMENT_TEID = int_to_gtpu_teid(0);

/// \brief Smallest TEID value that is not used for path management.
constexpr gtpu_teid_t GTPU_TEID_MIN = int_to_gtpu_teid(1);

/// \brief Largest TEID value that is not used for path management.
constexpr gtpu_teid_t GTPU_TEID_MAX = int_to_gtpu_teid(std::numeric_limits<uint32_t>::max());

} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::gtpu_teid_t> : public formatter<std::string> {
  template <typename FormatContext>
  auto format(const srsran::gtpu_teid_t& t, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "{:#08x}", t.value());
  }
};

} // namespace fmt
