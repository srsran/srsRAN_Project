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
                                  rx_window_checker&                  window_checker_,
                                  message_receiver_metrics_collector* msg_rcv_metrics_collector_,
                                  ether::receiver_metrics_collector*  eth_rcv_metrics_collector_) :
    is_disabled(!metrics_enabled),
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

    window_checker.collect_metrics(metrics.rx_messages_metrics);
    msg_rcv_metrics_collector->collect_metrics(metrics.rx_decoding_perf_metrics);
    eth_rcv_metrics_collector->collect_metrics(metrics.eth_receiver_metrics);
  }

  /// Returns true if the metrics collector is disabled.
  bool disabled() const { return is_disabled; }

private:
  const bool                          is_disabled;
  rx_window_checker&                  window_checker;
  message_receiver_metrics_collector* msg_rcv_metrics_collector;
  ether::receiver_metrics_collector*  eth_rcv_metrics_collector;
};

} // namespace ofh
} // namespace srsran
