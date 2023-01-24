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

#include "srsgnb/ran/lcid.h"
#include "srsgnb/support/prefixed_logger.h"
#include <string.h>

namespace srsgnb {

class rlc_bearer_log_prefix
{
public:
  rlc_bearer_log_prefix(uint32_t ue_index, rb_id_t rb_id)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(buffer, "UE={}, {}: ", ue_index, rb_id);
    prefix = srsgnb::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

using rlc_bearer_logger = prefixed_logger<rlc_bearer_log_prefix>;

} // namespace srsgnb

namespace fmt {

// associated formatter
template <>
struct formatter<srsgnb::rlc_bearer_log_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::rlc_bearer_log_prefix o, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", o.to_c_str());
  }
};
} // namespace fmt
