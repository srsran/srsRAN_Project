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

#include "apps/services/metrics/metrics_consumer.h"
#include "consumers/mac_metrics_consumers.h"
#include "consumers/scheduler_metrics_consumers.h"
#include "srsran/srslog/log_channel.h"

namespace srsran {

/// Consumer for the STDOUT DU metrics.
class du_metrics_consumer_stdout : public app_services::metrics_consumer
{
public:
  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

  /// This can be called from another execution context to turn on the actual plotting.
  void enable() { print_metrics.store(true, std::memory_order_relaxed); }

  /// This can be called from another execution context to turn off the actual plotting.
  void disable() { print_metrics.store(false, std::memory_order_relaxed); }
  /// Prints the header in the next metric handle.
  void print_header() { sched_consumer.print_header(); }

private:
  scheduler_cell_metrics_consumer_stdout sched_consumer;
  std::atomic<bool>                      print_metrics = {false};
};

/// Consumer for the json DU metrics.
class du_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit du_metrics_consumer_json(srslog::log_channel& log_chan_) : log_chan(log_chan_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::log_channel& log_chan;
};

/// Consumer for the logger DU metrics.
class du_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit du_metrics_consumer_log(srslog::log_channel& log_chan) : mac_consumer(log_chan), sched_consumer(log_chan) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  mac_metrics_consumer_log            mac_consumer;
  scheduler_cell_metrics_consumer_log sched_consumer;
};

/// Consumer for the E2 DU metrics.
class du_metrics_consumer_e2 : public app_services::metrics_consumer
{
public:
  explicit du_metrics_consumer_e2(scheduler_metrics_notifier& notifier) : sched_consumer(notifier) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  scheduler_cell_metrics_consumer_e2 sched_consumer;
};

} // namespace srsran
