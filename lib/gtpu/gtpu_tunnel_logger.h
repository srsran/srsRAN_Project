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

#include "srsran/cu_up/cu_up_types.h"
#include "srsran/gtpu/gtpu_teid.h"
#include "srsran/support/format/fmt_optional.h"
#include "srsran/support/format/fmt_to_c_str.h"
#include "srsran/support/format/prefixed_logger.h"
#include "fmt/format.h"

namespace srsran {

class gtpu_tunnel_log_prefix
{
public:
  gtpu_tunnel_log_prefix(std::optional<srs_cu_up::ue_index_t> ue_index, gtpu_teid_t teid, const char* dir)
  {
    fmt::memory_buffer buffer;
    if (ue_index.has_value()) {
      fmt::format_to(buffer, "ue={} {} teid={}: ", ue_index, dir, teid);
    } else {
      fmt::format_to(buffer, "{} teid={}: ", dir, teid);
    }
    prefix = srsran::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

using gtpu_tunnel_logger = prefixed_logger<gtpu_tunnel_log_prefix>;

} // namespace srsran

namespace fmt {

// associated formatter
template <>
struct formatter<srsran::gtpu_tunnel_log_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::gtpu_tunnel_log_prefix o, FormatContext& ctx)
  {
    return format_to(ctx.out(), "{}", o.to_c_str());
  }
};
} // namespace fmt
