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

#include "srsran/ran/pusch/pusch_context.h"

namespace fmt {

/// \brief Custom formatter for \c pusch_context.
template <>
struct formatter<srsran::pusch_context> {
public:
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pusch_context& context, FormatContext& ctx) const
  {
    helper.format_always(ctx, "rnti={}", context.rnti);
    helper.format_always(ctx, "h_id={}", fmt::underlying(context.h_id));
    return ctx.out();
  }

private:
  srsran::delimited_formatter helper;
};

} // namespace fmt
