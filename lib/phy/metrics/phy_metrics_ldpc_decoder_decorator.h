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

/// LDPC decoder metric decorator.
class phy_metrics_ldpc_decoder_decorator : public ldpc_decoder
{
public:
  phy_metrics_ldpc_decoder_decorator(std::unique_ptr<ldpc_decoder> base_decoder_,
                                     ldpc_decoder_metric_notifier& notifier_) :
    base_decoder(std::move(base_decoder_)), notifier(notifier_)
  {
    srsran_assert(base_decoder, "Invalid decoder.");
  }

  std::optional<unsigned> decode(bit_buffer&                      output,
                                 span<const log_likelihood_ratio> input,
                                 crc_calculator*                  crc,
                                 const configuration&             cfg) override
  {
    auto tp_before = std::chrono::high_resolution_clock::now();

    // Call base decoder.
    std::optional<unsigned> ret = base_decoder->decode(output, input, crc, cfg);

    auto tp_after = std::chrono::high_resolution_clock::now();

    // Create report metrics.
    notifier.new_metric({.cb_sz          = units::bits(output.size()),
                         .nof_iterations = ret.value_or(cfg.algorithm_conf.max_iterations),
                         .crc_ok         = ret.has_value(),
                         .elapsed        = tp_after - tp_before});

    return ret;
  }

private:
  std::unique_ptr<ldpc_decoder> base_decoder;
  ldpc_decoder_metric_notifier& notifier;
};

} // namespace srsran
