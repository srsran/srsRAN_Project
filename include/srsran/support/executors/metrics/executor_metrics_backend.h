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

#include "executor_metrics_channel.h"
#include "srsran/support/timers.h"
#include <chrono>
#include <memory>
#include <mutex>

namespace srsran {

class executor_metrics_notifier;

/// \brief Executor metrics backend
///
/// This class is responsible for handling metrics produced by different executors.
/// An executor registers itself in the backend by getting a unique metrics channel.
class executor_metrics_backend
{
  enum class worker_status { idle, running, stopped };

public:
  /// Constructor receives metrics period.
  executor_metrics_backend(std::chrono::milliseconds period_, unique_timer timer_);

  ~executor_metrics_backend();

  /// Delete copy and move constructors, a single object is used application-wise.
  executor_metrics_backend(const executor_metrics_backend& other) = delete;
  executor_metrics_backend(executor_metrics_backend&& other)      = delete;
  void operator=(const executor_metrics_backend& other)           = delete;
  void operator=(executor_metrics_backend&& other)                = delete;

  /// Creates metrics channel, that a calling executor can use to send its metrics.
  executor_metrics_channel& create_channel(const std::string& exec_name);

  /// Stops periodic metrics reporting.
  void stop();

  /// Start the backend worker using the stored timer and periodically report metrics to the given notifier object.
  void start(executor_metrics_notifier& notifier_);

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
  std::atomic<worker_status> status{worker_status::idle};

  /// List of metrics channels.
  std::mutex                                             mutex;
  std::vector<std::unique_ptr<executor_metrics_channel>> channels;
};

} // namespace srsran
