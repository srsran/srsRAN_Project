/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/srs/srs_resource_configuration.h"
#include "srsran/support/format/delimited_formatter.h"

namespace fmt {

/// \brief Custom formatter for \c srsran::srs_resource_configuration.
template <>
struct formatter<srsran::srs_resource_configuration> {
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
  auto format(const srsran::srs_resource_configuration& resource, FormatContext& ctx) const
  {
    helper.format_if_verbose(ctx, "nof_antenna_ports={}", fmt::underlying(resource.nof_antenna_ports));
    helper.format_if_verbose(ctx,
                             "symb=[{}, {})",
                             resource.start_symbol,
                             resource.start_symbol.value() + static_cast<unsigned>(resource.nof_symbols));
    helper.format_if_verbose(ctx, "configuration_index={}", resource.configuration_index);
    helper.format_if_verbose(ctx, "sequence_id={}", resource.sequence_id);
    helper.format_if_verbose(ctx, "bandwidth_index={}", resource.bandwidth_index);
    helper.format_if_verbose(ctx, "comb_size={}", fmt::underlying(resource.comb_size));
    helper.format_if_verbose(ctx, "comb_offset={}", resource.comb_offset);
    helper.format_if_verbose(ctx, "cyclic_shift={}", resource.cyclic_shift);
    helper.format_if_verbose(ctx, "freq_position={}", resource.freq_position);
    helper.format_if_verbose(ctx, "freq_shift={}", resource.freq_shift);
    helper.format_if_verbose(ctx, "freq_hopping={}", resource.freq_hopping);
    helper.format_if_verbose(ctx, "hopping={}", fmt::underlying(resource.hopping));
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
