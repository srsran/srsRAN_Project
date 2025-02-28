/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
    // TODO: add formatter for std::chrono::time_point<std::chrono::steady_clock>
    return format_to(ctx.out(), "pending_bytes={} hol_toa={}", bs.pending_bytes, bs.hol_toa.has_value());
  }
};
} // namespace fmt
