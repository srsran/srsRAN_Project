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

#include "aggregators/ldpc_decoder_metrics_aggregator.h"
#include "aggregators/ldpc_encoder_metrics_aggregator.h"
#include "aggregators/ldpc_rate_dematcher_metrics_aggregator.h"
#include "aggregators/ldpc_rate_matcher_metrics_aggregator.h"
#include "srsran/phy/upper/upper_phy_metrics.h"

namespace srsran {

/// LDPC processing metrics collector.
class upper_phy_ldpc_metrics_collector
{
public:
  /// Collects the metrics from the LDPC aggregators and writes them into the given metrics argument.
  void collect_metrics(upper_phy_ldpc_metrics& metrics);

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

} // namespace srsran
