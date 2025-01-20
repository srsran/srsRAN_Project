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
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/support/math/math_utils.h"
#include <atomic>

namespace srsran {

/// Pseudo-random sequence generator metric producer.
class pseudo_random_sequence_metrics_producer_impl : private pseudo_random_sequence_generator_metric_notifier
{
public:
  /// Gets the scrambling metric notifier.
  pseudo_random_sequence_generator_metric_notifier& get_notifier() { return *this; }

  /// Gets the average initialization time in microseconds.
  double get_average_init_time_us() const
  {
    return static_cast<double>(sum_init_elapsed_ns) / static_cast<double>(init_count) / 1000.0;
  }

  /// Gets the average advance rate in Mbps.
  double get_advance_rate_Mbps() const
  {
    return static_cast<double>(sum_advance_nof_bits) / static_cast<double>(sum_advance_elapsed_ns) * 1000.0;
  }

  /// Gets the average generation rate in Mbps.
  double get_generate_rate_Mbps() const
  {
    return static_cast<double>(sum_nof_bits) / static_cast<double>(sum_elapsed_ns) * 1000.0;
  }

  /// Gets the total time spent by the scrambler.
  std::chrono::nanoseconds get_total_time() const { return std::chrono::nanoseconds(sum_elapsed_ns); }

private:
  // See interface for documentation.
  void new_metric(const pseudo_random_sequence_generator_metrics& metrics) override
  {
    switch (metrics.method) {
      case pseudo_random_sequence_generator_metrics::methods::init:
        sum_init_elapsed_ns += metrics.elapsed.count();
        ++init_count;
        break;
      case pseudo_random_sequence_generator_metrics::methods::advance:
        sum_advance_nof_bits += metrics.nof_bits;
        sum_advance_elapsed_ns += metrics.elapsed.count();
        break;
      case pseudo_random_sequence_generator_metrics::methods::apply_xor_soft_bit:
      case pseudo_random_sequence_generator_metrics::methods::apply_xor_unpacked:
      case pseudo_random_sequence_generator_metrics::methods::apply_xor_packed:
      case pseudo_random_sequence_generator_metrics::methods::generate_bit_packed:
      case pseudo_random_sequence_generator_metrics::methods::generate_float:
        sum_nof_bits += metrics.nof_bits;
        sum_elapsed_ns += metrics.elapsed.count();
        break;
    }
    ++count;
  }

  std::atomic<uint64_t> sum_init_elapsed_ns    = {};
  std::atomic<uint64_t> init_count             = {};
  std::atomic<uint64_t> sum_advance_nof_bits   = {};
  std::atomic<uint64_t> sum_advance_elapsed_ns = {};
  std::atomic<uint64_t> sum_nof_bits           = {};
  std::atomic<uint64_t> sum_elapsed_ns         = {};
  std::atomic<uint64_t> count                  = {};
};

} // namespace srsran