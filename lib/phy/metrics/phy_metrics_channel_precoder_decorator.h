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

#include "srsran/phy/generic_functions/precoding/channel_precoder.h"
#include "srsran/phy/metrics/phy_metrics_notifiers.h"
#include "srsran/phy/metrics/phy_metrics_reports.h"
#include "srsran/support/resource_usage/scoped_resource_usage.h"
#include <memory>

namespace srsran {

/// Channel precoder metric decorator.
class phy_metrics_channel_precoder_decorator : public channel_precoder
{
public:
  /// Creates a channel precoder decorator from a base instance and a metric notifier.
  phy_metrics_channel_precoder_decorator(std::unique_ptr<channel_precoder> base_precoder_,
                                         channel_precoder_metric_notifier& notifier_) :
    base_precoder(std::move(base_precoder_)), notifier(notifier_)
  {
    srsran_assert(base_precoder, "Invalid channel precoder.");
  }

  // See interface for documentation.
  void apply_precoding(re_buffer_writer<cbf16_t>&     output,
                       const re_buffer_reader<>&      input,
                       const precoding_weight_matrix& precoding) const override
  {
    channel_precoder_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      // Call base precoder.
      base_precoder->apply_precoding(output, input, precoding);
    }
    metrics.method     = channel_precoder_metrics::methods::apply_precoding;
    metrics.nof_re     = output.get_nof_re();
    metrics.nof_layers = precoding.get_nof_layers();
    notifier.on_new_metric(metrics);
  }

  // See interface for documentation.
  void apply_layer_map_and_precoding(re_buffer_writer<cbf16_t>&     output,
                                     span<const ci8_t>              input,
                                     const precoding_weight_matrix& precoding) const override
  {
    channel_precoder_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base_precoder->apply_layer_map_and_precoding(output, input, precoding);
    }
    // Report metrics.
    metrics.method     = channel_precoder_metrics::methods::apply_layer_map_and_precoding;
    metrics.nof_re     = output.get_nof_re();
    metrics.nof_layers = precoding.get_nof_layers();
    metrics.nof_ports  = precoding.get_nof_ports();
    notifier.on_new_metric(metrics);
  }

private:
  std::unique_ptr<channel_precoder> base_precoder;
  channel_precoder_metric_notifier& notifier;
};

} // namespace srsran
