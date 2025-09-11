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

#include "cu_up_pdcp_metrics.h"
#include "srsran/srslog/log_channel.h"

namespace srsran {
class pdcp_metrics_notifier;

/// Consumer for the E2 CU-UP PDCP metrics.
class cu_up_pdcp_metrics_consumer_e2 : public app_services::metrics_consumer
{
public:
  explicit cu_up_pdcp_metrics_consumer_e2(pdcp_metrics_notifier& notifier_) : notifier(notifier_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  pdcp_metrics_notifier& notifier;
};

/// Consumer for the json CU-UP PDCP metrics.
class cu_up_pdcp_metrics_consumer_json : public app_services::metrics_consumer
{
  struct aggregated_metrics {
    void reset()
    {
      tx             = {};
      rx             = {};
      metrics_period = {};
      is_empty       = true;
    }

    bool is_empty = true;

    pdcp_tx_metrics_container tx;
    pdcp_rx_metrics_container rx;
    double                    tx_cpu_usage = 0.0;
    double                    rx_cpu_usage = 0.0;
    timer_duration            metrics_period;
  };

public:
  cu_up_pdcp_metrics_consumer_json(srslog::basic_logger& logger_,
                                   srslog::log_channel&  log_chan_,
                                   task_executor&        executor_,
                                   unique_timer          timer_,
                                   unsigned              report_period_ms_);

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  // Print metrics.
  void print_metrics();

  // Clear metrics.
  void clear_metrics() { aggr_metrics.reset(); }

  // Initialize timer.
  void initialize_timer();

  const unsigned        report_period_ms;
  srslog::basic_logger& logger;
  srslog::log_channel&  log_chan;
  task_executor&        executor;
  unique_timer          timer;
  aggregated_metrics    aggr_metrics;
};

/// Consumer for the log PDCP metrics.
class cu_up_pdcp_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit cu_up_pdcp_metrics_consumer_log(srslog::log_channel& log_chan_) : log_chan(log_chan_)
  {
    srsran_assert(log_chan.enabled(), "Logger log channel is not enabled");
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::log_channel& log_chan;
};

} // namespace srsran
