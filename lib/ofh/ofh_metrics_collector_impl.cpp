/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
