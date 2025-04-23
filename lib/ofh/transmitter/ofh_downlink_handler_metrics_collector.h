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
  data_flow_message_encoding_metrics_collector* df_cplane_collector;
  data_flow_message_encoding_metrics_collector* df_uplane_collector;
  tx_window_checker&                            window_checker;
};

} // namespace ofh
} // namespace srsran
