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

#include "aggregator_helpers.h"
#include "srsran/phy/metrics/phy_metrics_notifiers.h"
#include "srsran/phy/metrics/phy_metrics_reports.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/support/math/math_utils.h"
#include <atomic>

namespace srsran {

/// PUSCH processor metrics aggregator.
class pusch_processor_metrics_aggregator : public pusch_processor_metric_notifier
{
public:
  /// Gets the average data processing latency in microseconds.
  double get_avg_data_latency_us() const
  {
    return count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_data_elapsed_ns.load(std::memory_order_relaxed)) /
                     static_cast<double>(count.load(std::memory_order_relaxed)) * 1e-3
               : 0;
  }

  /// Gets the minimum data processing latency in microseconds.
  double get_min_data_latency_us() const
  {
    uint64_t min_data_latency_ns = unpack_duration(packed_min_data_latency_ns.load(std::memory_order_relaxed));
    return (min_data_latency_ns == unpack_duration(default_packed_min_latency_ns))
               ? 0
               : static_cast<double>(min_data_latency_ns) / 1000.0;
  }

  /// Gets the maximum transmission latency in microseconds and the slot point in which was detected.
  std::pair<double, slot_point> get_max_data_latency_us() const
  {
    uint64_t   max_data_latency_ns = unpack_duration(packed_max_data_latency_ns.load(std::memory_order_relaxed));
    slot_point slot                = unpack_slot(packed_max_data_latency_ns.load(std::memory_order_relaxed));
    return {static_cast<double>(max_data_latency_ns) / 1000.0, slot};
  }

  /// Gets the minimum UCI processing latency in microseconds.
  double get_min_uci_latency_us() const
  {
    return (min_uci_latency_ns.load(std::memory_order_relaxed) == UINT64_MAX)
               ? 0
               : static_cast<double>(min_uci_latency_ns.load(std::memory_order_relaxed)) / 1000.0;
  }

  /// Gets the maximum UCI processing latency in microseconds.
  double get_max_uci_latency_us() const
  {
    return static_cast<double>(max_uci_latency_ns.load(std::memory_order_relaxed)) / 1000.0;
  }

  /// Gets the average codeblock latency in microseconds.
  double get_avg_uci_latency_us() const
  {
    return uci_count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_uci_elapsed_ns.load(std::memory_order_relaxed)) /
                     static_cast<double>(uci_count.load(std::memory_order_relaxed)) * 1e-3
               : 0;
  }

  /// \brief Gets the average PUSCH processing transmission rate.
  ///
  /// This metric is calculated as the ratio of the total number of processed bits and the total elapsed time.
  double get_processing_rate_Mbps() const
  {
    return sum_data_elapsed_ns.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_tbs.load(std::memory_order_relaxed)) /
                     static_cast<double>(sum_data_elapsed_ns.load(std::memory_order_relaxed)) * 1000
               : 0;
  }

  /// Gets the total time spend by PUSCH processors.
  std::chrono::nanoseconds get_total_time() const
  {
    return std::chrono::nanoseconds(sum_data_elapsed_ns.load(std::memory_order_relaxed));
  }

  /// Gets the total time spent by the PUSCH processors waiting for the asynchronous decoding to be completed.
  std::chrono::nanoseconds get_total_wait_time() const
  {
    return std::chrono::nanoseconds(sum_waiting_ns.load(std::memory_order_relaxed));
  }

  /// Gets the CPU usage in microseconds of scrambling operations.
  double get_cpu_usage_us() const
  {
    return static_cast<double>(sum_used_cpu_time_ns.load(std::memory_order_relaxed)) / 1000.0;
  }

  /// Gets SINR in decibels as a result of channel estimation.
  float get_ch_est_sinr() const { return ch_est_sinr_db.load(std::memory_order_relaxed); }

  /// Gets EVM as a result of channel estimation.
  float get_ch_est_evm() const { return ch_est_evm.load(std::memory_order_relaxed); }

  double get_decoding_bler() const
  {
    return count.load(std::memory_order_relaxed) ? static_cast<double>(count.load(std::memory_order_relaxed) -
                                                                       sum_crc_ok.load(std::memory_order_relaxed)) /
                                                       static_cast<double>(count.load(std::memory_order_relaxed))
                                                 : 0;
  }

  /// Resets values of all internal counters.
  void reset()
  {
    sum_tbs.store(0, std::memory_order_relaxed);
    sum_crc_ok.store(0, std::memory_order_relaxed);
    sum_return_elapsed_ns.store(0, std::memory_order_relaxed);
    sum_data_elapsed_ns.store(0, std::memory_order_relaxed);
    sum_uci_elapsed_ns.store(0, std::memory_order_relaxed);
    sum_waiting_ns.store(0, std::memory_order_relaxed);
    count.store(0, std::memory_order_relaxed);
    uci_count.store(0, std::memory_order_relaxed);
    sum_used_cpu_time_ns.store(0, std::memory_order_relaxed);
    packed_min_data_latency_ns.store(default_packed_min_latency_ns, std::memory_order_relaxed);
    packed_max_data_latency_ns.store(default_packed_min_latency_ns, std::memory_order_relaxed);
    min_uci_latency_ns.store(UINT64_MAX, std::memory_order_relaxed);
    max_uci_latency_ns.store(0, std::memory_order_relaxed);
  }

private:
  static constexpr uint64_t default_packed_min_latency_ns =
      pack_slot_and_duration({}, std::numeric_limits<uint32_t>::max());
  static constexpr uint64_t default_packed_max_latency_ns =
      pack_slot_and_duration({}, std::numeric_limits<uint32_t>::min());

  // See interface for documentation.
  void on_new_metric(const pusch_processor_metrics& metrics) override
  {
    sum_tbs.fetch_add(metrics.tbs.to_bits().value(), std::memory_order_relaxed);
    sum_data_elapsed_ns.fetch_add(metrics.elapsed_data.count(), std::memory_order_relaxed);
    sum_return_elapsed_ns.fetch_add(metrics.elapsed_return.count(), std::memory_order_relaxed);
    if (metrics.elapsed_data > metrics.elapsed_return) {
      sum_waiting_ns.fetch_add(metrics.elapsed_data.count() - metrics.elapsed_return.count(),
                               std::memory_order_relaxed);
    }
    if (metrics.crc_ok) {
      sum_crc_ok.fetch_add(1, std::memory_order_relaxed);
    }
    count.fetch_add(1, std::memory_order_relaxed);
    update_slotmax(metrics.slot, metrics.elapsed_data.count(), packed_max_data_latency_ns);
    update_slotmin(metrics.slot, metrics.elapsed_data.count(), packed_min_data_latency_ns);
    if (metrics.elapsed_uci.has_value()) {
      sum_uci_elapsed_ns.fetch_add(metrics.elapsed_uci->count(), std::memory_order_relaxed);
      update_minmax(metrics.elapsed_uci->count(), max_uci_latency_ns, min_uci_latency_ns);
      uci_count.fetch_add(1, std::memory_order_relaxed);
    }

    sum_used_cpu_time_ns.fetch_add(metrics.cpu_time_usage_ns, std::memory_order_relaxed);

    // Save last channel estimation metrics.
    ch_est_sinr_db.store(metrics.sinr_dB, std::memory_order_relaxed);
    ch_est_evm.store(metrics.evm, std::memory_order_relaxed);
  }

  std::atomic<uint64_t> sum_tbs                    = {};
  std::atomic<uint64_t> sum_crc_ok                 = {};
  std::atomic<uint64_t> sum_return_elapsed_ns      = {};
  std::atomic<uint64_t> sum_data_elapsed_ns        = {};
  std::atomic<uint64_t> sum_uci_elapsed_ns         = {};
  std::atomic<uint64_t> sum_waiting_ns             = {};
  std::atomic<uint64_t> count                      = {};
  std::atomic<uint64_t> uci_count                  = {};
  std::atomic<float>    ch_est_sinr_db             = {};
  std::atomic<float>    ch_est_evm                 = {};
  std::atomic<uint64_t> sum_used_cpu_time_ns       = {};
  std::atomic<uint64_t> packed_min_data_latency_ns = default_packed_min_latency_ns;
  std::atomic<uint64_t> packed_max_data_latency_ns = default_packed_max_latency_ns;
  std::atomic<uint64_t> min_uci_latency_ns         = UINT64_MAX;
  std::atomic<uint64_t> max_uci_latency_ns         = 0;
};

} // namespace srsran
