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

#include "../support/metrics_helpers.h"
#include "srsran/ofh/ethernet/ethernet_transmitter_metrics.h"
#include "srsran/ofh/ethernet/ethernet_transmitter_metrics_collector.h"
#include "srsran/support/units.h"
#include <atomic>
#include <chrono>

namespace srsran {
namespace ether {

/// Open Fronthaul Ethernet receiver metrics collector implementation.
class transmitter_metrics_collector_impl : public transmitter_metrics_collector
{
public:
  explicit transmitter_metrics_collector_impl(bool enabled_) : enabled(enabled_) {}

  // See interface for documentation.
  void collect_metrics(transmitter_metrics& metrics) override
  {
    if (disabled()) {
      return;
    }
    uint64_t sum_nof_bytes    = sum_tx_bytes.load(std::memory_order_relaxed);
    uint64_t sum_tx_time      = sum_tx_time_ns.load(std::memory_order_relaxed);
    uint32_t total_tx_packets = total_nof_packets.load(std::memory_order_relaxed);

    metrics.avg_rate_Mbps =
        sum_tx_time ? static_cast<double>(units::bytes(sum_nof_bytes).to_bits().value()) / sum_tx_time * 1000.0 : 0.f;
    metrics.avg_packet_tx_latency_us =
        total_tx_packets ? static_cast<float>(sum_tx_time) / total_tx_packets / 1000.f : 0.f;

    metrics.max_packet_tx_latency_us = static_cast<float>(max_tx_latency_ns.load(std::memory_order_relaxed)) / 1000.f;
    metrics.min_packet_tx_latency_us = static_cast<float>(min_tx_latency_ns.load(std::memory_order_relaxed)) / 1000.f;
    metrics.cpu_usage_us             = static_cast<float>(sum_tx_time) / 1000.f;
    metrics.total_nof_bytes          = sum_nof_bytes;

    reset_stats();
  }

  /// Returns true if the metrics collector is disabled, false otherwise.
  bool disabled() const { return !enabled; }

  /// Returns new measurement object.
  ofh::time_execution_measurer create_time_execution_measurer() const
  {
    return ofh::time_execution_measurer(!disabled());
  }

  /// Updates internal statistics.
  void update_stats(std::chrono::nanoseconds elapsed_ns, uint64_t nof_bytes = 0, uint64_t nof_packets = 1)
  {
    if (disabled()) {
      return;
    }

    sum_tx_time_ns.fetch_add(elapsed_ns.count(), std::memory_order_relaxed);
    if (nof_bytes > 0) {
      sum_tx_bytes.fetch_add(nof_bytes, std::memory_order_relaxed);
      total_nof_packets.fetch_add(nof_packets, std::memory_order_relaxed);
      ofh::update_minmax(static_cast<uint32_t>(elapsed_ns.count() / nof_packets), max_tx_latency_ns, min_tx_latency_ns);
    }
  }

private:
  static constexpr uint32_t default_min_latency_ns = std::numeric_limits<uint32_t>::max();
  static constexpr uint32_t default_max_latency_ns = std::numeric_limits<uint32_t>::min();

  /// Resets values of all internal counters.
  void reset_stats()
  {
    sum_tx_time_ns.store(0, std::memory_order_relaxed);
    sum_tx_bytes.store(0, std::memory_order_relaxed);
    total_nof_packets.store(0, std::memory_order_relaxed);
    min_tx_latency_ns.store(default_min_latency_ns, std::memory_order_relaxed);
    max_tx_latency_ns.store(default_max_latency_ns, std::memory_order_relaxed);
  }

  const bool            enabled;
  std::atomic<uint64_t> sum_tx_bytes      = {};
  std::atomic<uint32_t> total_nof_packets = {};
  std::atomic<uint64_t> sum_tx_time_ns    = {};
  std::atomic<uint32_t> min_tx_latency_ns = default_min_latency_ns;
  std::atomic<uint32_t> max_tx_latency_ns = default_max_latency_ns;
};

} // namespace ether
} // namespace srsran
