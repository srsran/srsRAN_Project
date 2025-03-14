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

/// PUSCH demodulator metrics aggregator.
class pusch_demodulator_metrics_aggregator : public pusch_demodulator_metric_notifier
{
public:
  /// Gets the PUSCH demodulator average processing time in us.
  double get_average_processing_time_us() const
  {
    return count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_elapsed_ns) / static_cast<double>(count) / 1000.0
               : 0;
  }

  /// Gets the PUSCH demodulator total processing time excluding buffer operations in us.
  std::chrono::nanoseconds get_total_time() const
  {
    return std::chrono::nanoseconds(sum_elapsed_ns - sum_elapsed_buffer_ns);
  }

  /// Gets the CPU usage in microseconds of the PUSCH demodulator.
  double get_cpu_usage_us() const { return static_cast<double>(sum_elapsed_ns) / 1000.0; }

  /// Resets values of all internal counters.
  void reset()
  {
    count                 = 0;
    sum_elapsed_ns        = 0;
    sum_elapsed_buffer_ns = 0;
  }

private:
  // See interface for documentation.
  void on_new_metric(const pusch_demodulator_metrics& metrics) override
  {
    sum_elapsed_ns += metrics.measurements.duration.count();
    sum_elapsed_buffer_ns += metrics.elapsed_buffer.count();
    ++count;
  }

  std::atomic<uint64_t> sum_elapsed_ns        = {};
  std::atomic<uint64_t> sum_elapsed_buffer_ns = {};
  std::atomic<uint64_t> count                 = {};
};

} // namespace srsran
