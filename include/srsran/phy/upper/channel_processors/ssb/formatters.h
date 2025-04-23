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

#include "srsran/phy/upper/channel_processors/ssb/ssb_processor.h"
#include "srsran/srsvec/copy.h"
#include "srsran/support/format/delimited_formatter.h"

namespace fmt {

/// \brief Custom formatter for \c ssb_processor::pdu_t.
template <>
struct formatter<srsran::ssb_processor::pdu_t> {
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
  auto format(const srsran::ssb_processor::pdu_t& pdu, FormatContext& ctx) const
  {
    helper.format_always(ctx, "pci={}", pdu.phys_cell_id);
    helper.format_always(ctx, "ssb_idx={}", pdu.ssb_idx);
    helper.format_always(ctx, "L_max={}", pdu.L_max);
    helper.format_always(ctx, "common_scs={}", scs_to_khz(pdu.common_scs));
    helper.format_always(ctx, "sc_offset={}", pdu.subcarrier_offset.value());
    helper.format_always(ctx, "offset_PointA={}", pdu.offset_to_pointA.value());
    helper.format_always(ctx, "pattern={}", to_string(pdu.pattern_case));

    helper.format_if_verbose(ctx, "beta_pss={:+.1f}dB", pdu.beta_pss);
    helper.format_if_verbose(ctx, "slot={}", pdu.slot);
    helper.format_if_verbose(ctx, "ports={}", srsran::span<const uint8_t>(pdu.ports));

    return ctx.out();
  }
};

} // namespace fmt
