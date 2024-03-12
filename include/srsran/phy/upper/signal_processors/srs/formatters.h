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

#include "srsran/phy/upper/signal_processors/srs/srs_estimator_configuration.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_result.h"
#include "srsran/ran/srs/srs_channel_matrix.h"
#include "srsran/ran/srs/srs_channel_matrix_formatters.h"
#include "srsran/ran/srs/srs_resource_formatter.h"
#include "srsran/support/format_utils.h"

namespace fmt {

/// \brief Custom formatter for \c srsran::srs_estimator_configuration.
template <>
struct formatter<srsran::srs_estimator_configuration> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::srs_estimator_configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_if_verbose(ctx, "slot={}", config.slot);
    helper.format_always(ctx, "{}", config.resource);
    helper.format_if_verbose(ctx, "ports=[{}]", srsran::span<const uint8_t>(config.ports));

    return ctx.out();
  }
};

/// \brief Custom formatter for \c srsran::srs_estimator_result.
template <>
struct formatter<srsran::srs_estimator_result> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::srs_estimator_result& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "t_align={:.1}us", config.time_alignment.time_alignment * 1e6);
    helper.format_always(ctx, "noise_var={}", config.noise_variance);
    helper.format_if_verbose(ctx, "H={}", config.channel_matrix);

    return ctx.out();
  }
};

} // namespace fmt