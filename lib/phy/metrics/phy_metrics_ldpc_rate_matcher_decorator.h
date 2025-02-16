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
#include "srsran/support/resource_usage/scoped_resource_usage.h"
#include <memory>

namespace srsran {

/// LDPC rate matcher metric decorator.
class phy_metrics_ldpc_rate_matcher_decorator : public ldpc_rate_matcher
{
public:
  /// Creates an LDPC rate matcher from a base instance and a metric notifier.
  phy_metrics_ldpc_rate_matcher_decorator(std::unique_ptr<ldpc_rate_matcher> base_rate_matcher_,
                                          ldpc_rate_matcher_metric_notifier& notifier_) :
    base(std::move(base_rate_matcher_)), notifier(notifier_)
  {
    srsran_assert(base, "Invalid decoder.");
  }

  // See interface for documentation.
  void rate_match(bit_buffer& output, const ldpc_encoder_buffer& input, const codeblock_metadata& cfg) override
  {
    ldpc_rate_matcher_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base->rate_match(output, input, cfg);
    }
    metrics.output_size = units::bits(output.size());
    notifier.on_new_metric(metrics);
  }

private:
  std::unique_ptr<ldpc_rate_matcher> base;
  ldpc_rate_matcher_metric_notifier& notifier;
};

} // namespace srsran
