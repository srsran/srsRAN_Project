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

void executor_metrics_backend::start(std::chrono::milliseconds  period_,
                                     unique_timer               timer_,
                                     executor_metrics_notifier& notifier_)
{
  if (notifier) {
    return;
  }

  period   = period_;
  timer    = std::move(timer_);
  notifier = &notifier_;

  stop_control.reset();

  timer.set(period, [this](timer_id_t tid) { fetch_metrics(); });
  timer.run();

  srslog::fetch_basic_logger("METRICS").info("Started the executor metrics backend worker");
}

void executor_metrics_backend::stop()
{
  // Signal stop to asynchronous timer thread.
  stop_control.stop();
  timer.stop();

  notifier = nullptr;

  srslog::fetch_basic_logger("METRICS").info("Stopped the executor metrics backend worker");
}

executor_metrics_backend::~executor_metrics_backend()
{
  stop();
}

executor_metrics_channel& executor_metrics_backend::add_channel(const std::string& exec_name)
{
  srsran_assert(!notifier, "Cannot add new metrics channel when the backend is running");

  auto  unique_channel = std::make_unique<executor_metrics_channel>(exec_name);
  auto& channel        = *unique_channel.get();
  {
    std::unique_lock lock(mutex);
    channels.push_back(std::move(unique_channel));
  }
  return channel;
}

void executor_metrics_backend::fetch_metrics()
{
  auto token = stop_control.get_token();

  // Do not rearm the timer and process metrics if stop was requested.
  if (SRSRAN_UNLIKELY(token.is_stop_requested())) {
    return;
  }

  // Rearm the timer.
  timer.run();

  srsran_assert(notifier, "Invalid notifier");

  // Notify new metrics.
  for (auto& channel : channels) {
    executor_metrics report = channel->read();
    notifier->on_new_metrics(report);
  }
}
