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

#include "aggregators/ldpc_decoder_metrics_aggregator.h"
#include "aggregators/ldpc_encoder_metrics_aggregator.h"
#include "aggregators/ldpc_rate_dematcher_metrics_aggregator.h"
#include "aggregators/ldpc_rate_matcher_metrics_aggregator.h"
#include "srsran/du/du_low/o_du_low_metrics.h"

namespace srsran {
namespace srs_du {

/// LDPC processing metrics collector.
class du_low_ldpc_metrics_collector
{
public:
  /// Collects the metrics from the LDPC aggregators and writes them into the given metrics argument.
  void collect_metrics(o_du_low_ldpc_metrics& metrics);

  /// Returns LDPC decoding metric aggregator's interface for metrics notification.
  ldpc_decoder_metric_notifier& get_ldpc_decoder_notifier() { return ldpc_decoder_aggregator; }

  /// Returns LDPC encoding metric aggregator's interface for metrics notification.
  ldpc_encoder_metric_notifier& get_ldpc_encoder_notifier() { return ldpc_encoder_aggregator; }

  /// Returns LDPC rate matching metric aggregator's interface for metrics notification.
  ldpc_rate_matcher_metric_notifier& get_ldpc_rate_matcher_notifier() { return ldpc_rate_matcher_aggregator; }

  /// Returns LDPC rate dematching metric aggregator's interface for metrics notification.
  ldpc_rate_dematcher_metric_notifier& get_ldpc_rate_dematcher_notifier() { return ldpc_rate_dematcher_aggregator; }

private:
  /// LDPC decoder metrics aggregator.
  ldpc_decoder_metrics_aggregator ldpc_decoder_aggregator;
  /// LDPC rate matcher metrics aggregator.
  ldpc_rate_matcher_metrics_aggregator ldpc_rate_matcher_aggregator;
  /// LDPC rate dematcher metrics aggregator.
  ldpc_rate_dematcher_metrics_aggregator ldpc_rate_dematcher_aggregator;
  /// LDPC encoder metrics aggregator.
  ldpc_encoder_metrics_aggregator ldpc_encoder_aggregator;
};

} // namespace srs_du
} // namespace srsran
