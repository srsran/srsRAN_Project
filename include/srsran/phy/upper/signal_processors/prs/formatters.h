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

#include "srsran/phy/support/precoding_formatters.h"
#include "srsran/phy/upper/signal_processors/prs/prs_generator_configuration.h"
#include "srsran/ran/precoding/precoding_weight_matrix_formatters.h"

namespace fmt {

/// \brief Custom formatter for \c pdcch_processor::coreset_description.
template <>
struct formatter<srsran::prs_generator_configuration> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::prs_generator_configuration& config, FormatContext& ctx) const
  {
    helper.format_if_verbose(ctx, "slot={}", config.slot);
    helper.format_if_verbose(ctx, "cp={}", config.cp);
    helper.format_always(ctx, "n_id={}", config.n_id_prs);
    helper.format_always(ctx, "comb_size={}", fmt::underlying(config.comb_size));
    helper.format_always(ctx, "comb_offset={}", config.comb_offset);
    helper.format_always(
        ctx,
        "t_alloc={}",
        srsran::interval<unsigned>(config.start_symbol, config.start_symbol + static_cast<unsigned>(config.duration)));
    helper.format_if_verbose(ctx, "rb_start={}", config.prb_start);
    helper.format_always(ctx, "f_alloc={}", config.freq_alloc);
    helper.format_if_verbose(ctx, "power_offset={}", config.power_offset_dB);
    helper.format_if_verbose(ctx, "precoding={}", config.precoding);

    return ctx.out();
  }
};

} // namespace fmt
