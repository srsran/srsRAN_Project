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
    bool empty() { return is_empty; }
    void reset() { is_empty = true; }

    bool is_empty = true;

    std::chrono::time_point<std::chrono::system_clock> start_time;
    pdcp_metrics_container                             metric;
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

  // Aggregate metrics.
  void aggregate_metrics(const pdcp_metrics_container& metrics);

  // Initialize timer.
  void initialize_timer();

private:
  const unsigned        report_period_ms;
  srslog::basic_logger& logger;
  srslog::log_channel&  log_chan;
  task_executor&        executor;
  unique_timer          timer;
  aggregated_metrics    aggr_metrics;
};

} // namespace srsran
