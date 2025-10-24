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
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_encoder.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_encoder_buffer.h"
#include "srsran/support/resource_usage/scoped_resource_usage.h"
#include <memory>

namespace srsran {

/// LDPC encoder metric decorator.
class phy_metrics_ldpc_encoder_decorator : public ldpc_encoder
{
  /// Dummy LDPC encoder buffer.
  class dummy_encoder_buffer : public ldpc_encoder_buffer
  {
    // See interface for documentation.
    unsigned get_codeblock_length() const override { return 0; }
    // See interface for documentation.
    void write_codeblock(span<uint8_t> data, unsigned offset) const override {}
  };

public:
  /// Creates an LDPC encoder decorator from a base LDPC encoder instance and a metric notifier.
  phy_metrics_ldpc_encoder_decorator(std::unique_ptr<ldpc_encoder> base_encoder_,
                                     ldpc_encoder_metric_notifier& notifier_) :
    base_encoder(std::move(base_encoder_)), notifier(notifier_)
  {
    srsran_assert(base_encoder, "Invalid encoder.");
  }

  // See interface for documentation.
  const ldpc_encoder_buffer& encode(const bit_buffer& input, const configuration& cfg) override
  {
    static dummy_encoder_buffer                       dummy_buffer;
    std::reference_wrapper<const ldpc_encoder_buffer> ret(dummy_buffer);

    ldpc_encoder_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      ret = base_encoder->encode(input, cfg);
    }
    metrics.cb_sz = units::bits(input.size());

    notifier.on_new_metric(metrics);
    return ret.get();
  }

private:
  std::unique_ptr<ldpc_encoder> base_encoder;
  ldpc_encoder_metric_notifier& notifier;
};

} // namespace srsran
