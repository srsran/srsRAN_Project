/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/executors/metrics/executor_metrics_backend.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/metrics/executor_metrics_notifier.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

executor_metrics_backend::executor_metrics_backend(std::chrono::milliseconds period_, unique_timer timer_) :
  period(period_), timer(std::move(timer_))
{
  report_error_if_not(timer.is_valid(), "Invalid timer passed to executor metrics backend");
}

void executor_metrics_backend::start(executor_metrics_notifier& notifier_)
{
  if (status == worker_status::running) {
    return;
  }

  notifier = &notifier_;
  report_error_if_not(notifier, "Invalid notifier passed to executor metrics backend");

  status = worker_status::running;

  timer.set(period, [this](timer_id_t tid) { fetch_metrics(); });
  timer.run();

  srslog::fetch_basic_logger("METRICS").info("Started the executor metrics backend worker");
}

void executor_metrics_backend::stop()
{
  if (status != worker_status::running) {
    return;
  }

  status.store(worker_status::stopped, std::memory_order_relaxed);
  timer.stop();

  srslog::fetch_basic_logger("METRICS").info("Stopped the metrics backend worker");
}

executor_metrics_backend::~executor_metrics_backend()
{
  stop();
}

executor_metrics_channel& executor_metrics_backend::create_channel(const std::string& exec_name)
{
  srsran_assert(status != worker_status::running, "Cannot add new metrics channel when the backend is running");

  auto unique_channel = std::make_unique<executor_metrics_channel>(exec_name);
  srsran_assert(unique_channel, "Failed to create metrics channel");

  auto& channel = *unique_channel.get();
  {
    std::unique_lock lock(mutex);
    channels.push_back(std::move(unique_channel));
  }
  return channel;
}

void executor_metrics_backend::fetch_metrics()
{
  if (SRSRAN_UNLIKELY(status.load(std::memory_order_relaxed) != worker_status::running)) {
    return;
  }

  // Rearm the timer.
  timer.run();

  // Notify new metrics.
  for (auto& channel : channels) {
    executor_metrics report = channel->read();
    notifier->on_new_metrics(report);
  }
}
