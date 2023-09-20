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

#include "srsran/ran/lcid.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/support/prefixed_logger.h"
#include "fmt/format.h"
#include <string.h>

namespace srsran {
namespace srs_cu_up {

class f1u_bearer_log_prefix
{
public:
  f1u_bearer_log_prefix(uint32_t ue_index, drb_id_t drb_id, const up_transport_layer_info& ul_tnl_info)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(buffer, "ue={} {} ul-teid={}: ", ue_index, drb_id, ul_tnl_info.gtp_teid);
    prefix = srsran::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

using f1u_bearer_logger = prefixed_logger<f1u_bearer_log_prefix>;

} // namespace srs_cu_up
} // namespace srsran

namespace fmt {

// associated formatter
template <>
struct formatter<srsran::srs_cu_up::f1u_bearer_log_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::srs_cu_up::f1u_bearer_log_prefix o, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", o.to_c_str());
  }
};
} // namespace fmt
