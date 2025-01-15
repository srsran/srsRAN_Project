/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_up/cu_up_types.h"
#include "srsran/support/format/prefixed_logger.h"
#include "fmt/format.h"

namespace srsran {
namespace srs_cu_up {

class cu_up_log_prefix
{
public:
  cu_up_log_prefix(ue_index_t ue_index)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer), "ue={}: ", fmt::underlying(ue_index));
    prefix = srsran::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

using cu_up_ue_logger = prefixed_logger<cu_up_log_prefix>;

} // namespace srs_cu_up
} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::srs_cu_up::cu_up_log_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::srs_cu_up::cu_up_log_prefix& o, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "{}", o.to_c_str());
  }
};

} // namespace fmt
