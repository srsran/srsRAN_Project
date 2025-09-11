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
 * PDCP TX entity metrics collection. This also includes formatting
 * helpers for printing the metrics.
 */
namespace srsran {

/// This struct will hold relevant metrics for the PDCP TX
struct pdcp_tx_metrics_container {
  uint32_t num_sdus;             ///< Number of SDUs
  uint32_t num_sdu_bytes;        ///< Number of SDU bytes
  uint32_t num_dropped_sdus;     ///< Number of dropped SDUs
  uint32_t num_pdus;             ///< Number of transmitted PDUs
  uint32_t num_pdu_bytes;        ///< Number of transmitted PDU bytes
  uint32_t num_discard_timeouts; ///< Number of discard timeouts
  uint32_t sum_pdu_latency_ns;   ///< total PDU latency (in ns)
  unsigned counter;

  // CPU Usage metrics
  uint32_t sum_crypto_processing_latency_ns;

  // Histogram of PDU latencies
  static constexpr unsigned                   pdu_latency_hist_bins = 8;
  static constexpr unsigned                   nof_usec_per_bin      = 1;
  std::array<uint32_t, pdu_latency_hist_bins> pdu_latency_hist;

  std::optional<uint32_t> min_pdu_latency_ns;
  std::optional<uint32_t> max_pdu_latency_ns;

  pdcp_tx_metrics_container copy()
  {
    pdcp_tx_metrics_container copy = *this;
    ++counter;
    return copy;
  }
};

inline std::string format_pdcp_tx_metrics(timer_duration metrics_period, const pdcp_tx_metrics_container& m)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer),
                 "num_sdus={} sdu_rate={}bps dropped_sdus={} num_pdus={} pdu_rate={}bps num_discard_timeouts={} "
                 "avg_pdu_latency={:.2f}us "
                 "pdu_latency_hist=[",
                 scaled_fmt_integer(m.num_sdus, false),
                 float_to_eng_string(static_cast<float>(m.num_sdu_bytes) * 8 * 1000 / metrics_period.count(), 1, false),
                 scaled_fmt_integer(m.num_dropped_sdus, false),
                 scaled_fmt_integer(m.num_pdus, false),
                 float_to_eng_string(static_cast<float>(m.num_pdu_bytes) * 8 * 1000 / metrics_period.count(), 1, false),
                 scaled_fmt_integer(m.num_discard_timeouts, false),
                 m.num_pdus > 0 ? m.sum_pdu_latency_ns / m.num_pdus * 1e-3 : 0);
  bool first_bin = true;
  for (auto freq : m.pdu_latency_hist) {
    fmt::format_to(std::back_inserter(buffer), "{}{}", first_bin ? "" : " ", float_to_eng_string(freq, 1, false));
    first_bin = false;
  }
  fmt::format_to(std::back_inserter(buffer), "]");
  if (m.min_pdu_latency_ns) {
    fmt::format_to(std::back_inserter(buffer), " min_pdu_latency={:.2f}us", *m.min_pdu_latency_ns * 1e-3);
  } else {
    fmt::format_to(std::back_inserter(buffer), " min_pdu_latency={{na}}");
  }
  if (m.max_pdu_latency_ns) {
    fmt::format_to(std::back_inserter(buffer), " max_pdu_latency={:.2f}us", *m.max_pdu_latency_ns * 1e-3);
  } else {
    fmt::format_to(std::back_inserter(buffer), " max_pdu_latency={{na}}");
  }
  fmt::format_to(std::back_inserter(buffer),
                 " crypto_cpu_usage={:.2f}\%",
                 static_cast<float>(m.sum_crypto_processing_latency_ns) / (1000000 * metrics_period.count()) * 100);
  return to_c_str(buffer);
}

} // namespace srsran

namespace fmt {
// PDCP TX metrics formatter
template <>
struct formatter<srsran::pdcp_tx_metrics_container> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::pdcp_tx_metrics_container& m, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "num_sdus={} num_sdu_bytes={} num_dropped_sdus={} num_pdus={} num_pdu_bytes={} "
                     "num_discard_timeouts={} avg_pdu_latency={:.2f}us "
                     "pdu_latency_hist=[{}] min_pdu_latency={}{} max_pdu_latency={}{} avg_crypto_latency={:.2f}us",
                     m.num_sdus,
                     m.num_sdu_bytes,
                     m.num_dropped_sdus,
                     m.num_pdus,
                     m.num_pdu_bytes,
                     m.num_discard_timeouts,
                     m.num_pdus > 0 ? m.sum_pdu_latency_ns / m.num_pdus * 1e-3 : 0,
                     fmt::join(m.pdu_latency_hist, " "),
                     m.min_pdu_latency_ns,
                     m.min_pdu_latency_ns.has_value() ? "ns" : "",
                     m.max_pdu_latency_ns,
                     m.max_pdu_latency_ns.has_value() ? "ns" : "",
                     m.num_pdus > 0 ? m.sum_crypto_processing_latency_ns / m.num_pdus * 1e-3 : 0);
  }
};
} // namespace fmt
