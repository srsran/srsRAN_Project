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

#pragma once

#include "srsran/ran/srs/srs_resource_configuration.h"
#include "srsran/support/format_utils.h"

namespace fmt {

/// \brief Custom formatter for \c srsran::srs_resource_configuration.
template <>
struct formatter<srsran::srs_resource_configuration> {
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
  auto format(const srsran::srs_resource_configuration& resource, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_if_verbose(ctx, "nof_antenna_ports={}", resource.nof_antenna_ports);
    helper.format_if_verbose(ctx, "nof_symbols={}", resource.nof_symbols);
    helper.format_if_verbose(ctx, "start_symbol={}", resource.start_symbol);
    helper.format_if_verbose(ctx, "configuration_index={}", resource.configuration_index);
    helper.format_if_verbose(ctx, "sequence_id={}", resource.sequence_id);
    helper.format_if_verbose(ctx, "bandwidth_index={}", resource.bandwidth_index);
    helper.format_if_verbose(ctx, "comb_size={}", resource.comb_size);
    helper.format_if_verbose(ctx, "comb_offset={}", resource.comb_offset);
    helper.format_if_verbose(ctx, "cyclic_shift={}", resource.cyclic_shift);
    helper.format_if_verbose(ctx, "freq_position={}", resource.freq_position);
    helper.format_if_verbose(ctx, "freq_shift={}", resource.freq_shift);
    helper.format_if_verbose(ctx, "freq_hopping={}", resource.freq_hopping);
    helper.format_if_verbose(ctx, "hopping={}", resource.hopping);
    if (resource.periodicity.has_value()) {
      helper.format_if_verbose(
          ctx, "periodicity={}/{}", resource.periodicity->periodicity, resource.periodicity->offset);
    } else {
      helper.format_if_verbose(ctx, "periodicity=na");
    }

    return ctx.out();
  }
};

} // namespace fmt