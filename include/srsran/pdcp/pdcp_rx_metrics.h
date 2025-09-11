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
#include "fmt/ranges.h"
#include <array>
#include <optional>

/*
 * This file will hold the interfaces and structures for the
 * PDCP RX entity metrics collection. This also includes formatting
 * helpers for printing the metrics.
 */
namespace srsran {

/// This struct will hold relevant metrics for the PDCP RX
struct pdcp_rx_metrics_container {
  uint32_t num_pdus;
  uint32_t num_pdu_bytes;
  uint32_t num_data_pdus;
  uint32_t num_data_pdu_bytes;
  uint32_t num_dropped_pdus;
  uint32_t num_sdus;
  uint32_t num_sdu_bytes;
  uint32_t num_integrity_verified_pdus;
  uint32_t num_integrity_failed_pdus;
  uint32_t num_t_reordering_timeouts;
  uint32_t reordering_delay_us;
  uint32_t reordering_counter;
  uint32_t sum_sdu_latency_ns; ///< total SDU latency (in ns)
  unsigned counter;

  // CPU Usage metrics
  uint32_t sum_crypto_processing_latency_ns;

  // Histogram of SDU latencies
  static constexpr unsigned                   sdu_latency_hist_bins = 8;
  static constexpr unsigned                   nof_usec_per_bin      = 1;
  std::array<uint32_t, sdu_latency_hist_bins> sdu_latency_hist;

  std::optional<uint32_t> min_sdu_latency_ns;
  std::optional<uint32_t> max_sdu_latency_ns;

  pdcp_rx_metrics_container copy()
  {
    pdcp_rx_metrics_container copy = *this;
    ++counter;
    return copy;
  }
};

inline std::string format_pdcp_rx_metrics(timer_duration metrics_period, const pdcp_rx_metrics_container& m)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer),
                 "num_sdus={} sdu_rate={}bps num_dropped_pdus={} num_pdus={} pdu_rate={}bps "
                 "num_integrity_verified_pdus={} num_integrity_failed_pdus={} num_t_reordering_timeouts={} "
                 "avg_reordering_delay={:.2f}ms reordering_counter={} avg_sdu_latency={:.2f}us sdu_latency_hist=[",
                 scaled_fmt_integer(m.num_sdus, false),
                 float_to_eng_string(static_cast<float>(m.num_sdu_bytes) * 8 * 1000 / metrics_period.count(), 1, false),
                 scaled_fmt_integer(m.num_dropped_pdus, false),
                 scaled_fmt_integer(m.num_pdus, false),
                 float_to_eng_string(static_cast<float>(m.num_pdu_bytes) * 8 * 1000 / metrics_period.count(), 1, false),
                 scaled_fmt_integer(m.num_integrity_verified_pdus, false),
                 scaled_fmt_integer(m.num_integrity_failed_pdus, false),
                 scaled_fmt_integer(m.num_t_reordering_timeouts, false),
                 m.reordering_counter > 0 ? static_cast<float>(m.reordering_delay_us) / m.reordering_counter * 1e-3 : 0,
                 scaled_fmt_integer(m.reordering_counter, false),
                 m.num_sdus > 0 ? static_cast<float>(m.sum_sdu_latency_ns) / m.num_sdus * 1e-3 : 0);
  bool first_bin = true;
  for (auto freq : m.sdu_latency_hist) {
    fmt::format_to(std::back_inserter(buffer), "{}{}", first_bin ? "" : " ", float_to_eng_string(freq, 1, false));
    first_bin = false;
  }
  fmt::format_to(std::back_inserter(buffer), "]");
  if (m.min_sdu_latency_ns.has_value()) {
    fmt::format_to(std::back_inserter(buffer), " min_sdu_latency={:.2f}us", m.min_sdu_latency_ns.value() * 1e-3);
  } else {
    fmt::format_to(std::back_inserter(buffer), " min_sdu_latency={{na}}");
  }
  if (m.max_sdu_latency_ns) {
    fmt::format_to(std::back_inserter(buffer), " max_sdu_latency={:.2f}us", m.max_sdu_latency_ns.value() * 1e-3);
  } else {
    fmt::format_to(std::back_inserter(buffer), " max_sdu_latency={{na}}");
  }
  fmt::format_to(std::back_inserter(buffer),
                 " crypto_cpu_usage={:.2f}\%",
                 static_cast<float>(m.sum_crypto_processing_latency_ns) / (1000000 * metrics_period.count()) * 100);
  return to_c_str(buffer);
}
} // namespace srsran

namespace fmt {
// PDCP RX metrics formatter
template <>
struct formatter<srsran::pdcp_rx_metrics_container> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::pdcp_rx_metrics_container& m, FormatContext& ctx) const
  {
    return format_to(
        ctx.out(),
        "num_sdus={} num_sdu_bytes={} num_dropped_pdus={} num_pdus={} num_pdu_bytes={} "
        "num_integrity_verified_pdus={} num_integrity_failed_pdus={} num_t_reordering_timeouts={} "
        "avg_reordering_delay={:.2f}ms reordering_counter={} avg_sdu_latency={:.2f}us sdu_latency_hist=[{}] "
        "min_sdu_latency={}{} max_sdu_latency={}{} avg_crypto_latency={:.2f}us",
        m.num_sdus,
        m.num_sdu_bytes,
        m.num_dropped_pdus,
        m.num_pdus,
        m.num_pdu_bytes,
        m.num_integrity_verified_pdus,
        m.num_integrity_failed_pdus,
        m.num_t_reordering_timeouts,
        m.reordering_counter > 0 ? static_cast<float>(m.reordering_delay_us) / m.reordering_counter * 1e-3 : 0,
        m.reordering_counter,
        m.num_sdus > 0 ? m.sum_sdu_latency_ns / m.num_sdus * 1e-3 : 0,
        fmt::join(m.sdu_latency_hist, " "),
        m.min_sdu_latency_ns,
        m.min_sdu_latency_ns.has_value() ? "ns" : "",
        m.max_sdu_latency_ns,
        m.max_sdu_latency_ns.has_value() ? "ns" : "",
        m.num_sdus > 0 ? m.sum_crypto_processing_latency_ns / m.num_sdus * 1e-3 : 0);
  }
};
} // namespace fmt
