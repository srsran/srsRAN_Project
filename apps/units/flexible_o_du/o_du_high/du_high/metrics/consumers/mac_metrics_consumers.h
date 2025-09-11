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

#include "srsran/mac/mac_metrics.h"
#include "srsran/srslog/log_channel.h"

namespace srsran {

/// Logger consumer for the MAC metrics.
class mac_metrics_consumer_log
{
public:
  explicit mac_metrics_consumer_log(srslog::log_channel& log_chan_) : log_chan(log_chan_) {}

  /// Handle the given DU high MAC metrics.
  void handle_metric(const mac_dl_metric_report& report);

private:
  srslog::log_channel& log_chan;
};

} // namespace srsran
