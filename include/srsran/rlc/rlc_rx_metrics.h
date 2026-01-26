/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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

#include "srsran/support/engineering_notation.h"
#include "srsran/support/timers.h"
#include "fmt/format.h"
#include <variant>

namespace srsran {

struct rlc_tm_rx_metrics {
  void reset() {}
};

struct rlc_um_rx_metrics {
  uint32_t num_sdu_segments;      ///< Number of SDU segments RX'ed
  uint32_t num_sdu_segment_bytes; ///< Number of SDU segments Bytes
  void     reset()
  {
    num_sdu_segments      = {};
    num_sdu_segment_bytes = {};
  }
};

struct rlc_am_rx_metrics {
  uint32_t num_sdu_segments;      ///< Number of SDU segments RX'ed
  uint32_t num_sdu_segment_bytes; ///< Number of SDU segments bytes
  uint32_t num_ctrl_pdus;         ///< Number of control PDUs
  uint32_t num_ctrl_pdu_bytes;    ///< Number of control PDUs bytes
  void     reset()
  {
    num_sdu_segments      = {};
    num_sdu_segment_bytes = {};
    num_ctrl_pdus         = {};
    num_ctrl_pdu_bytes    = {};
  }
};

struct rlc_rx_metrics {
  // SDU metrics
  uint32_t num_sdus;      ///< Number of SDUs
  size_t   num_sdu_bytes; ///< Number of SDU bytes

  // PDU metrics
  uint32_t num_pdus;           ///< Number of PDUs
  size_t   num_pdu_bytes;      ///< Number of PDU bytes
  uint32_t num_lost_pdus;      ///< Number of dropped PDUs (reassembly timeout expiry or out of rx window)
  uint32_t num_malformed_pdus; ///< Number of malformed PDUs
  uint32_t sdu_latency_us;     ///< total SDU latency (in us)>

  /// Mode-specific metrics
  ///
  /// Variant that holds mode-specific metrics for TM, UM, or AM.
  std::variant<rlc_tm_rx_metrics, rlc_um_rx_metrics, rlc_am_rx_metrics> mode_specific;

  /// Counter of amount of times we collected metrics.
  /// Useful to aggregate high and low metrics.
  uint32_t counter = 0;

  rlc_rx_metrics get()
  {
    rlc_rx_metrics ret = *this;
    counter++;
    return ret;
  }

  void reset()
  {
    // SDU metrics
    num_sdus           = {};
    num_sdu_bytes      = {};
    num_pdus           = {};
    num_pdu_bytes      = {};
    num_lost_pdus      = {};
    num_malformed_pdus = {};
    sdu_latency_us     = {};

    // reset mode-specific values
    if (std::holds_alternative<rlc_tm_rx_metrics>(mode_specific)) {
      std::get<rlc_tm_rx_metrics>(mode_specific).reset();
    } else if (std::holds_alternative<rlc_um_rx_metrics>(mode_specific)) {
      std::get<rlc_um_rx_metrics>(mode_specific).reset();
    } else if (std::holds_alternative<rlc_am_rx_metrics>(mode_specific)) {
      std::get<rlc_am_rx_metrics>(mode_specific).reset();
    }

    // do not reset mode or counter
  }
};

class rlc_rx_metrics_interface
{
public:
  rlc_rx_metrics_interface()                                            = default;
  virtual ~rlc_rx_metrics_interface()                                   = default;
  rlc_rx_metrics_interface(const rlc_rx_metrics_interface&)             = delete;
  rlc_rx_metrics_interface& operator=(const rlc_rx_metrics_interface&)  = delete;
  rlc_rx_metrics_interface(const rlc_rx_metrics_interface&&)            = delete;
  rlc_rx_metrics_interface& operator=(const rlc_rx_metrics_interface&&) = delete;

  virtual rlc_rx_metrics get_metrics()           = 0;
  virtual rlc_rx_metrics get_and_reset_metrics() = 0;
  virtual void           reset_metrics()         = 0;
};

inline void format_rlc_rx_metrics(fmt::memory_buffer& buffer, timer_duration metrics_period, const rlc_rx_metrics& m)
{
  // Common metrics
  fmt::format_to(
      std::back_inserter(buffer),
      "num_sdus={} sdu_rate={}bps num_pdus={} pdu_rate={}bps",
      scaled_fmt_integer(m.num_sdus, false),
      float_to_eng_string(static_cast<float>(m.num_sdu_bytes) * 8 * 1000 / metrics_period.count(), 1, false),
      scaled_fmt_integer(m.num_pdus, false),
      float_to_eng_string(static_cast<double>(m.num_pdu_bytes) * 8 * 1000 / metrics_period.count(), 1, false));

  // Mode specific metrics
  if (std::holds_alternative<rlc_um_rx_metrics>(m.mode_specific)) {
    // UM metrics
    auto& um = std::get<rlc_um_rx_metrics>(m.mode_specific);
    fmt::format_to(std::back_inserter(buffer),
                   " num_sdu_segments={} sdu_segmments_rate={}bps",
                   scaled_fmt_integer(um.num_sdu_segments, false),
                   float_to_eng_string(
                       static_cast<float>(um.num_sdu_segment_bytes) * 8 * 1000 / metrics_period.count(), 1, false));
  } else if (std::holds_alternative<rlc_am_rx_metrics>(m.mode_specific)) {
    // AM metrics
    auto& am = std::get<rlc_am_rx_metrics>(m.mode_specific);
    fmt::format_to(
        std::back_inserter(buffer),
        " num_sdu_segments={} sdu_segmments_rate={}bps ctrl_pdus={} ctrl_rate={}bps",
        scaled_fmt_integer(am.num_sdu_segments, false),
        float_to_eng_string(static_cast<float>(am.num_sdu_segment_bytes) * 8 * 1000 / metrics_period.count(), 1, false),
        scaled_fmt_integer(am.num_ctrl_pdus, false),
        float_to_eng_string(static_cast<float>(am.num_ctrl_pdu_bytes) * 8 * 1000 / metrics_period.count(), 1, false));
  }
}
} // namespace srsran

namespace fmt {

// RLC RX metrics formatter
// TODO print mode-specific metrics
template <>
struct formatter<srsran::rlc_rx_metrics> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rlc_rx_metrics& m, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "num_pdus={} num_pdu_bytes={} num_lost_pdus={} num_malformed_pdus={} num_sdus={} num_sdu_bytes={}",
                     m.num_pdus,
                     m.num_pdu_bytes,
                     m.num_lost_pdus,
                     m.num_malformed_pdus,
                     m.num_sdus,
                     m.num_sdu_bytes);
  }
};
} // namespace fmt
