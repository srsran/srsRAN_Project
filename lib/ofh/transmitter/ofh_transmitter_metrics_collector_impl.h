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

#include "ofh_downlink_handler_metrics_collector.h"
#include "ofh_message_transmitter_metrics_collector.h"
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
                                     message_transmitter_metrics_collector&    message_tx_collector_,
                                     ether::transmitter_metrics_collector*     eth_transmitter_collector_) :
    is_disabled(!metrics_enabled),
    dl_metrics_collector(dl_metrics_collector_),
    ul_metrics_collector(ul_metrics_collector_),
    message_tx_collector(message_tx_collector_),
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
    message_tx_collector.collect_metrics(metric.message_tx_metrics);
    if (eth_transmitter_collector) {
      eth_transmitter_collector->collect_metrics(metric.eth_transmitter_metrics);
    }
  }

private:
  const bool                                is_disabled;
  downlink_handler_metrics_collector&       dl_metrics_collector;
  uplink_request_handler_metrics_collector& ul_metrics_collector;
  message_transmitter_metrics_collector&    message_tx_collector;
  ether::transmitter_metrics_collector*     eth_transmitter_collector;
};

} // namespace ofh
} // namespace srsran
