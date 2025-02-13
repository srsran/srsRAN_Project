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

#include "srsran/phy/metrics/phy_metrics_notifiers.h"
#include "srsran/phy/metrics/phy_metrics_reports.h"
#include <atomic>

namespace srsran {

/// CRC calculator metrics aggregator.
class crc_calculator_metrics_aggregator : public crc_calculator_metric_notifier
{
public:
  /// Gets the average codeblock latency in microseconds.
  double get_avg_latency_us(crc_generator_poly poly) const
  {
    const metrics_per_polynomial& metrics_poly = select_metrics(poly);
    return metrics_poly.count.load(std::memory_order_relaxed)
               ? static_cast<double>(metrics_poly.sum_elapsed_ns) / static_cast<double>(metrics_poly.count) * 1e-3
               : 0;
  }

  /// Gets the processing rate.
  double get_avg_rate_Mbps(crc_generator_poly poly) const
  {
    const metrics_per_polynomial& metrics_poly = select_metrics(poly);
    return metrics_poly.sum_elapsed_ns.load(std::memory_order_relaxed)
               ? static_cast<double>(metrics_poly.sum_nof_bits) / static_cast<double>(metrics_poly.sum_elapsed_ns) *
                     1000
               : 0;
  }

  /// Gets the total amount of time the CRC calculator spent calculating.
  std::chrono::nanoseconds get_total_time(crc_generator_poly poly) const
  {
    const metrics_per_polynomial& metrics_poly = select_metrics(poly);
    return std::chrono::nanoseconds(metrics_poly.sum_elapsed_ns);
  }

  /// Gets the CPU usage in microseconds of the CRC calculation.
  uint64_t get_cpu_usage_us() const { return sum_used_cpu_time_us; }

  /// Resets values of all internal counters.
  void reset()
  {
    crc16_metrics_collection.reset();
    crc24A_metrics_collection.reset();
    crc24B_metrics_collection.reset();
    other_metrics_collection.reset();
    sum_used_cpu_time_us = 0;
  }

private:
  /// Groups all metrics of a single polynomial.
  struct metrics_per_polynomial {
    std::atomic<uint64_t> sum_nof_bits   = {};
    std::atomic<uint64_t> sum_elapsed_ns = {};
    std::atomic<uint64_t> count          = {};

    /// Reset all values to 0.
    void reset()
    {
      sum_nof_bits   = 0;
      sum_elapsed_ns = 0;
      count          = 0;
    }
  };

  /// Selects the CRC calculator metrics depending on the polynomial for read-write operations.
  metrics_per_polynomial& select_metrics(crc_generator_poly poly)
  {
    switch (poly) {
      case crc_generator_poly::CRC24A:
        return crc24A_metrics_collection;
      case crc_generator_poly::CRC24B:
        return crc24B_metrics_collection;
      case crc_generator_poly::CRC16:
        return crc16_metrics_collection;
      case crc_generator_poly::CRC24C:
      case crc_generator_poly::CRC11:
      case crc_generator_poly::CRC6:
      default:
        return other_metrics_collection;
    }
  }

  /// Selects the CRC calculator metrics depending on the polynomial for read-only operations.
  const metrics_per_polynomial& select_metrics(crc_generator_poly poly) const
  {
    switch (poly) {
      case crc_generator_poly::CRC24A:
        return crc24A_metrics_collection;
      case crc_generator_poly::CRC24B:
        return crc24B_metrics_collection;
      case crc_generator_poly::CRC16:
        return crc16_metrics_collection;
      case crc_generator_poly::CRC24C:
      case crc_generator_poly::CRC11:
      case crc_generator_poly::CRC6:
      default:
        return other_metrics_collection;
    }
  }

  // See interface for documentation.
  void on_new_metric(const crc_calculator_metrics& metrics) override
  {
    metrics_per_polynomial& metrics_poly = select_metrics(metrics.poly);
    metrics_poly.sum_nof_bits += metrics.nof_bits.value();
    metrics_poly.sum_elapsed_ns += metrics.elapsed.count();
    ++metrics_poly.count;

    sum_used_cpu_time_us += (metrics.cpu_measurements.user_time.count() + metrics.cpu_measurements.system_time.count());
  }

  metrics_per_polynomial crc16_metrics_collection;
  metrics_per_polynomial crc24A_metrics_collection;
  metrics_per_polynomial crc24B_metrics_collection;
  metrics_per_polynomial other_metrics_collection;

  std::atomic<uint64_t> sum_used_cpu_time_us = {};
};

} // namespace srsran
