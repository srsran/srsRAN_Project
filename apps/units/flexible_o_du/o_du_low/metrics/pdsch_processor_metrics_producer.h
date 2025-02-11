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

/// PDSCH processor metric producer.
class pdsch_processor_metric_producer_impl : private pdsch_processor_metric_notifier
{
public:
  /// Gets the PDSCH processor metric interface.
  pdsch_processor_metric_notifier& get_notifier() { return *this; }

  /// Gets the total number of processed codeblocks.
  uint64_t get_nof_processed_cb() const { return count; }

  /// Gets the average transmission latency in microseconds.
  double get_avg_latency_us() const
  {
    return static_cast<double>(sum_elapsed_completion_ns) / static_cast<double>(count) * 1e-3;
  }

  /// \brief Gets the average return time in microseconds.
  ///
  /// It is the average processing time of a single-thread PDSCH processor.
  double get_avg_return_time_us() const
  {
    return static_cast<double>(sum_elapsed_return_ns) / static_cast<double>(count) * 1e-3;
  }

  /// Gets the processing rate.
  double get_process_rate_Mbps() const
  {
    return static_cast<double>(sum_tb_sz) / static_cast<double>(sum_elapsed_completion_ns) * 1000;
  }

  /// Gets the total amount of time the PDSCH processor spent processing.
  std::chrono::nanoseconds get_total_time() const { return std::chrono::nanoseconds(sum_elapsed_completion_ns); }

private:
  // See interface for documentation.
  void new_metric(const pdsch_processor_metrics& metrics) override
  {
    sum_tb_sz += metrics.tbs.to_bits().value();
    sum_elapsed_return_ns += metrics.elapsed_return.count();
    sum_elapsed_completion_ns += metrics.elapsed_completion.count();
    ++count;
  }

  std::atomic<uint64_t> sum_tb_sz                 = {};
  std::atomic<uint64_t> sum_elapsed_return_ns     = {};
  std::atomic<uint64_t> sum_elapsed_completion_ns = {};
  std::atomic<uint64_t> count                     = {};
};

} // namespace srsran
