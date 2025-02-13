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

#include "srsran/phy/generic_functions/transform_precoding/transform_precoder.h"
#include "srsran/phy/metrics/phy_metrics_notifiers.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/support/resource_usage/scoped_resource_usage.h"

namespace srsran {

/// Transform precoder metric decorator.
class phy_metrics_transform_precoder_decorator : public transform_precoder
{
public:
  /// Creates a transform precoder decorator from a base instance and a metric notifier.
  phy_metrics_transform_precoder_decorator(std::unique_ptr<transform_precoder> base_,
                                           transform_precoder_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    srsran_assert(base, "Invalid base instance.");
  }

  // See interface for documentation.
  void deprecode_ofdm_symbol(span<cf_t> out, span<const cf_t> in) override
  {
    transform_precoder_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.cpu_measurements,
                                                                 resource_usage_utils::rusage_measurement_type::THREAD);

      auto tp_before = std::chrono::high_resolution_clock::now();
      base->deprecode_ofdm_symbol(out, in);
      auto tp_after = std::chrono::high_resolution_clock::now();

      metrics.elapsed = tp_after - tp_before;
    }
    metrics.nof_re = static_cast<unsigned>(out.size());
    notifier.on_new_metric(metrics);
  }

  // See interface for documentation.
  void deprecode_ofdm_symbol_noise(span<float> out, span<const float> in) override
  {
    base->deprecode_ofdm_symbol_noise(out, in);
  }

private:
  std::unique_ptr<transform_precoder> base;
  transform_precoder_metric_notifier& notifier;
};

} // namespace srsran
