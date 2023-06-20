/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once
#include "srsran/phy/support/precoding_configuration.h"
#include "srsran/support/format_utils.h"

namespace fmt {

/// \brief Custom formatter for \c precoding_configuration
template <>
struct formatter<srsran::precoding_configuration> {
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
  auto format(const srsran::precoding_configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    format_to(ctx.out(), "prg_size={} ", config.get_prg_size());

    unsigned nof_prg    = config.get_nof_prg();
    unsigned nof_layers = config.get_nof_layers();
    unsigned nof_ports  = config.get_nof_ports();

    format_to(ctx.out(), "[");
    for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
      format_to(ctx.out(), "prg{}=[", i_prg);
      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        format_to(ctx.out(), "layer{}=[", i_layer);
        for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
          format_to(ctx.out(), "{} ", config.get_coefficient(i_layer, i_port, i_prg));
        }
        format_to(ctx.out(), "] ");
      }
      format_to(ctx.out(), "] ");
    }
    format_to(ctx.out(), "]");

    return ctx.out();
  }
};

} // namespace fmt