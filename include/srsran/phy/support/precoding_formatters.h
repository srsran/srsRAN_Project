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
#include "srsran/phy/support/precoding_configuration.h"
#include "srsran/ran/precoding/precoding_weight_matrix_formatters.h"

namespace fmt {

/// \brief Custom formatter for \c precoding_configuration
template <>
struct formatter<srsran::precoding_configuration> {
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
  auto format(const srsran::precoding_configuration& config, FormatContext& ctx)

  {
    format_to(ctx.out(), "prg_size={} ", config.get_prg_size());

    unsigned nof_prg = config.get_nof_prg();

    for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
      if (i_prg != 0) {
        format_to(ctx.out(), " ");
      }
      format_to(ctx.out(), "prg{}={}", i_prg, config.get_prg_coefficients(i_prg));
    }

    return ctx.out();
  }
};

} // namespace fmt