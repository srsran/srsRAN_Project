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
#include <cstdint>

namespace srsran::srs_du {

/// \brief Configurable parameters of the F1-U bearer in the DU
struct f1u_config {
  uint32_t t_notify;              ///< Timer used for periodic transmission of uplink notifications
  uint32_t rlc_queue_bytes_limit; ///< RLC queue limit in bytes. Used for initial report of buffer space to the CU-UP.
  bool     warn_on_drop         = true;  ///< Log a warning instead of an info message whenever a PDU is dropped
  bool     buffer_ul_on_startup = false; ///< Buffer UL SDUs on startup. Useful during handover to avoid losses.
  uint32_t ul_buffer_size       = 4096;  ///< Size of UL buffer for handover.
  std::chrono::milliseconds ul_buffer_timeout{700}; ///< How long to wait to wait for confirmation of finished handover.

  bool operator==(const f1u_config& other) const
  {
    return t_notify == other.t_notify and rlc_queue_bytes_limit == other.rlc_queue_bytes_limit and
           warn_on_drop == other.warn_on_drop and buffer_ul_on_startup == other.buffer_ul_on_startup and
           ul_buffer_size == other.ul_buffer_size;
  }
};

} // namespace srsran::srs_du

namespace fmt {

// F1-U config formatter
template <>
struct formatter<srsran::srs_du::f1u_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::srs_du::f1u_config cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "t_notify={} rlc_queue_bytes_limit={} warn_on_drop={} buffer_ul_on_startup={} ul_buffer_size={}",
                     cfg.t_notify,
                     cfg.rlc_queue_bytes_limit,
                     cfg.warn_on_drop,
                     cfg.buffer_ul_on_startup,
                     cfg.ul_buffer_size);
  }
};

} // namespace fmt
