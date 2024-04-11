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

#include "fmt/format.h"

namespace srsran {
namespace srs_cu_up {

/// \brief Configurable parameters of the F1-U bearer in the CU-UP
struct f1u_config {
  bool warn_on_drop = true; ///< Log a warning instead of an info message whenever a PDU is dropped
};

} // namespace srs_cu_up
} // namespace srsran

namespace fmt {

// F1-U config formatter
template <>
struct formatter<srsran::srs_cu_up::f1u_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::srs_cu_up::f1u_config cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "warn_on_drop={}", cfg.warn_on_drop);
  }
};

} // namespace fmt
