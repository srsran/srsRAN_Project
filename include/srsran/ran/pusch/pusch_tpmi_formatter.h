/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
      helper.format_always(ctx,
                           "{}_layer: tpmi={} sinr=({:+.1f})dB",
                           i_nof_layers,
                           info.tpmi,
                           srsran::span<const float>(info.sinr_dB_layer));
    }

    return ctx.out();
  }

private:
  srsran::delimited_formatter helper;
};

} // namespace fmt
