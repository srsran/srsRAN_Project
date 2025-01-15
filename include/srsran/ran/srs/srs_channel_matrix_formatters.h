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

#include "srsran/ran/srs/srs_channel_matrix.h"

namespace fmt {

/// \brief Custom formatter for \c srs_channel_matrix
template <>
struct formatter<srsran::srs_channel_matrix> {
  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::srs_channel_matrix& matrix, FormatContext& ctx) const
  {
    unsigned nof_rx_ports = matrix.get_nof_rx_ports();
    unsigned nof_tx_ports = matrix.get_nof_tx_ports();

    format_to(ctx.out(), "[");
    for (unsigned i_rx_port = 0; i_rx_port != nof_rx_ports; ++i_rx_port) {
      if (i_rx_port != 0) {
        format_to(ctx.out(), ";");
      }
      for (unsigned i_tx_port = 0; i_tx_port != nof_tx_ports; ++i_tx_port) {
        if (i_tx_port != 0) {
          format_to(ctx.out(), ",");
        }
        format_to(ctx.out(), "{:+.3f}", matrix.get_coefficient(i_rx_port, i_tx_port));
      }
    }
    format_to(ctx.out(), "]");

    return ctx.out();
  }
};

} // namespace fmt
