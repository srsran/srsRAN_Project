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
#include "srsgnb/ran/pdcch/pdcch_context.h"
#include "srsgnb/support/format_utils.h"

namespace fmt {

/// \brief Custom formatter for \c pdcch_context.
template <>
struct formatter<srsran::pdcch_context> {
public:
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pdcch_context& context, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "ss_id={}", context.ss_id);
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