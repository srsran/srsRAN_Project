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

namespace srsran {

/// EVM calculator metric decorator.
class phy_metrics_evm_calculator_decorator : public evm_calculator
{
public:
  /// Creates an EVM decorator from a base instance and metric notifier.
  phy_metrics_evm_calculator_decorator(std::unique_ptr<evm_calculator>     base_,
                                       channel_modulation_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    srsran_assert(base, "Invalid base instance.");
  }

  // See interface for documentation.
  float
  calculate(span<const log_likelihood_ratio> soft_bits, span<const cf_t> symbols, modulation_scheme modulation) override
  {
    auto tp_before = std::chrono::high_resolution_clock::now();

    float ret = base->calculate(soft_bits, symbols, modulation);

    auto tp_after = std::chrono::high_resolution_clock::now();

    notifier.new_metric({.modulation  = modulation,
                         .nof_symbols = static_cast<unsigned>(symbols.size()),
                         .elapsed     = tp_after - tp_before});

    return ret;
  }

private:
  std::unique_ptr<evm_calculator>     base;
  channel_modulation_metric_notifier& notifier;
};

} // namespace srsran
