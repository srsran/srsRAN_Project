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

#include "du_high_mac_metrics.h"
#include "srsran/srslog/log_channel.h"

namespace srsran {

/// Consumer for the json MAC metrics.
class mac_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit mac_metrics_consumer_json(srslog::log_channel& log_chan_) : log_chan(log_chan_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::log_channel& log_chan;
};

/// Consumer for the logger MAC metrics.
class mac_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit mac_metrics_consumer_log(srslog::log_channel& log_chan_) : log_chan(log_chan_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::log_channel& log_chan;
};

} // namespace srsran
