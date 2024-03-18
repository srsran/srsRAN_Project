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

#include "srsran/phy/upper/signal_processors/srs/srs_estimator_configuration.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_result.h"
#include "srsran/ran/srs/srs_resource_formatter.h"
#include "srsran/support/format_utils.h"

namespace fmt {

/// \brief Custom formatter for \c srsran::srs_estimator_configuration.
template <>
struct formatter<srsran::srs_estimator_configuration> {
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
  auto format(const srsran::srs_estimator_configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_if_verbose(ctx, "slot={}", config.slot);
    helper.format_always(ctx, "{}", config.resource);
    helper.format_if_verbose(ctx, "ports=[{}]", srsran::span<const uint8_t>(config.ports));

    return ctx.out();
  }
};

} // namespace fmt