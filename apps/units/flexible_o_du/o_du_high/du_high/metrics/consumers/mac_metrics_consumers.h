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

#include "srsran/mac/mac_metrics.h"
#include "srsran/srslog/log_channel.h"

namespace srsran {

/// JSON consumer for the MAC metrics.
class mac_metrics_consumer_json
{
public:
  explicit mac_metrics_consumer_json(srslog::log_channel& log_chan_) : log_chan(log_chan_) {}

  /// Handle the given DU high MAC metrics.
  void handle_metric(const mac_dl_metric_report& report);

private:
  srslog::log_channel& log_chan;
};

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
