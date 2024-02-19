/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "srsran/ran/srs/srs_channel_matrix.h"
#include "srsran/support/format_utils.h"

namespace fmt {

/// \brief Custom formatter for \c srs_channel_matrix
template <>
struct formatter<srsran::srs_channel_matrix> {
  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::srs_channel_matrix& matrix, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
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
        format_to(ctx.out(), "{:+.2f}", matrix.get_coefficient(i_rx_port, i_tx_port));
      }
    }
    format_to(ctx.out(), "]");

    return ctx.out();
  }
};

} // namespace fmt
