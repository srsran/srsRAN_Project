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

#include "apps/services/metrics/metrics_consumer.h"
#include "du_high_scheduler_cell_metrics.h"
#include "srsran/srslog/log_channel.h"
#include "srsran/srslog/logger.h"

namespace srsran {

/// Consumer for the STDOUT scheduler cell metrics.
class scheduler_cell_metrics_consumer_stdout : public app_services::metrics_consumer
{
public:
  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

  /// This can be called from another execution context to turn on the actual plotting.
  void enable() { print_metrics = true; }

  /// This can be called from another execution context to turn off the actual plotting.
  void disable() { print_metrics = false; }

  /// Prints the header in the next metric handle.
  void print_header() { nof_lines = 10; }

private:
  unsigned          nof_lines     = 10;
  std::atomic<bool> print_metrics = {false};
};

/// Consumer for the json scheduler cell metrics.
class scheduler_cell_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit scheduler_cell_metrics_consumer_json(srslog::log_channel& log_chan_) : log_chan(log_chan_)
  {
    srsran_assert(log_chan.enabled(), "JSON log channel is not enabled");
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::log_channel& log_chan;
};

/// Consumer for the log file scheduler cell metrics.
class scheduler_cell_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit scheduler_cell_metrics_consumer_log(srslog::log_channel& log_chan_) : log_chan(log_chan_)
  {
    srsran_assert(log_chan.enabled(), "Logger log channel is not enabled");
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::log_channel& log_chan;
};

/// Consumer for the E2 file scheduler cell metrics.
class scheduler_cell_metrics_consumer_e2 : public app_services::metrics_consumer
{
public:
  explicit scheduler_cell_metrics_consumer_e2(scheduler_metrics_notifier& notifier_) : notifier(notifier_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  scheduler_metrics_notifier& notifier;
};

} // namespace srsran
