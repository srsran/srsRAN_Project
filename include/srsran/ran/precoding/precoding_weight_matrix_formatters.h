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

#include "srsran/ran/precoding/precoding_weight_matrix.h"
#include "srsran/support/format/delimited_formatter.h"

namespace fmt {

/// \brief Custom formatter for \c precoding_configuration
template <>
struct formatter<srsran::precoding_weight_matrix> {
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
  auto format(const srsran::precoding_weight_matrix& matrix, FormatContext& ctx)

  {
    unsigned nof_layers = matrix.get_nof_layers();
    unsigned nof_ports  = matrix.get_nof_ports();

    format_to(ctx.out(), "[");
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      if (i_layer != 0) {
        format_to(ctx.out(), ";");
      }
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        if (i_port != 0) {
          format_to(ctx.out(), ",");
        }
        format_to(ctx.out(), "{:+.2f}", matrix.get_coefficient(i_layer, i_port));
      }
    }
    format_to(ctx.out(), "]");

    return ctx.out();
  }
};

} // namespace fmt
