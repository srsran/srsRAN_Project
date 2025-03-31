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

#include "ofh_downlink_handler_metrics_collector.h"
#include "ofh_uplink_request_handler_metrics_collector.h"
#include "srsran/ofh/ethernet/ethernet_transmitter_metrics_collector.h"
#include "srsran/ofh/transmitter/ofh_transmitter_metrics_collector.h"

namespace srsran {
namespace ofh {

class transmitter_metrics_collector_impl : public transmitter_metrics_collector
{
public:
  transmitter_metrics_collector_impl(bool                                      metrics_enabled,
                                     downlink_handler_metrics_collector&       dl_metrics_collector_,
                                     uplink_request_handler_metrics_collector& ul_metrics_collector_,
                                     ether::transmitter_metrics_collector*     eth_transmitter_collector_) :
    is_disabled(!metrics_enabled),
    dl_metrics_collector(dl_metrics_collector_),
    ul_metrics_collector(ul_metrics_collector_),
    eth_transmitter_collector(eth_transmitter_collector_)
  {
  }

  /// Returns true if the metrics collector is disabled.
  bool disabled() const { return is_disabled; }

  // See interface for documentation.
  void collect_metrics(transmitter_metrics& metric) override
  {
    if (is_disabled) {
      return;
    }

    dl_metrics_collector.collect_metrics(metric.dl_metrics);
    ul_metrics_collector.collect_metrics(metric.ul_metrics);
    if (eth_transmitter_collector) {
      eth_transmitter_collector->collect_metrics(metric.eth_transmitter_metrics);
    }
  }

private:
  const bool                                is_disabled;
  downlink_handler_metrics_collector&       dl_metrics_collector;
  uplink_request_handler_metrics_collector& ul_metrics_collector;
  ether::transmitter_metrics_collector*     eth_transmitter_collector;
};

} // namespace ofh
} // namespace srsran
