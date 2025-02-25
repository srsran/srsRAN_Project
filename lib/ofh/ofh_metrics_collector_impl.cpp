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

#include "ofh_metrics_collector_impl.h"
#include "srsran/ofh/ofh_metrics.h"

using namespace srsran;
using namespace ofh;

namespace {

/// Receiver metrics collector dummy.
class receiver_metrics_collector_dummy : public receiver_metrics_collector
{
public:
  // See interface for documentation.
  void collect_metrics(receiver_metrics& metric) override {}
};

} // namespace

/// Dummy receiver metrics collector.
static receiver_metrics_collector_dummy dummy_rx_metrics_collector;

metrics_collector_impl::metrics_collector_impl(receiver_metrics_collector* rx_metrics_collector_,
                                               bool                        is_enabled_,
                                               unsigned                    sector_id_) :
  sector_id(sector_id_),
  is_enabled(is_enabled_),
  rx_metrics_collector(rx_metrics_collector_ ? *rx_metrics_collector_ : dummy_rx_metrics_collector)

{
}

void metrics_collector_impl::collect_metrics(sector_metrics& metric)
{
  metric.sector_id = sector_id;

  // Collect receiver metrics.
  rx_metrics_collector.collect_metrics(metric.rx_metrics);
}
