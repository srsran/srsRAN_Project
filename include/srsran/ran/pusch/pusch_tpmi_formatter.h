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

#include "srsran/ran/pusch/pusch_tpmi_select.h"
#include "srsran/support/format/delimited_formatter.h"

namespace fmt {

/// \brief Custom formatter for \c pusch_tpmi_select_info.
template <>
struct formatter<srsran::pusch_tpmi_select_info> {
public:
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pusch_tpmi_select_info& context, FormatContext& ctx) const
  {
    unsigned max_nof_layers = context.get_max_nof_layers();
    for (unsigned i_nof_layers = 1; i_nof_layers <= max_nof_layers; ++i_nof_layers) {
      const auto& info = context.get_tpmi_select(i_nof_layers);
      helper.format_always(ctx, "{}_layer: tpmi={} sinr={:+.1f}dB", i_nof_layers, info.tpmi, info.sinr_dB);
    }

    return ctx.out();
  }

private:
  srsran::delimited_formatter helper;
};

} // namespace fmt
