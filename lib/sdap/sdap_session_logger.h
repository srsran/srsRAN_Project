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

#include "srsran/adt/optional.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/support/prefixed_logger.h"
#include "fmt/format.h"

namespace srsran {

class sdap_session_trx_log_prefix
{
public:
  sdap_session_trx_log_prefix(uint32_t                ue_index,
                              pdu_session_id_t        psi,
                              optional<qos_flow_id_t> qfi,
                              drb_id_t                drb_id,
                              const char*             dir)
  {
    fmt::memory_buffer buffer;
    if (qfi.has_value()) {
      fmt::format_to(buffer, "ue={} psi={} {} {} {}: ", ue_index, psi, qfi.value(), drb_id, dir);
    } else {
      fmt::format_to(buffer, "ue={} psi={} {} {}: ", ue_index, psi, drb_id, dir);
    }
    prefix = srsran::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

using sdap_session_trx_logger = prefixed_logger<sdap_session_trx_log_prefix>;

class sdap_session_log_prefix
{
public:
  sdap_session_log_prefix(uint32_t ue_index, pdu_session_id_t psi)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(buffer, "ue={} psi={}: ", ue_index, psi);
    prefix = srsran::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

using sdap_session_logger = prefixed_logger<sdap_session_log_prefix>;

} // namespace srsran

namespace fmt {

// associated formatters
template <>
struct formatter<srsran::sdap_session_trx_log_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::sdap_session_trx_log_prefix o, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", o.to_c_str());
  }
};

// associated formatters
template <>
struct formatter<srsran::sdap_session_log_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::sdap_session_log_prefix o, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", o.to_c_str());
  }
};

} // namespace fmt
