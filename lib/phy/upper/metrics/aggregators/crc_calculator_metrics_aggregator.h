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
               ? static_cast<double>(metrics_poly.sum_elapsed_ns.load(std::memory_order_relaxed)) /
                     static_cast<double>(metrics_poly.count.load(std::memory_order_relaxed)) * 1e-3
               : 0;
  }

  /// Gets the processing rate.
  double get_avg_rate_Mbps(crc_generator_poly poly) const
  {
    const metrics_per_polynomial& metrics_poly = select_metrics(poly);
    return metrics_poly.sum_elapsed_ns.load(std::memory_order_relaxed)
               ? static_cast<double>(metrics_poly.sum_nof_bits.load(std::memory_order_relaxed)) /
                     static_cast<double>(metrics_poly.sum_elapsed_ns.load(std::memory_order_relaxed)) * 1000
               : 0;
  }

  /// Gets the total amount of time the CRC calculator spent calculating.
  std::chrono::nanoseconds get_total_time(crc_generator_poly poly) const
  {
    const metrics_per_polynomial& metrics_poly = select_metrics(poly);
    return std::chrono::nanoseconds(metrics_poly.sum_elapsed_ns.load(std::memory_order_relaxed));
  }

  /// Gets the CPU usage in microseconds of the CRC calculation.
  double get_cpu_usage_us() const
  {
    return static_cast<double>(crc16_metrics_collection.sum_elapsed_ns.load(std::memory_order_relaxed) +
                               crc24A_metrics_collection.sum_elapsed_ns.load(std::memory_order_relaxed) +
                               crc24B_metrics_collection.sum_elapsed_ns.load(std::memory_order_relaxed) +
                               other_metrics_collection.sum_elapsed_ns.load(std::memory_order_relaxed)) /
           1000.0;
  }

  /// Resets values of all internal counters.
  void reset()
  {
    crc16_metrics_collection.reset();
    crc24A_metrics_collection.reset();
    crc24B_metrics_collection.reset();
    other_metrics_collection.reset();
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
      sum_nof_bits.store(0, std::memory_order_relaxed);
      sum_elapsed_ns.store(0, std::memory_order_relaxed);
      count.store(0, std::memory_order_relaxed);
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
    metrics_poly.sum_nof_bits.fetch_add(metrics.nof_bits.value(), std::memory_order_relaxed);
    metrics_poly.sum_elapsed_ns.fetch_add(metrics.measurements.duration.count(), std::memory_order_relaxed);
    metrics_poly.count.fetch_add(1, std::memory_order_relaxed);
  }

  metrics_per_polynomial crc16_metrics_collection;
  metrics_per_polynomial crc24A_metrics_collection;
  metrics_per_polynomial crc24B_metrics_collection;
  metrics_per_polynomial other_metrics_collection;
};

} // namespace srsran
