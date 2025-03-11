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
