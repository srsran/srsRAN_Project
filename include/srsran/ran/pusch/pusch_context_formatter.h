/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once
#include "srsran/ran/pusch/pusch_context.h"
#include "srsran/support/format_utils.h"

namespace fmt {

/// \brief Custom formatter for \c pusch_context.
template <>
struct formatter<srsran::pusch_context> {
public:
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pusch_context& context, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "rnti=0x{:04x}", context.rnti);
    helper.format_always(ctx, "h_id={}", context.h_id);
    return ctx.out();
  }

private:
  srsran::delimited_formatter helper;
};

} // namespace fmt