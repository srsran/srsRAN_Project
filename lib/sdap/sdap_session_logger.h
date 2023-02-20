/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#pragma once

#include "srsran/ran/cu_types.h"
#include "srsran/support/prefixed_logger.h"
#include "fmt/format.h"

namespace srsran {

class sdap_session_log_prefix
{
public:
  sdap_session_log_prefix(uint32_t ue_index, pdu_session_id_t sid, const char* dir)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(buffer, "ue={} sid={} {}: ", ue_index, sid, dir);
    prefix = srsran::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

using sdap_session_logger = prefixed_logger<sdap_session_log_prefix>;

} // namespace srsran

namespace fmt {

// associated formatter
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
