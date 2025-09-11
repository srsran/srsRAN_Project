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

#include "ofh_closed_rx_window_handler.h"
#include "ofh_message_receiver_metrics_collector.h"
#include "ofh_rx_window_checker.h"
#include "srsran/ofh/ethernet/ethernet_receiver_metrics_collector.h"
#include "srsran/ofh/receiver/ofh_receiver_metrics_collector.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul receiver metrics collector implementation.
class receiver_metrics_collector_impl : public receiver_metrics_collector
{
public:
  receiver_metrics_collector_impl(bool                                metrics_enabled,
                                  closed_rx_window_handler&           closed_window_handler_,
                                  rx_window_checker&                  window_checker_,
                                  message_receiver_metrics_collector* msg_rcv_metrics_collector_,
                                  ether::receiver_metrics_collector*  eth_rcv_metrics_collector_) :
    is_disabled(!metrics_enabled),
    closed_window_handler(closed_window_handler_),
    window_checker(window_checker_),
    msg_rcv_metrics_collector(msg_rcv_metrics_collector_),
    eth_rcv_metrics_collector(eth_rcv_metrics_collector_)
  {
    if (!is_disabled) {
      srsran_assert(msg_rcv_metrics_collector && eth_rcv_metrics_collector,
                    "Open fronthaul receiver metrics collectors must be initialized when the metrics are enabled");
    }
  }

  // See interface for documentation.
  void collect_metrics(receiver_metrics& metrics) override
  {
    if (disabled()) {
      return;
    }

    closed_window_handler.collect_metrics(metrics.closed_window_metrics);
    window_checker.collect_metrics(metrics.rx_messages_metrics);
    msg_rcv_metrics_collector->collect_metrics(metrics.rx_decoding_perf_metrics);
    eth_rcv_metrics_collector->collect_metrics(metrics.eth_receiver_metrics);
  }

  /// Returns true if the metrics collector is disabled.
  bool disabled() const { return is_disabled; }

private:
  const bool                          is_disabled;
  closed_rx_window_handler&           closed_window_handler;
  rx_window_checker&                  window_checker;
  message_receiver_metrics_collector* msg_rcv_metrics_collector;
  ether::receiver_metrics_collector*  eth_rcv_metrics_collector;
};

} // namespace ofh
} // namespace srsran
