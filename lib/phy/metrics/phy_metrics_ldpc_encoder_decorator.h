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
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_encoder.h"
#include <memory>

namespace srsran {

/// LDPC encoder metric decorator.
class phy_metrics_ldpc_encoder_decorator : public ldpc_encoder
{
public:
  /// Creates an LDPC encoder decorator from a base LDPC encoder instance and metric notifier.
  phy_metrics_ldpc_encoder_decorator(std::unique_ptr<ldpc_encoder> base_encoder_,
                                     ldpc_encoder_metric_notifier& notifier_) :
    base_encoder(std::move(base_encoder_)), notifier(notifier_)
  {
    srsran_assert(base_encoder, "Invalid encoder.");
  }

  // See interface for documentation.
  const ldpc_encoder_buffer& encode(const bit_buffer& input, const codeblock_metadata::tb_common_metadata& cfg) override
  {
    auto tp_before = std::chrono::high_resolution_clock::now();

    // Call base encoder.
    const ldpc_encoder_buffer& ret = base_encoder->encode(input, cfg);

    auto tp_after = std::chrono::high_resolution_clock::now();

    // Create report metrics.
    notifier.new_metric({.cb_sz = units::bits(input.size()), .elapsed = tp_after - tp_before});

    return ret;
  }

private:
  std::unique_ptr<ldpc_encoder> base_encoder;
  ldpc_encoder_metric_notifier& notifier;
};

} // namespace srsran
