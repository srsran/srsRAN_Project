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
#include "ofh_data_flow_uplane_downlink_data.h"
#include "ofh_tx_window_checker.h"
#include "srsran/ofh/transmitter/ofh_transmitter_metrics.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul downlink handler metrics collector.
class downlink_handler_metrics_collector
{
public:
  downlink_handler_metrics_collector(data_flow_cplane_scheduling_commands& df_cplane,
                                     data_flow_uplane_downlink_data&       df_uplane,
                                     tx_window_checker&                    window_checker_) :
    df_cplane_collector(df_cplane.get_metrics_collector()),
    df_uplane_collector(df_uplane.get_metrics_collector()),
    window_checker(window_checker_)
  {
  }

  /// Collects downlink handler metrics.
  void collect_metrics(transmitter_dl_metrics& metrics)
  {
    if (df_cplane_collector) {
      df_cplane_collector->collect_metrics(metrics.dl_cp_metrics);
    }
    if (df_uplane_collector) {
      df_uplane_collector->collect_metrics(metrics.dl_up_metrics);
    }
    metrics.nof_late_dl_grids = window_checker.get_nof_lates_and_reset();
  }

private:
  data_flow_downlink_metrics_collector* df_cplane_collector;
  data_flow_downlink_metrics_collector* df_uplane_collector;
  tx_window_checker&                    window_checker;
};

} // namespace ofh
} // namespace srsran
