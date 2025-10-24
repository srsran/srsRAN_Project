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

#include "srsran/phy/upper/channel_processors/prach_detector.h"
#include "srsran/support/format/delimited_formatter.h"

namespace fmt {

/// \brief Custom formatter for \c prach_detector::configuration.
template <>
struct formatter<srsran::prach_detector::configuration> {
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
  auto format(const srsran::prach_detector::configuration& config, FormatContext& ctx) const
  {
    helper.format_always(ctx, "rsi={}", config.root_sequence_index);
    helper.format_if_verbose(ctx,
                             "preambles=[{}, {})",
                             config.start_preamble_index,
                             config.start_preamble_index + config.nof_preamble_indices);
    helper.format_if_verbose(ctx, "format={}", to_string(config.format));
    helper.format_if_verbose(ctx, "set={}", to_string(config.restricted_set));
    helper.format_if_verbose(ctx, "zcz={}", config.zero_correlation_zone);
    helper.format_if_verbose(ctx, "scs={}", to_string(config.ra_scs));
    helper.format_if_verbose(ctx, "nof_rx_ports={}", config.nof_rx_ports);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c prach_detection_result::preamble_indication.
template <>
struct formatter<srsran::prach_detection_result::preamble_indication> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::prach_detection_result::preamble_indication& preamble, FormatContext& ctx) const
  {
    format_to(ctx.out(),
              "{{idx={} ta={:.2f}us detection_metric={:.1f} power_dB={:.2f}}}",
              preamble.preamble_index,
              preamble.time_advance.to_seconds() * 1e6,
              preamble.detection_metric,
              preamble.preamble_power_dB);
    return ctx.out();
  }
};

/// \brief Custom formatter for \c prach_detection_result.
template <>
struct formatter<srsran::prach_detection_result> {
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
  auto format(const srsran::prach_detection_result& result, FormatContext& ctx) const
  {
    helper.format_always(ctx, "rssi={:+.1f}dB", result.rssi_dB);
    helper.format_if_verbose(ctx, "res={:.1f}us", result.time_resolution.to_seconds() * 1e6);
    helper.format_if_verbose(ctx, "max_ta={:.2f}us", result.time_advance_max.to_seconds() * 1e6);
    helper.format_always(ctx,
                         "detected_preambles=[{:,}]",
                         srsran::span<const srsran::prach_detection_result::preamble_indication>(result.preambles));

    return ctx.out();
  }
};

} // namespace fmt
