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

/// PUSCH demodulator metric producer.
class pusch_demodulator_metric_producer_impl : private pusch_demodulator_metric_notifier
{
public:
  /// Gets the PUSCH demodulator metric notifier.
  pusch_demodulator_metric_notifier& get_notifier() { return *this; }

  /// Gets the PUSCH demodulator average processing time in us.
  double get_average_processing_time_us() const
  {
    return static_cast<double>(sum_elapsed_ns) / static_cast<double>(count) / 1000.0;
  }

  /// Gets the PUSCH demodulator total processing time excluding buffer operations in us.
  std::chrono::nanoseconds get_total_time() const
  {
    return std::chrono::nanoseconds(sum_elapsed_ns - sum_elapsed_buffer_ns);
  }

private:
  // See interface for documentation.
  void new_metric(const pusch_demodulator_metrics& metrics) override
  {
    sum_elapsed_ns += metrics.elapsed.count();
    sum_elapsed_buffer_ns += metrics.elapsed_buffer.count();
    ++count;
  }

  std::atomic<uint64_t> sum_elapsed_ns        = {};
  std::atomic<uint64_t> sum_elapsed_buffer_ns = {};
  std::atomic<uint64_t> count                 = {};
};

} // namespace srsran
