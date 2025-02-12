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
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_decoder.h"
#include <memory>

namespace srsran {

/// LDPC rate dematcher metric decorator.
class phy_metrics_ldpc_rate_dematcher_decorator : public ldpc_rate_dematcher
{
public:
  /// Creates an LDPC rate dematcher from a base instance and a metric notifier.
  phy_metrics_ldpc_rate_dematcher_decorator(std::unique_ptr<ldpc_rate_dematcher> base_rate_dematcher_,
                                            ldpc_rate_dematcher_metric_notifier& notifier_) :
    base(std::move(base_rate_dematcher_)), notifier(notifier_)
  {
    srsran_assert(base, "Invalid decoder.");
  }

  // See interface for documentation.
  void rate_dematch(span<log_likelihood_ratio>       output,
                    span<const log_likelihood_ratio> input,
                    bool                             new_data,
                    const codeblock_metadata&        cfg) override
  {
    auto tp_before = std::chrono::high_resolution_clock::now();
    base->rate_dematch(output, input, new_data, cfg);
    auto tp_after = std::chrono::high_resolution_clock::now();

    notifier.on_new_metric({.input_size = units::bits(input.size()), .elapsed = tp_after - tp_before});
  }

private:
  std::unique_ptr<ldpc_rate_dematcher> base;
  ldpc_rate_dematcher_metric_notifier& notifier;
};

} // namespace srsran
