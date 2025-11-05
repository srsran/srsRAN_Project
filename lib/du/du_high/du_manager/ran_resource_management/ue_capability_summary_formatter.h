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

#include "ue_capability_summary.h"
#include "srsran/adt/span.h"
#include "srsran/support/format/delimited_formatter.h"
#include <fmt/format.h>

namespace fmt {

/// \brief Custom formatter for \c srsran::srs_du::ue_capability_summary::supported_band.
template <>
struct formatter<srsran::srs_du::ue_capability_summary::supported_band> {
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
  auto format(const srsran::srs_du::ue_capability_summary::supported_band& params, FormatContext& ctx) const
  {
    helper.format_always(ctx, "pusch_qam256_supported={}", params.pusch_qam256_supported);
    helper.format_always(ctx, "pusch_tx_coherence={}", srsran::to_string(params.pusch_tx_coherence));
    helper.format_always(ctx, "pusch_max_rank={}", params.pusch_max_rank);
    helper.format_always(ctx, "nof_srs_tx_ports={}", params.nof_srs_tx_ports);
    helper.format_always(ctx, "max_dl_harq_process_num={}", params.max_dl_harq_process_num);
    helper.format_always(ctx, "max_ul_harq_process_num={}", params.max_ul_harq_process_num);
    helper.format_always(ctx, "ul_pre_compensation_supported={}", params.ul_pre_compensation_supported);
    helper.format_always(ctx, "ul_ta_reporting_supported={}", params.ul_ta_reporting_supported);
    helper.format_always(ctx, "ue_specific_k_offset_supported={}", params.ue_specific_k_offset_supported);
    return ctx.out();
  }
};

/// \brief Custom formatter for \c srsran::srs_du::ue_capability_summary.
template <>
struct formatter<srsran::srs_du::ue_capability_summary> {
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
  auto format(const srsran::srs_du::ue_capability_summary& params, FormatContext& ctx) const
  {
    helper.format_always(ctx, "pdsch_qam256_supported={}", params.pdsch_qam256_supported);
    helper.format_always(ctx, "pdsch_qam64lowse_supported={}", params.pdsch_qam64lowse_supported);
    helper.format_always(ctx, "pusch_qam64lowse_supported={}", params.pusch_qam64lowse_supported);
    for (const auto& band : params.bands) {
      helper.format_always(ctx, "n{}={{{}}}", fmt::underlying(band.first), band.second);
    }
    helper.format_always(ctx, "long_drx_cycle_supported={}", params.long_drx_cycle_supported);
    helper.format_always(ctx, "short_drx_cycle_supported={}", params.short_drx_cycle_supported);
    helper.format_always(
        ctx, "pdsch_interleaving_vrb_to_prb_supported={}", params.pdsch_interleaving_vrb_to_prb_supported);
    helper.format_always(ctx, "ntn_supported={}", params.ntn_supported);
    helper.format_always(ctx, "disabled_dl_harq_feedback_supported={}", params.disabled_dl_harq_feedback_supported);
    helper.format_always(ctx, "ul_harq_mode_b_supported={}", params.ul_harq_mode_b_supported);
    return ctx.out();
  }
};

} // namespace fmt
