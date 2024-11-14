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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/rnti.h"
#include "srsran/support/format/fmt_to_c_str.h"
#include "srsran/support/format/prefixed_logger.h"
#include "fmt/format.h"
#include <string.h>

namespace srsran {
namespace srs_cu_cp {

class rrc_ue_log_prefix
{
public:
  rrc_ue_log_prefix(ue_index_t ue_index, rnti_t c_rnti)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(buffer, "ue={} c-rnti={}: ", ue_index, c_rnti);
    prefix = srsran::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

using rrc_ue_logger = prefixed_logger<rrc_ue_log_prefix>;

} // namespace srs_cu_cp
} // namespace srsran

namespace fmt {

// associated formatter
template <>
struct formatter<srsran::srs_cu_cp::rrc_ue_log_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::srs_cu_cp::rrc_ue_log_prefix o, FormatContext& ctx)

  {
    return format_to(ctx.out(), "{}", o.to_c_str());
  }
};
} // namespace fmt
