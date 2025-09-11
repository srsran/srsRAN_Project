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
#include "srsran/phy/support/time_alignment_estimator/time_alignment_estimator.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/support/resource_usage/scoped_resource_usage.h"

namespace srsran {

/// Time alignment estimator metric decorator.
class phy_metrics_time_alignment_estimator_decorator : public time_alignment_estimator
{
public:
  /// Creates a time alignment estimator decorator from a base instance and a metric notifier.
  phy_metrics_time_alignment_estimator_decorator(std::unique_ptr<time_alignment_estimator> base_,
                                                 time_alignment_estimator_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    srsran_assert(base, "Invalid base instance.");
  }

  // See interface for documentation.
  time_alignment_measurement estimate(span<const cf_t>                       symbols,
                                      const bounded_bitset<max_nof_symbols>& mask,
                                      subcarrier_spacing                     scs,
                                      double                                 max_ta) override
  {
    time_alignment_estimator_metrics metrics;
    time_alignment_measurement       ret;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      ret = base->estimate(symbols, mask, scs, max_ta);
    }
    metrics.nof_re = static_cast<unsigned>(symbols.size());
    notifier.on_new_metric(metrics);

    return ret;
  }

  // See interface for documentation.
  time_alignment_measurement estimate(const re_buffer_reader<cf_t>&          symbols,
                                      const bounded_bitset<max_nof_symbols>& mask,
                                      subcarrier_spacing                     scs,
                                      double                                 max_ta) override
  {
    time_alignment_estimator_metrics metrics;
    time_alignment_measurement       ret;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      ret = base->estimate(symbols, mask, scs, max_ta);
    }
    metrics.nof_re = static_cast<unsigned>(symbols.get_nof_re());
    notifier.on_new_metric(metrics);

    return ret;
  }

  // See interface for documentation.
  time_alignment_measurement
  estimate(span<const cf_t> symbols, unsigned stride, subcarrier_spacing scs, double max_ta) override
  {
    time_alignment_estimator_metrics metrics;
    time_alignment_measurement       ret;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      ret = base->estimate(symbols, stride, scs, max_ta);
    }
    metrics.nof_re = static_cast<unsigned>(symbols.size());
    notifier.on_new_metric(metrics);

    return ret;
  }

  // See interface for documentation.
  time_alignment_measurement
  estimate(const re_buffer_reader<cf_t>& symbols, unsigned stride, subcarrier_spacing scs, double max_ta) override
  {
    time_alignment_estimator_metrics metrics;
    time_alignment_measurement       ret;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      ret = base->estimate(symbols, stride, scs, max_ta);
    }
    metrics.nof_re = static_cast<unsigned>(symbols.get_nof_re());
    notifier.on_new_metric(metrics);

    return ret;
  }

private:
  std::unique_ptr<time_alignment_estimator> base;
  time_alignment_estimator_metric_notifier& notifier;
};

} // namespace srsran
