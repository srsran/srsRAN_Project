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

#include "ofh_data_flow_cplane_scheduling_commands.h"
#include "ofh_tx_window_checker.h"
#include "srsran/ofh/transmitter/ofh_transmitter_metrics.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul uplink request handler metrics collector.
class uplink_request_handler_metrics_collector
{
public:
  uplink_request_handler_metrics_collector(data_flow_message_encoding_metrics_collector* df_metrics_collector_,
                                           tx_window_checker&                            window_checker_) :
    df_metrics_collector(df_metrics_collector_), window_checker(window_checker_)
  {
  }

  /// Updates the number of late messages (messages that were not transmitted) of the Control-Plane uplink by the
  /// given value.
  void update_cp_ul_lates(unsigned nof_lates) { nof_late_cp_ul.fetch_add(nof_lates, std::memory_order_relaxed); }

  /// Collects uplink request handler metrics.
  void collect_metrics(transmitter_ul_metrics& metrics)
  {
    if (df_metrics_collector) {
      df_metrics_collector->collect_metrics(metrics.ul_cp_metrics);
    }
    metrics.nof_late_ul_requests = window_checker.get_nof_lates_and_reset();
    metrics.nof_late_cp_ul       = nof_late_cp_ul.exchange(0, std::memory_order_relaxed);
  }

private:
  data_flow_message_encoding_metrics_collector* df_metrics_collector;
  tx_window_checker&                            window_checker;
  std::atomic<uint32_t>                         nof_late_cp_ul = {0};
};

} // namespace ofh
} // namespace srsran
