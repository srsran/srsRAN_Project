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

#include "srsran/ran/pdcch/pdcch_context.h"

namespace fmt {

/// \brief Custom formatter for \c pdcch_context.
template <>
struct formatter<srsran::pdcch_context> {
public:
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pdcch_context& context, FormatContext& ctx) const
  {
    helper.format_always(ctx, "ss_id={}", fmt::underlying(context.ss_id));
    helper.format_always(ctx, "format={}", context.dci_format);
    if (context.harq_feedback_timing.has_value()) {
      helper.format_if_verbose(ctx, "harq_feedback_timing={}", context.harq_feedback_timing.value());
    }
    return ctx.out();
  }

private:
  srsran::delimited_formatter helper;
};

} // namespace fmt
