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

#include "srsran/phy/support/precoding_formatters.h"
#include "srsran/phy/upper/channel_processors/pdcch/pdcch_processor.h"
#include "srsran/ran/pdcch/pdcch_context_formatter.h"
#include "srsran/ran/precoding/precoding_weight_matrix_formatters.h"

namespace fmt {

/// \brief Custom formatter for \c pdcch_processor::coreset_description.
template <>
struct formatter<srsran::pdcch_processor::coreset_description> {
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
  auto format(const srsran::pdcch_processor::coreset_description& coreset, FormatContext& ctx) const
  {
    helper.format_always(ctx, "bwp=[{}, {})", coreset.bwp_start_rb, coreset.bwp_start_rb + coreset.bwp_size_rb);
    helper.format_always(
        ctx, "symb=[{}, {})", coreset.start_symbol_index, coreset.start_symbol_index + coreset.duration);
    helper.format_always(ctx, "f_re={}", coreset.frequency_resources);

    switch (coreset.cce_to_reg_mapping) {
      case srsran::pdcch_processor::cce_to_reg_mapping_type::CORESET0:
        helper.format_if_verbose(ctx, "cce_to_reg_map=coreset0");
        break;
      case srsran::pdcch_processor::cce_to_reg_mapping_type::INTERLEAVED:
        helper.format_if_verbose(ctx, "cce_to_reg_map=interleaved");
        helper.format_if_verbose(ctx, "reg_bundle_size={}", coreset.reg_bundle_size);
        helper.format_if_verbose(ctx, "interleaver_size={}", coreset.interleaver_size);
        break;
      case srsran::pdcch_processor::cce_to_reg_mapping_type::NON_INTERLEAVED:
        helper.format_if_verbose(ctx, "cce_to_reg_map=non-interleaved");
        break;
    }

    helper.format_if_verbose(ctx, "shift_idx={}", coreset.shift_index);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pdcch_processor::pdu_t.
template <>
struct formatter<srsran::pdcch_processor::pdu_t> {
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
  auto format(const srsran::pdcch_processor::pdu_t& pdu, FormatContext& ctx) const
  {
    helper.format_always(ctx, "rnti=0x{:04x}", pdu.dci.rnti);
    if (pdu.context.has_value()) {
      helper.format_always(ctx, *pdu.context);
    }
    helper.format_if_verbose(ctx, "slot={}", pdu.slot);
    helper.format_if_verbose(ctx, "cp={}", pdu.cp.to_string());
    helper.format_if_verbose(ctx, pdu.coreset);
    helper.format_always(ctx, "cce={}", pdu.dci.cce_index);
    helper.format_always(ctx, "al={}", pdu.dci.aggregation_level);
    helper.format_if_verbose(ctx, "size={}", pdu.dci.payload.size());
    helper.format_if_verbose(ctx, "n_id_dmrs={}", pdu.dci.n_id_pdcch_dmrs);
    helper.format_if_verbose(ctx, "n_id_data={}", pdu.dci.n_id_pdcch_data);
    helper.format_if_verbose(ctx, "n_rnti={}", pdu.dci.n_rnti);
    helper.format_if_verbose(ctx, "power_dmrs={:+.1f}dB", pdu.dci.dmrs_power_offset_dB);
    helper.format_if_verbose(ctx, "power_data={:+.1f}dB", pdu.dci.data_power_offset_dB);
    helper.format_if_verbose(ctx, "precoding={}", pdu.dci.precoding);
    return ctx.out();
  }
};

} // namespace fmt
