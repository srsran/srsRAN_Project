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

#include "executor_metrics_channel.h"
#include "executor_metrics_channel_registry.h"
#include "srsran/support/synchronization/stop_event.h"
#include "srsran/support/timers.h"
#include <chrono>
#include <memory>
#include <mutex>

namespace srsran {

class executor_metrics_notifier;

/// \brief Executor metrics backend.
///
/// This class is responsible for handling metrics produced by different executors.
/// An executor registers itself in the backend by getting a unique metrics channel.
class executor_metrics_backend : public executor_metrics_channel_registry
{
  enum class worker_status { running, stopped };

public:
  executor_metrics_backend() = default;
  ~executor_metrics_backend() override;

  /// Delete copy and move constructors, a single object is used application-wise.
  executor_metrics_backend(const executor_metrics_backend& other) = delete;
  executor_metrics_backend(executor_metrics_backend&& other)      = delete;
  void operator=(const executor_metrics_backend& other)           = delete;
  void operator=(executor_metrics_backend&& other)                = delete;

  // See interface for documentation.
  executor_metrics_channel& add_channel(const std::string& exec_name) override;

  /// Stops periodic metrics reporting.
  void stop();

  /// Start the backend worker using the passed timer and periodically report metrics to the given notifier object.
  void start(std::chrono::milliseconds period_, unique_timer timer_, executor_metrics_notifier& notifier_);

private:
  /// Periodic work done by this backend.
  /// Every report period it notifies the new metrics obtained from the channels.
  void fetch_metrics();

  /// Metrics report period.
  std::chrono::milliseconds period;
  /// Periodic timer.
  unique_timer timer;
  /// An interface to a metrics handler.
  executor_metrics_notifier* notifier;
  /// Backend worker status.
  std::atomic<worker_status> status{worker_status::stopped};
  /// Stop control.
  stop_event_source stop_control;

  /// List of metrics channels.
  std::mutex                                             mutex;
  std::vector<std::unique_ptr<executor_metrics_channel>> channels;
};

} // namespace srsran
