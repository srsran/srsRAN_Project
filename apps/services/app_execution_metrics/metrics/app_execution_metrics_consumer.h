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

#include "app_execution_metrics.h"
#include "apps/helpers/metrics/json_generators/executors.h"
#include "apps/services/metrics/metrics_consumer.h"
#include "srsran/srslog/log_channel.h"
#include "srsran/support/format/fmt_to_c_str.h"

namespace srsran {

/// Log consumer for the application executors metrics.
class app_execution_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit app_execution_metrics_consumer_log(srslog::log_channel& log_chan_) : log_chan(log_chan_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override
  {
    static constexpr unsigned str_buffer_size = 256;

    const auto& metrics = static_cast<const executor_metrics_impl&>(metric).get_metrics();
    fmt::basic_memory_buffer<char, str_buffer_size> buffer;

    fmt::format_to(std::back_inserter(buffer),
                   "Executor metrics \"{}\": nof_executes={} nof_defers={} enqueue_avg={}usec enqueue_max={}usec "
                   "task_avg={}usec task_max={}usec cpu_load={:.1f}% {}",
                   metrics.name,
                   metrics.nof_executes,
                   metrics.nof_defers,
                   metrics.avg_enqueue_latency_us.count(),
                   metrics.max_enqueue_latency_us.count(),
                   metrics.avg_task_us.count(),
                   metrics.max_task_us.count(),
                   metrics.cpu_load,
                   metrics.total_rusg);

    // Flush buffer to the logger.
    log_chan("{}", to_c_str(buffer));
  }

private:
  srslog::log_channel& log_chan;
};

/// JSON consumer for the application executors metrics.
class app_execution_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit app_execution_metrics_consumer_json(srslog::log_channel& log_chan_) : log_chan(log_chan_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override
  {
    const auto& metrics = static_cast<const executor_metrics_impl&>(metric).get_metrics();
    log_chan("{}", app_helpers::json_generators::generate_string(metrics, 2));
  }

private:
  srslog::log_channel& log_chan;
};

} // namespace srsran
