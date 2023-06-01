/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/ran/precoding/precoding_configuration.h"
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