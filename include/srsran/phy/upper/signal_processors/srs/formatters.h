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

#include "srsran/phy/upper/signal_processors/srs/srs_estimator_configuration.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_result.h"
#include "srsran/ran/resource_allocation/rb_interval.h"
#include "srsran/ran/srs/srs_channel_matrix.h"
#include "srsran/ran/srs/srs_channel_matrix_formatters.h"
#include "srsran/ran/srs/srs_context_formatter.h"
#include "srsran/ran/srs/srs_information.h"
#include "srsran/ran/srs/srs_resource_formatter.h"
#include <limits>

namespace fmt {

/// \brief Custom formatter for \c srsran::srs_estimator_configuration.
template <>
struct formatter<srsran::srs_estimator_configuration> {
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
  auto format(const srsran::srs_estimator_configuration& config, FormatContext& ctx) const
  {
    srsran::bounded_bitset<srsran::NOF_SUBCARRIERS_PER_RB> re_mask(srsran::NOF_SUBCARRIERS_PER_RB);
    srsran::srs_information                                info = {};
    for (unsigned i_antenna_port = 0, nof_antennas = static_cast<unsigned>(config.resource.nof_antenna_ports);
         i_antenna_port != nof_antennas;
         ++i_antenna_port) {
      info = srsran::get_srs_information(config.resource, i_antenna_port);

      unsigned initial_re      = info.mapping_initial_subcarrier % srsran::NOF_SUBCARRIERS_PER_RB;
      unsigned nof_srs_per_prb = srsran::NOF_SUBCARRIERS_PER_RB / info.comb_size;
      for (unsigned i = 0; i != nof_srs_per_prb; ++i) {
        re_mask.set(i * info.comb_size + initial_re);
      }
    }

    srsran::crb_interval rb_range = {info.mapping_initial_subcarrier / srsran::NOF_SUBCARRIERS_PER_RB,
                                     info.sequence_length * info.comb_size / srsran::NOF_SUBCARRIERS_PER_RB};
    if (config.context) {
      helper.format_always(ctx, "{}", *config.context);
    }
    helper.format_if_verbose(ctx, "slot={}", config.slot);
    helper.format_always(ctx, "crb={}", rb_range);
    helper.format_always(ctx, "re={{{:n}}}", re_mask);
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
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::srs_estimator_result& config, FormatContext& ctx) const
  {
    helper.format_always(ctx, "t_align={:+.1f}ns", config.time_alignment.time_alignment * 1e9);
    helper.format_always(ctx, "epre={:+.1f}dB", config.epre_dB.value_or(std::numeric_limits<float>::quiet_NaN()));
    helper.format_always(ctx, "rsrp={:+.1f}dB", config.rsrp_dB.value_or(std::numeric_limits<float>::quiet_NaN()));
    helper.format_always(
        ctx,
        "noise_var={:+.1f}dB",
        srsran::convert_power_to_dB(config.noise_variance.value_or(std::numeric_limits<float>::quiet_NaN())));

    // Get matrix Frobenius norm.
    float frobenius_norm = config.channel_matrix.frobenius_norm();

    if (std::isnormal(frobenius_norm)) {
      // Normalize matrix.
      srsran::srs_channel_matrix norm_matrix = config.channel_matrix;
      norm_matrix *= 1.0F / frobenius_norm;

      // Print norm and matrix.
      helper.format_if_verbose(ctx, "H={:.3e} * {}", frobenius_norm, norm_matrix);
    } else {
      // Do not print anything if there are no coefficients.
      helper.format_if_verbose(ctx, "H=[]");
    }

    return ctx.out();
  }
};

} // namespace fmt
