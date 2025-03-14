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
#include <optional>

namespace srsran {

/// Structure used to represent RLC buffer state.
/// The buffer state is transmitted towards lower layers (i.e. the MAC and the scheduler).
/// It includes the amount of data pending for transmission (queued SDUs, headers, and for RLC AM, ReTx and status PDUs)
/// and the time of arrival of the oldest PDU among those queues.
struct rlc_buffer_state {
  /// Amount of bytes pending for transmission.
  unsigned pending_bytes = 0;
  /// Head of line (HOL) time of arrival (TOA) holds the TOA of the oldest SDU or ReTx that is queued for transmission.
  std::optional<std::chrono::time_point<std::chrono::steady_clock>> hol_toa;
};
} // namespace srsran

namespace fmt {

// associated formatter
template <>
struct formatter<srsran::rlc_buffer_state> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rlc_buffer_state& bs, FormatContext& ctx) const
  {
    if (bs.hol_toa) {
      return format_to(ctx.out(),
                       "pending_bytes={} hol_toa={}",
                       bs.pending_bytes,
                       std::chrono::duration_cast<std::chrono::microseconds>(bs.hol_toa->time_since_epoch()));
    }
    return format_to(ctx.out(), "pending_bytes={} hol_toa=none", bs.pending_bytes);
  }
};
} // namespace fmt
