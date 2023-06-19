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
#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/support/format_utils.h"

/// \brief Custom formatter for \c srsran::csi_report_configuration.
template <>
struct fmt::formatter<srsran::csi_report_configuration> {
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
  auto format(const srsran::csi_report_configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "nof_csi_rs_resources={}", config.nof_csi_rs_resources);
    helper.format_always(ctx, "pmi_codebook={}", to_string(config.pmi_codebook));
    helper.format_always(ctx, "ri_restriction={}", config.ri_restriction);

    helper.format_always(ctx, "quantities={}", to_string(config.quantities));

    return ctx.out();
  }
};

/// \brief Custom formatter for \c srsran::csi_report_pmi.
template <>
struct fmt::formatter<srsran::csi_report_pmi> {
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
  auto format(const srsran::csi_report_pmi& pmi, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    if (srsran::variant_holds_alternative<srsran::csi_report_pmi::two_antenna_port>(pmi.type)) {
      srsran::csi_report_pmi::two_antenna_port value =
          srsran::variant_get<srsran::csi_report_pmi::two_antenna_port>(pmi.type);
      helper.format_always(ctx, "pmi={}", value.pmi);
    }

    if (srsran::variant_holds_alternative<srsran::csi_report_pmi::typeI_single_panel_4ports_mode1>(pmi.type)) {
      srsran::csi_report_pmi::typeI_single_panel_4ports_mode1 value =
          srsran::variant_get<srsran::csi_report_pmi::typeI_single_panel_4ports_mode1>(pmi.type);
      helper.format_always(ctx, "i_1_1={}", value.i_1_1);
      helper.format_always(ctx, "i_1_2={}", value.i_1_2);
      helper.format_always(ctx, "i_1_3={}", value.i_1_3);
      helper.format_always(ctx, "i_2={}", value.i_2);
    }

    return ctx.out();
  }
};

/// \brief Custom formatter for \c srsran::csi_report_data.
template <>
struct fmt::formatter<srsran::csi_report_data> {
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
  auto format(const srsran::csi_report_data& data, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
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
