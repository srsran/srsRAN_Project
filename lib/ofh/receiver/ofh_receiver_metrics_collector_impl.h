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

#include "ofh_message_receiver.h"
#include "ofh_rx_window_checker.h"
#include "srsran/ofh/ethernet/ethernet_receiver_metrics_collector.h"
#include "srsran/ofh/receiver/ofh_receiver_metrics_collector.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul receiver metrics collector implementation.
class receiver_metrics_collector_impl : public receiver_metrics_collector
{
public:
  receiver_metrics_collector_impl(bool               metrics_enabled,
                                  rx_window_checker& window_checker_,
                                  message_receiver&  msg_receiver_) :
    is_disabled(!metrics_enabled), window_checker(window_checker_), msg_receiver(msg_receiver_)
  {
  }

  // See interface for documentation.
  void collect_metrics(receiver_metrics& metrics) override
  {
    window_checker.collect_metrics(metrics.rx_messages_metrics);
    msg_receiver.get_metrics_collector().collect_metrics(metrics.rx_perf_metrics);
  }

  /// Returns true if the metrics collector is disabled.
  bool disabled() const { return is_disabled; }

private:
  const bool         is_disabled;
  rx_window_checker& window_checker;
  message_receiver&  msg_receiver;
};

} // namespace ofh
} // namespace srsran
