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

#include "fmt/format.h"
#include <chrono>

namespace srsran {
namespace srs_cu_up {

/// \brief Configurable parameters of the F1-U bearer in the CU-UP
struct f1u_config {
  bool                      warn_on_drop = true; ///< Log a warning instead of an info message whenever a PDU is dropped
  std::chrono::milliseconds dl_t_notif_timer{10};
  uint32_t                  queue_size = 8192;
  uint32_t                  batch_size = 256;
};

} // namespace srs_cu_up
} // namespace srsran

namespace fmt {

// F1-U config formatter
template <>
struct formatter<srsran::srs_cu_up::f1u_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::srs_cu_up::f1u_config cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "warn_on_drop={} dl_t_notif_timer={}ms queue_size={} batch_size={}",
                     cfg.warn_on_drop,
                     cfg.dl_t_notif_timer.count(),
                     cfg.queue_size,
                     cfg.batch_size);
  }
};

} // namespace fmt
