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

/// LDPC decoder metric decorator.
class phy_metrics_ldpc_decoder_decorator : public ldpc_decoder
{
public:
  /// Creates an LDPC decoder decorator from a base LDPC decoder instance and a metric notifier.
  phy_metrics_ldpc_decoder_decorator(std::unique_ptr<ldpc_decoder> base_decoder_,
                                     ldpc_decoder_metric_notifier& notifier_) :
    base_decoder(std::move(base_decoder_)), notifier(notifier_)
  {
    srsran_assert(base_decoder, "Invalid decoder.");
  }

  // See interface for documentation.
  std::optional<unsigned> decode(bit_buffer&                      output,
                                 span<const log_likelihood_ratio> input,
                                 crc_calculator*                  crc,
                                 const configuration&             cfg) override
  {
    ldpc_decoder_metrics    metrics;
    std::optional<unsigned> ret;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      ret = base_decoder->decode(output, input, crc, cfg);
    }
    metrics.cb_sz          = units::bits(output.size());
    metrics.nof_iterations = ret.value_or(cfg.algorithm_conf.max_iterations);
    metrics.crc_ok         = ret.has_value();

    notifier.on_new_metric(metrics);

    return ret;
  }

private:
  std::unique_ptr<ldpc_decoder> base_decoder;
  ldpc_decoder_metric_notifier& notifier;
};

} // namespace srsran
