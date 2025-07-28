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

#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/csi_report/csi_report_size.h"
#include "srsran/ran/uci/uci_formatters.h"
#include "fmt/std.h"

/// Custom formatter for \c srsran::csi_report_configuration.
template <>
struct fmt::formatter<srsran::csi_report_configuration> {
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
  auto format(const srsran::csi_report_configuration& config, FormatContext& ctx) const
  {
    helper.format_always(ctx, "nof_csi_rs_resources={}", config.nof_csi_rs_resources);
    helper.format_always(ctx, "pmi_codebook={}", to_string(config.pmi_codebook));
    helper.format_always(ctx, "ri_restriction={}", config.ri_restriction);

    helper.format_always(ctx, "quantities={}", to_string(config.quantities));

    return ctx.out();
  }
};

/// Custom formatter for \c srsran::csi_report_pmi.
template <>
struct fmt::formatter<srsran::csi_report_pmi> {
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
  auto format(const srsran::csi_report_pmi& pmi, FormatContext& ctx) const
  {
    if (const auto* two_ports_pmi = std::get_if<srsran::csi_report_pmi::two_antenna_port>(&pmi.type)) {
      helper.format_always(ctx, "pmi={}", two_ports_pmi->pmi);
    } else if (const auto* four_ports_pmi =
                   std::get_if<srsran::csi_report_pmi::typeI_single_panel_4ports_mode1>(&pmi.type)) {
      helper.format_always(ctx, "i_1_1={}", four_ports_pmi->i_1_1);
      helper.format_always(ctx, "i_1_3={}", four_ports_pmi->i_1_3);
      helper.format_always(ctx, "i_2={}", four_ports_pmi->i_2);
    }

    return ctx.out();
  }
};

/// Custom formatter for \c srsran::csi_report_data.
template <>
struct fmt::formatter<srsran::csi_report_data> {
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
  auto format(const srsran::csi_report_data& data, FormatContext& ctx) const
  {
    if (data.cri.has_value()) {
      helper.format_always(ctx, "cri={}", data.cri.value());
    }

    if (data.ri.has_value()) {
      helper.format_always(ctx, "ri={}", data.ri.value());
    }

    if (data.li.has_value()) {
      helper.format_always(ctx, "li={}", data.li.value());
    }

    if (data.pmi.has_value()) {
      helper.format_always(ctx, "{}", data.pmi.value());
    }

    if (data.first_tb_wideband_cqi.has_value()) {
      helper.format_always(ctx, "cqi1={}", data.first_tb_wideband_cqi.value());
    }

    if (data.second_tb_wideband_cqi.has_value()) {
      helper.format_always(ctx, "cqi2={}", data.second_tb_wideband_cqi.value());
    }

    return ctx.out();
  }
};

/// Custom formatter for \c srsran::csi_report_pusch_size.
template <>
struct fmt::formatter<srsran::csi_report_size> {
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
  auto format(const srsran::csi_report_size& data, FormatContext& ctx) const
  {
    helper.format_always(ctx, "part1={}", data.part1_size.value());
    helper.format_always(ctx, "part2={{{}}}", data.part2_correspondence);
    helper.format_always(ctx, "part2_min_size={}", data.part2_min_size);
    helper.format_always(ctx, "part2_max_size={}", data.part2_max_size);
    return ctx.out();
  }
};
