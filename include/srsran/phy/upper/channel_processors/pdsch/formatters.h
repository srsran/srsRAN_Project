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
#include "srsran/phy/support/re_pattern_formatters.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_processor.h"
#include "srsran/ran/pdsch/pdsch_context_formatter.h"

namespace fmt {

/// \brief Custom formatter for \c pdsch_processor::codeword_description.
template <>
struct formatter<srsran::pdsch_processor::codeword_description> {
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
  auto format(const srsran::pdsch_processor::codeword_description& codeword_descr, FormatContext& ctx) const
  {
    helper.format_always(ctx, "mod={}", to_string(codeword_descr.modulation));
    helper.format_always(ctx, "rv={}", codeword_descr.rv);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pdsch_processor::pdu_t.
template <>
struct formatter<srsran::pdsch_processor::pdu_t> {
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
  auto format(const srsran::pdsch_processor::pdu_t& pdu, FormatContext& ctx) const
  {
    helper.format_always(ctx, "rnti=0x{:04x}", pdu.rnti);
    if (pdu.context.has_value()) {
      helper.format_always(ctx, *pdu.context);
    }
    helper.format_if_verbose(ctx, "bwp=[{}, {})", pdu.bwp_start_rb, pdu.bwp_start_rb + pdu.bwp_size_rb);
    helper.format_always(ctx, "prb={}", pdu.freq_alloc);
    helper.format_always(ctx, "symb=[{}, {})", pdu.start_symbol_index, pdu.start_symbol_index + pdu.nof_symbols);
    helper.format_always(ctx, srsran::span<const srsran::pdsch_processor::codeword_description>(pdu.codewords));

    helper.format_if_verbose(ctx, "n_id={}", pdu.n_id);
    helper.format_if_verbose(
        ctx, "ref_point={}", (pdu.ref_point == srsran::pdsch_processor::pdu_t::CRB0) ? "CRB0" : "PRB0");
    helper.format_if_verbose(ctx, "dmrs_type={}", (pdu.dmrs == srsran::dmrs_type::TYPE1) ? 1 : 2);
    helper.format_if_verbose(ctx, "dmrs_mask={}", pdu.dmrs_symbol_mask);
    helper.format_if_verbose(ctx, "n_scidid={}", pdu.scrambling_id);
    helper.format_if_verbose(ctx, "n_scid={}", pdu.n_scid);
    helper.format_if_verbose(ctx, "ncgwd={}", pdu.nof_cdm_groups_without_data);
    helper.format_if_verbose(ctx, "bg={}", (pdu.ldpc_base_graph == srsran::ldpc_base_graph_type::BG1) ? "BG1" : "BG2");
    helper.format_if_verbose(ctx, "tbs_lbrm={}bytes", pdu.tbs_lbrm);
    helper.format_if_verbose(ctx, "power_dmrs={:+.1f}dB", -pdu.ratio_pdsch_dmrs_to_sss_dB);
    helper.format_if_verbose(ctx, "power_data={:+.1f}dB", pdu.ratio_pdsch_data_to_sss_dB);
    helper.format_if_verbose(ctx, "slot={}", pdu.slot);
    helper.format_if_verbose(ctx, "cp={}", pdu.cp.to_string());
    helper.format_if_verbose(ctx, "precoding={}", pdu.precoding);
    if (pdu.reserved.get_nof_entries() > 0) {
      helper.format_if_verbose(ctx, "reserved=[{:,}]", pdu.reserved.get_re_patterns());
    }
    return ctx.out();
  }
};

} // namespace fmt
