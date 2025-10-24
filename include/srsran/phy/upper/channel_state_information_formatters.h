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

#include "srsran/adt/span.h"
#include "srsran/phy/upper/channel_state_information.h"
#include "srsran/support/format/delimited_formatter.h"
#include <fmt/core.h>

namespace fmt {

template <>
struct formatter<std::optional<float>> {
private:
  formatter<float> underlying_;

  template <class U>
  FMT_CONSTEXPR static auto maybe_set_debug_format(U& u, bool set) -> decltype(u.set_debug_format(set))
  {
    u.set_debug_format(set);
  }

  template <class U>
  FMT_CONSTEXPR static void maybe_set_debug_format(U&, ...)
  {
  }

public:
  template <typename ParseContext>
  FMT_CONSTEXPR auto parse(ParseContext& ctx)
  {
    maybe_set_debug_format(underlying_, true);
    return underlying_.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const std::optional<float>& opt, FormatContext& ctx) const -> decltype(ctx.out())
  {
    if (!opt) {
      return detail::write(ctx.out(), "na");
    }

    return underlying_.format(*opt, ctx);
  }
};

/// \brief Custom formatter for \c channel_state_information.
template <>
struct formatter<srsran::channel_state_information> {
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
  auto format(const srsran::channel_state_information& csi, FormatContext& ctx) const
  {
    // Verbose representation prints all available SINR parameters. It appends [sel] to the label of the selected
    // measurement point.
    if (helper.is_verbose()) {
      if (csi.sinr_ch_estimator_dB.has_value()) {
        const char* selected =
            (csi.sinr_report_type == srsran::channel_state_information::sinr_type::channel_estimator) ? "[sel]" : "";
        helper.format_if_verbose(ctx, "sinr_ch_est{}={:.1f}dB", selected, *csi.sinr_ch_estimator_dB);
      }

      if (csi.sinr_post_eq_dB.has_value()) {
        const char* selected =
            (csi.sinr_report_type == srsran::channel_state_information::sinr_type::post_equalization) ? "[sel]" : "";
        helper.format_if_verbose(ctx, "sinr_eq{}={:.1f}dB", selected, *csi.sinr_post_eq_dB);
      }

      if (csi.sinr_evm_dB.has_value()) {
        const char* selected =
            (csi.sinr_report_type == srsran::channel_state_information::sinr_type::evm) ? "[sel]" : "";
        helper.format_if_verbose(ctx, "sinr_evm{}={:.1f}dB", selected, *csi.sinr_evm_dB);
      }

      // Print EVM per a symbol basis only if it is present.
      auto total_evm  = csi.get_total_evm();
      auto symbol_evm = csi.get_symbol_evm();
      if (std::any_of(symbol_evm.begin(), symbol_evm.end(), [](auto elem) { return elem.has_value(); })) {
        helper.format_if_verbose(ctx, "evm=[{:.2f}]", symbol_evm);
      } else if (total_evm.has_value()) {
        helper.format_if_verbose(ctx, "evm={:.2f}", total_evm);
      }

      // Extract CSI measurements. It could be some of them are not available.
      std::optional<float>                 epre_dB       = csi.get_epre_dB();
      std::optional<float>                 sinr_dB       = csi.get_sinr_dB();
      std::optional<srsran::phy_time_unit> time_aligment = csi.get_time_alignment();
      std::optional<float>                 cfo_Hz        = csi.get_cfo_Hz();

      // Print the measurements that are present.
      if (epre_dB.has_value()) {
        helper.format_always(ctx, "epre={:+.1f}dB", *epre_dB);
      } else {
        helper.format_always(ctx, "epre=na");
      }

      // Print the RSRP for each port, if available. Otherwise, print the average RSRP.
      srsran::span<const float> port_rsrp = csi.get_port_rsrp_dB();
      if (std::any_of(port_rsrp.begin(), port_rsrp.end(), [](auto elem) { return !std::isnan(elem); })) {
        helper.format_if_verbose(ctx, "rsrp=[{:.1f}]dB", port_rsrp);
      } else {
        std::optional<float> rsrp_dB = csi.get_rsrp_dB();
        if (rsrp_dB.has_value()) {
          helper.format_if_verbose(ctx, "rsrp={:.1f}dB", rsrp_dB);
        } else {
          helper.format_if_verbose(ctx, "rsrp=na");
        }
      }

      if (sinr_dB.has_value()) {
        helper.format_if_verbose(ctx, "sinr={:+.1f}dB", *sinr_dB);
      } else {
        helper.format_if_verbose(ctx, "sinr=na");
      }
      if (time_aligment.has_value()) {
        helper.format_if_verbose(ctx, "t_align={:.2f}us", time_aligment->to_seconds() * 1e6);
      } else {
        helper.format_if_verbose(ctx, "t_align=na");
      }
      if (cfo_Hz.has_value()) {
        helper.format_if_verbose(ctx, "cfo={:+.1f}Hz", *cfo_Hz);
      } else {
        helper.format_if_verbose(ctx, "cfo=na");
      }
    } else {
      // Short representation only prints the SINR selected for CSI reporting to higher layers.
      std::optional<float> sinr_dB = csi.get_sinr_dB();
      if (sinr_dB.has_value()) {
        helper.format_always(ctx, "sinr={:.1f}dB", *sinr_dB);
      } else {
        // SINR is not available.
        helper.format_always(ctx, "sinr=na");
      }
    }
    return ctx.out();
  }
};

} // namespace fmt
