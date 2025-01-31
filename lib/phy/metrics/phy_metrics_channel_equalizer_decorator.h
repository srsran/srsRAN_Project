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
#include "srsran/phy/upper/equalization/channel_equalizer.h"
#include <memory>

namespace srsran {

/// Channel precoder metric decorator.
class phy_metrics_channel_equalizer_decorator : public channel_equalizer
{
public:
  /// Creates a channel equalizer decorator from a base instance and a metric notifier.
  phy_metrics_channel_equalizer_decorator(std::unique_ptr<channel_equalizer> base_equalizer_,
                                          channel_equalizer_metric_notifier& notifier_) :
    base_equalizer(std::move(base_equalizer_)), notifier(notifier_)
  {
    srsran_assert(base_equalizer, "Invalid channel equalizer.");
  }

  // See interface for documentation.
  void equalize(span<cf_t>                       eq_symbols,
                span<float>                      eq_noise_vars,
                const re_buffer_reader<cbf16_t>& ch_symbols,
                const ch_est_list&               ch_estimates,
                span<const float>                noise_var_estimates,
                float                            tx_scaling) override
  {
    auto tp_before = std::chrono::high_resolution_clock::now();
    base_equalizer->equalize(eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var_estimates, tx_scaling);
    auto tp_after = std::chrono::high_resolution_clock::now();

    notifier.new_metric({.nof_re     = ch_estimates.get_nof_re(),
                         .nof_layers = ch_estimates.get_nof_tx_layers(),
                         .nof_ports  = ch_estimates.get_nof_rx_ports(),
                         .elapsed    = tp_after - tp_before});
  }

  // See interface for documentation.
  bool is_supported(unsigned nof_ports, unsigned nof_layers) override
  {
    return base_equalizer->is_supported(nof_ports, nof_layers);
  }

private:
  std::unique_ptr<channel_equalizer> base_equalizer;
  channel_equalizer_metric_notifier& notifier;
};

} // namespace srsran
