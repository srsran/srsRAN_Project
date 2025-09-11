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

#include "srsran/support/engineering_notation.h"
#include "srsran/support/format/fmt_to_c_str.h"
#include "srsran/support/timers.h"
#include "fmt/format.h"
#include "fmt/std.h"
#include <optional>
#include <variant>

namespace srsran {

struct rlc_tx_metrics_higher {
  uint32_t num_sdus;             ///< Number of SDUs
  size_t   num_sdu_bytes;        ///< Number of SDU bytes
  uint32_t num_dropped_sdus;     ///< Number of dropped SDUs (due to full queue)
  uint32_t num_discarded_sdus;   ///< Number of discarded SDUs (instructed from higher layer)
  uint32_t num_discard_failures; ///< Number of failed SDU discards (instructed from higher layer)

  /// Counter of amount of times we collected metrics.
  /// Useful to aggregate high and low metrics.
  uint32_t counter = 0;

  rlc_tx_metrics_higher get()
  {
    rlc_tx_metrics_higher ret = *this;
    counter++;
    return ret;
  }

  void reset()
  {
    num_sdus             = {};
    num_sdu_bytes        = {};
    num_dropped_sdus     = {};
    num_discarded_sdus   = {};
    num_discard_failures = {};
    // do not reset counter
  }
};

struct rlc_tm_tx_metrics_lower {
  uint32_t num_small_allocs; ///< Number of allocations that are too small to TX PDU

  void reset() { num_small_allocs = {}; }
};

struct rlc_um_tx_metrics_lower {
  uint32_t num_pdus_with_segmentation;      ///< Number of transmitted PDUs with segmentation
  uint32_t num_pdu_bytes_with_segmentation; ///< Number of transmitted PDU bytes with segmentation

  void reset()
  {
    num_pdus_with_segmentation      = {};
    num_pdu_bytes_with_segmentation = {};
  }
};

struct rlc_am_tx_metrics_poll_latency {
  uint32_t                num_latency_meas; ///< Number of handle status latency measurements
  uint32_t                sum_latency_us;   ///< Total handle status latency over a (in us)
  std::optional<uint32_t> min_latency_us;   ///< Minimum handle status latency (in us)
  std::optional<uint32_t> max_latency_us;   ///< Maximum handle status latency (in us)
};

struct rlc_am_tx_metrics_lower {
  uint32_t num_pdus_with_segmentation;      ///< Number of transmitted PDUs with segmentation
  uint32_t num_pdu_bytes_with_segmentation; ///< Number of transmitted PDU bytes with segmentation
  uint32_t num_retx_pdus;                   ///< Number of retransmitted PDUs
  uint32_t num_retx_pdu_bytes;              ///< Number of retransmitted PDU bytes
  uint32_t num_ctrl_pdus;                   ///< Number of control PDUs
  uint32_t num_ctrl_pdu_bytes;              ///< Number of control PDUs bytes

  uint32_t                num_ack_latency_meas; ///< Number of ACK latency measurements
  uint32_t                sum_ack_latency_ms;   ///< Total ACK latency over a (in ms)
  std::optional<uint32_t> min_ack_latency_ms;   ///< Minimum ACK latency (in ms)
  std::optional<uint32_t> max_ack_latency_ms;   ///< Maximum ACK latency (in ms)

  uint32_t                num_handle_status_latency_meas; ///< Number of handle status latency measurements
  uint32_t                sum_handle_status_latency_us;   ///< Total handle status latency over a (in us)
  std::optional<uint32_t> min_handle_status_latency_us;   ///< Minimum handle status latency (in us)
  std::optional<uint32_t> max_handle_status_latency_us;   ///< Maximum handle status latency (in us)
  std::optional<uint32_t> max_processed_acks;             ///< Processed ACKs in slowest handle status.
  std::optional<uint32_t> max_processed_nacks;            ///< Processed NACKs in slowest handle status.

  rlc_am_tx_metrics_poll_latency poll_latency;

  void reset()
  {
    num_pdus_with_segmentation      = {};
    num_pdu_bytes_with_segmentation = {};
    num_retx_pdus                   = {};
    num_retx_pdu_bytes              = {};
    num_ctrl_pdus                   = {};
    num_ctrl_pdu_bytes              = {};

    num_ack_latency_meas = {};
    sum_ack_latency_ms   = {};
    min_ack_latency_ms   = {};
    max_ack_latency_ms   = {};

    num_handle_status_latency_meas = {};
    sum_handle_status_latency_us   = {};
    min_handle_status_latency_us   = {};
    max_handle_status_latency_us   = {};
  }
};

struct rlc_tx_metrics_lower {
  uint32_t num_pdus_no_segmentation;      ///< Number of transmitted PDUs without segmentation
  size_t   num_pdu_bytes_no_segmentation; ///< Number of transmitted PDU bytes without segmentation
  uint32_t num_of_pulled_sdus;            ///< Number of pulled SDUs
  uint32_t sum_sdu_latency_us;            ///< total SDU latency (in us)>
  uint32_t sum_pdu_latency_ns;            ///< total PDU latency (in ns)>

  // Histogram of pull pdus
  static constexpr unsigned                   pdu_latency_hist_bins = 8;
  static constexpr unsigned                   nof_usec_per_bin      = 1;
  std::array<uint32_t, pdu_latency_hist_bins> pdu_latency_hist_ns;
  uint32_t                                    max_pdu_latency_ns;

  /// Mode-specific metrics
  ///
  /// Variant that holds mode-specific metrics for TM, UM, or AM.
  std::variant<rlc_tm_tx_metrics_lower, rlc_um_tx_metrics_lower, rlc_am_tx_metrics_lower> mode_specific;

  /// Counter of amount of times we collected metrics.
  /// Useful to aggregate high and low metrics.
  uint32_t counter = 0;

  rlc_tx_metrics_lower get()
  {
    rlc_tx_metrics_lower ret = *this;
    counter++;
    return ret;
  }
  void reset()
  {
    num_pdus_no_segmentation      = {};
    num_pdu_bytes_no_segmentation = {};
    num_of_pulled_sdus            = {};
    sum_sdu_latency_us            = {};
    sum_pdu_latency_ns            = {};
    pdu_latency_hist_ns           = {};
    max_pdu_latency_ns            = {};

    // reset mode-specific values
    if (std::holds_alternative<rlc_tm_tx_metrics_lower>(mode_specific)) {
      std::get<rlc_tm_tx_metrics_lower>(mode_specific).reset();
    } else if (std::holds_alternative<rlc_um_tx_metrics_lower>(mode_specific)) {
      std::get<rlc_um_tx_metrics_lower>(mode_specific).reset();
    } else if (std::holds_alternative<rlc_am_tx_metrics_lower>(mode_specific)) {
      std::get<rlc_am_tx_metrics_lower>(mode_specific).reset();
    }
  }
};

struct rlc_tx_metrics {
  rlc_tx_metrics_higher tx_high;
  rlc_tx_metrics_lower  tx_low;
};

class rlc_tx_metrics_interface
{
public:
  rlc_tx_metrics_interface()                                            = default;
  virtual ~rlc_tx_metrics_interface()                                   = default;
  rlc_tx_metrics_interface(const rlc_tx_metrics_interface&)             = delete;
  rlc_tx_metrics_interface& operator=(const rlc_tx_metrics_interface&)  = delete;
  rlc_tx_metrics_interface(const rlc_tx_metrics_interface&&)            = delete;
  rlc_tx_metrics_interface& operator=(const rlc_tx_metrics_interface&&) = delete;

  virtual rlc_tx_metrics get_metrics()           = 0;
  virtual rlc_tx_metrics get_and_reset_metrics() = 0;
  virtual void           reset_metrics()         = 0;
};

// Format t-PollRetransmission latency metrics
inline void format_rlc_am_tx_t_poll_latencty_metrics(fmt::memory_buffer&                   buffer,
                                                     const rlc_am_tx_metrics_poll_latency& m)
{
  fmt::format_to(std::back_inserter(buffer), " t_poll_nof_expiration={}", m.num_latency_meas);
  if (m.num_latency_meas > 0) {
    fmt::format_to(std::back_inserter(buffer),
                   " t_poll_latency_avg={:.1f}us t_poll_latency_min={}us t_poll_latency_max={}us",
                   (float)m.sum_latency_us / m.num_latency_meas,
                   m.min_latency_us,
                   m.max_latency_us);
  }
}

inline void format_rlc_tx_metrics(fmt::memory_buffer& buffer, timer_duration metrics_period, const rlc_tx_metrics& m)
{
  /// Common metrics
  fmt::format_to(
      std::back_inserter(buffer),
      "num_sdus={} sdu_rate={}bps dropped_sdus={} discarded_sdus={} "
      "num_pdus_no_segm={} pdu_rate_no_segm={}bps",
      scaled_fmt_integer(m.tx_high.num_sdus, false),
      float_to_eng_string(static_cast<float>(m.tx_high.num_sdu_bytes) * 8 * 1000 / (metrics_period.count()), 1, false),
      scaled_fmt_integer(m.tx_high.num_dropped_sdus, false),
      scaled_fmt_integer(m.tx_high.num_discarded_sdus, false),
      scaled_fmt_integer(m.tx_low.num_pdus_no_segmentation, false),
      float_to_eng_string(
          static_cast<float>(m.tx_low.num_pdu_bytes_no_segmentation) * 8 * 1000 / metrics_period.count(), 1, false));

  /// Mode specific metrics
  if (std::holds_alternative<rlc_um_tx_metrics_lower>(m.tx_low.mode_specific)) {
    /// UM metrics
    auto& um = std::get<rlc_um_tx_metrics_lower>(m.tx_low.mode_specific);
    fmt::format_to(std::back_inserter(buffer),
                   " num_pdus_with_segm={} pdu_with_segm_rate={}bps",
                   um.num_pdus_with_segmentation,
                   static_cast<float>(um.num_pdu_bytes_with_segmentation) * 8 / metrics_period.count());
  } else if (std::holds_alternative<rlc_am_tx_metrics_lower>(m.tx_low.mode_specific)) {
    /// AM metrics
    auto& am = std::get<rlc_am_tx_metrics_lower>(m.tx_low.mode_specific);
    fmt::format_to(
        std::back_inserter(buffer),
        " num_pdus_with_segm={} pdu_rate_with_segm={}bps num_retx={}"
        " retx_rate={}bps ctrl_pdus={} ctrl_rate={}bps pull_latency_avg={} pull_latency_sum={}s"
        " num_ack_latency_meas={}",
        scaled_fmt_integer(am.num_pdus_with_segmentation, false),
        float_to_eng_string(
            static_cast<float>(am.num_pdu_bytes_with_segmentation) * 8 * 1000 / metrics_period.count(), 1, false),
        scaled_fmt_integer(am.num_retx_pdus, false),
        float_to_eng_string(static_cast<float>(am.num_retx_pdu_bytes) * 8 * 1000 / metrics_period.count(), 1, false),
        scaled_fmt_integer(am.num_ctrl_pdus, false),
        float_to_eng_string(
            static_cast<float>(am.num_ctrl_pdu_bytes) * 8 * 1000 / (double)metrics_period.count(), 1, false),
        float_to_eng_string(static_cast<float>(m.tx_low.sum_pdu_latency_ns * 1e-9) /
                                (m.tx_low.num_pdus_no_segmentation + am.num_pdus_with_segmentation + am.num_retx_pdus +
                                 am.num_ctrl_pdus),
                            1,
                            false),
        float_to_eng_string(static_cast<float>(m.tx_low.sum_pdu_latency_ns) * 1e-9, 1, false),
        scaled_fmt_integer(am.num_ack_latency_meas, false));
    if (am.min_ack_latency_ms.has_value()) {
      fmt::format_to(std::back_inserter(buffer), " ack_latency_min={}ms", am.min_ack_latency_ms.value());
    }
    if (am.num_ack_latency_meas > 0) {
      fmt::format_to(
          std::back_inserter(buffer),
          " ack_latency_avg={}s",
          float_to_eng_string(static_cast<float>(am.sum_ack_latency_ms * 1e-3) / am.num_ack_latency_meas, 1, false));
    }
    if (am.max_ack_latency_ms.has_value()) {
      fmt::format_to(std::back_inserter(buffer), " ack_latency_max={}ms", am.max_ack_latency_ms.value());
    }
    fmt::format_to(std::back_inserter(buffer),
                   " num_handle_status_latency_meas={}",
                   scaled_fmt_integer(am.num_handle_status_latency_meas, false));
    if (am.min_handle_status_latency_us.has_value()) {
      fmt::format_to(
          std::back_inserter(buffer), " min_handle_status_latency={}us", am.min_handle_status_latency_us.value());
    }
    if (am.max_handle_status_latency_us.has_value()) {
      fmt::format_to(
          std::back_inserter(buffer), " max_handle_status_latency={}us", am.max_handle_status_latency_us.value());
    }
    if (am.num_handle_status_latency_meas > 0) {
      fmt::format_to(std::back_inserter(buffer),
                     " handle_status_latency_avg={}s",
                     float_to_eng_string(static_cast<float>(am.sum_handle_status_latency_us * 1e-6) /
                                             am.num_handle_status_latency_meas,
                                         1,
                                         false));
    }
    if (am.max_processed_acks.has_value()) {
      fmt::format_to(std::back_inserter(buffer), " max_processed_acks={}", am.max_processed_acks.value());
    }
    if (am.max_processed_nacks.has_value()) {
      fmt::format_to(std::back_inserter(buffer), " max_processed_nacks={}", am.max_processed_nacks.value());
    }

    format_rlc_am_tx_t_poll_latencty_metrics(buffer, am.poll_latency);
  }
  fmt::format_to(std::back_inserter(buffer), " pdu_latency_hist=[");
  for (unsigned i = 0; i < rlc_tx_metrics_lower::pdu_latency_hist_bins; i++) {
    fmt::format_to(std::back_inserter(buffer), " {}", float_to_eng_string(m.tx_low.pdu_latency_hist_ns[i], 1, false));
  }
  fmt::format_to(std::back_inserter(buffer),
                 "] max_pull_latency={:.2f}us",
                 static_cast<float>(m.tx_low.max_pdu_latency_ns * 1e-3));
}
} // namespace srsran

namespace fmt {

// RLC TX metrics formatter
// TODO print mode-specific metrics
template <>
struct formatter<srsran::rlc_tx_metrics> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rlc_tx_metrics& m, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "num_sdus={} num_sdu_bytes={} num_dropped_sdus={} num_discarded_sdus={} "
                     "num_discard_failures={} num_pdus_no_segm={} num_pdu_bytes_no_segm={}",
                     m.tx_high.num_sdus,
                     m.tx_high.num_sdu_bytes,
                     m.tx_high.num_dropped_sdus,
                     m.tx_high.num_discarded_sdus,
                     m.tx_high.num_discard_failures,
                     m.tx_low.num_pdus_no_segmentation,
                     m.tx_low.num_pdu_bytes_no_segmentation);
  }
};
} // namespace fmt
