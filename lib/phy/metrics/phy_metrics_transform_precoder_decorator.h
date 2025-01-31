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
    auto tp_before = std::chrono::high_resolution_clock::now();
    base->deprecode_ofdm_symbol(out, in);
    auto tp_after = std::chrono::high_resolution_clock::now();

    notifier.new_metric({.elapsed = tp_after - tp_before, .nof_re = static_cast<unsigned>(out.size())});
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
