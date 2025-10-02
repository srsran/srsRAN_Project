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

#include "srsran/ran/srs/srs_context.h"
#include "srsran/support/format/delimited_formatter.h"

namespace fmt {

/// \brief Custom formatter for \c srs_context.
template <>
struct formatter<srsran::srs_context> {
public:
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::srs_context& context, FormatContext& ctx) const
  {
    helper.format_always(ctx, "sector_id={}", context.sector_id);
    helper.format_always(ctx, "rnti={}", context.rnti);
    return ctx.out();
  }

private:
  srsran::delimited_formatter helper;
};

} // namespace fmt
