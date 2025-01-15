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

#include "srsran/phy/upper/channel_state_information.h"
#include "srsran/support/format/delimited_formatter.h"
#include <fmt/core.h>

namespace fmt {

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
        helper.format_if_verbose(ctx, "sinr_ch_est{}={:.1f}dB", selected, csi.sinr_ch_estimator_dB.value());
      }

      if (csi.sinr_post_eq_dB.has_value()) {
        const char* selected =
            (csi.sinr_report_type == srsran::channel_state_information::sinr_type::post_equalization) ? "[sel]" : "";
        helper.format_if_verbose(ctx, "sinr_eq{}={:.1f}dB", selected, csi.sinr_post_eq_dB.value());
      }

      if (csi.sinr_evm_dB.has_value()) {
        const char* selected =
            (csi.sinr_report_type == srsran::channel_state_information::sinr_type::evm) ? "[sel]" : "";
        helper.format_if_verbose(ctx, "sinr_evm{}={:.1f}dB", selected, csi.sinr_evm_dB.value());
      }

      if (csi.evm.has_value()) {
        helper.format_if_verbose(ctx, "evm={:.2f}", csi.evm.value());
      }

      // Extract CSI measurements. It could be some of them are not available.
      std::optional<float>                 epre_dB       = csi.get_epre_dB();
      std::optional<float>                 rsrp_dB       = csi.get_rsrp_dB();
      std::optional<float>                 sinr_dB       = csi.get_sinr_dB();
      std::optional<srsran::phy_time_unit> time_aligment = csi.get_time_alignment();
      std::optional<float>                 cfo_Hz        = csi.get_cfo_Hz();

      // Print the measurements that are present.
      if (epre_dB.has_value()) {
        helper.format_always(ctx, "epre={:+.1f}dB", epre_dB.value());
      } else {
        helper.format_always(ctx, "epre=na");
      }
      if (rsrp_dB.has_value()) {
        helper.format_always(ctx, "rsrp={:+.1f}dB", rsrp_dB.value());
      } else {
        helper.format_always(ctx, "rsrp=na");
      }
      if (sinr_dB.has_value()) {
        helper.format_if_verbose(ctx, "sinr={:+.1f}dB", sinr_dB.value());
      } else {
        helper.format_if_verbose(ctx, "sinr=na");
      }
      if (time_aligment.has_value()) {
        helper.format_if_verbose(ctx, "t_align={:.2f}us", time_aligment.value().to_seconds() * 1e6);
      } else {
        helper.format_if_verbose(ctx, "t_align=na");
      }
      if (cfo_Hz.has_value()) {
        helper.format_if_verbose(ctx, "cfo={:+.1f}Hz", cfo_Hz.value());
      } else {
        helper.format_if_verbose(ctx, "cfo=na");
      }
    } else {
      // Short representation only prints the SINR selected for CSI reporting to higher layers.
      std::optional<float> sinr_dB = csi.get_sinr_dB();
      if (sinr_dB.has_value()) {
        helper.format_always(ctx, "sinr={:.1f}dB", sinr_dB.value());
      } else {
        // SINR is not available.
        helper.format_always(ctx, "sinr=na");
      }
    }
    return ctx.out();
  }
};

} // namespace fmt
