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
#include "srsran/ran/pdsch/pdsch_context.h"
#include "srsran/support/format/delimited_formatter.h"

namespace fmt {

/// \brief Custom formatter for \c pdsch_context.
template <>
struct formatter<srsran::pdsch_context> {
public:
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pdsch_context& context, FormatContext& ctx)
  {
    helper.format_always(ctx, "h_id={}", context.h_id);
    helper.format_always(ctx, "k1={}", context.k1);
    return ctx.out();
  }

private:
  srsran::delimited_formatter helper;
};

} // namespace fmt