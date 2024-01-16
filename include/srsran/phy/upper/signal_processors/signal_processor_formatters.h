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

/// \file
/// \brief Formatters for signal processor related structures.

#pragma once

#include "srsran/phy/support/precoding_formatters.h"
#include "srsran/phy/upper/signal_processors/nzp_csi_rs_generator.h"
#include "srsran/support/format_utils.h"

namespace fmt {
/// \brief Custom formatter for \c nzp_csi_rs_generator::config_t.
template <>
struct formatter<srsran::nzp_csi_rs_generator::config_t> {
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
  auto format(const srsran::nzp_csi_rs_generator::config_t& nzp_csi_config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "prb=[{}, {})", nzp_csi_config.start_rb, nzp_csi_config.start_rb + nzp_csi_config.nof_rb);
    helper.format_always(ctx, "row={}", nzp_csi_config.csi_rs_mapping_table_row);
    helper.format_always(ctx, "Ki={}", srsran::span<const unsigned>(nzp_csi_config.freq_allocation_ref_idx));
    helper.format_always(ctx, "l0={}", nzp_csi_config.symbol_l0);
    helper.format_always(ctx, "l1={}", nzp_csi_config.symbol_l1);
    helper.format_always(ctx, "CDM={}", to_string(nzp_csi_config.cdm));
    helper.format_always(ctx, "density={}", to_string(nzp_csi_config.freq_density));
    helper.format_if_verbose(ctx, "slot={}", nzp_csi_config.slot);
    helper.format_if_verbose(ctx, "cp={}", nzp_csi_config.cp.to_string());
    helper.format_if_verbose(ctx, "sc_id={}", nzp_csi_config.scrambling_id);
    helper.format_if_verbose(ctx, "amplitude={}", nzp_csi_config.amplitude);
    helper.format_if_verbose(ctx, "precoding={}", nzp_csi_config.precoding);

    return ctx.out();
  }
};

} // namespace fmt