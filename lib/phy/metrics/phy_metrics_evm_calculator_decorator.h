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
#include "srsran/phy/upper/channel_modulation/evm_calculator.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/support/resource_usage/scoped_resource_usage.h"

namespace srsran {

/// EVM calculator metric decorator.
class phy_metrics_evm_calculator_decorator : public evm_calculator
{
public:
  /// Creates an EVM decorator from a base instance and a metric notifier.
  phy_metrics_evm_calculator_decorator(std::unique_ptr<evm_calculator>            base_,
                                       common_channel_modulation_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    srsran_assert(base, "Invalid base instance.");
  }

  // See interface for documentation.
  float
  calculate(span<const log_likelihood_ratio> soft_bits, span<const cf_t> symbols, modulation_scheme modulation) override
  {
    evm_calculator_metrics metrics;
    float                  ret;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.cpu_measurements,
                                                                 resource_usage_utils::rusage_measurement_type::THREAD);

      auto tp_before = std::chrono::high_resolution_clock::now();
      ret            = base->calculate(soft_bits, symbols, modulation);
      auto tp_after  = std::chrono::high_resolution_clock::now();

      metrics.elapsed = tp_after - tp_before;
    }
    metrics.modulation  = modulation;
    metrics.nof_symbols = static_cast<unsigned>(symbols.size());
    notifier.on_new_metric(metrics);

    return ret;
  }

private:
  std::unique_ptr<evm_calculator>            base;
  common_channel_modulation_metric_notifier& notifier;
};

} // namespace srsran
