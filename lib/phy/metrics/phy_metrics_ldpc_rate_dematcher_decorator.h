/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/phy/metrics/phy_metrics_notifiers.h"
#include "srsran/phy/metrics/phy_metrics_reports.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_decoder.h"
#include "srsran/support/resource_usage/scoped_resource_usage.h"
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
    ldpc_rate_dematcher_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base->rate_dematch(output, input, new_data, cfg);
    }
    metrics.input_size = units::bits(input.size());
    notifier.on_new_metric(metrics);
  }

private:
  std::unique_ptr<ldpc_rate_dematcher> base;
  ldpc_rate_dematcher_metric_notifier& notifier;
};

} // namespace srsran
