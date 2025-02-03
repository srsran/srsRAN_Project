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
#include "srsran/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsran/phy/upper/unique_rx_buffer.h"

namespace srsran {

/// Demodulation mapper metric decorator.
class phy_metrics_demodulation_mapper_decorator : public demodulation_mapper
{
public:
  /// Creates a demodulation mapper decorator from a base instance and metric notifier.
  phy_metrics_demodulation_mapper_decorator(std::unique_ptr<demodulation_mapper>       base_,
                                            common_channel_modulation_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    srsran_assert(base, "Invalid base instance.");
  }

  // See interface for documentation.
  void demodulate_soft(span<log_likelihood_ratio> llrs,
                       span<const cf_t>           symbols,
                       span<const float>          noise_vars,
                       modulation_scheme          mod) override
  {
    auto tp_before = std::chrono::high_resolution_clock::now();

    base->demodulate_soft(llrs, symbols, noise_vars, mod);

    auto tp_after = std::chrono::high_resolution_clock::now();

    notifier.new_metric(
        {.modulation = mod, .nof_symbols = static_cast<unsigned>(symbols.size()), .elapsed = tp_after - tp_before});
  }

private:
  std::unique_ptr<demodulation_mapper>       base;
  common_channel_modulation_metric_notifier& notifier;
};

} // namespace srsran
