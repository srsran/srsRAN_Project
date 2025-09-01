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

#include "include/srsran/support/srsran_assert.h"
#include "include/srsran/support/timers.h"
#include "metrics_producer.h"
#include <future>

namespace srsran {
namespace app_services {

/// This controller class uses unique timer and triggers new metrics generation by the registered producers based on a
/// configured period.
class periodic_metrics_report_controller
{
public:
  /// Constructor receives timer object, report period and application metric configs.
  periodic_metrics_report_controller(std::vector<metrics_producer*> producers_,
                                     unique_timer                   timer_,
                                     std::chrono::milliseconds      report_period_) :
    timer(std::move(timer_)), report_period(report_period_), producers(std::move(producers_))
  {
    srsran_assert(timer.is_valid(), "Invalid timer passed to metrics controller");
    timer.set(report_period, [this](timer_id_t tid) { report_metrics(); });
  }

  /// Starts the metrics report timer.
  void start()
  {
    if (!report_period.count()) {
      return;
    }
    stopped.store(false, std::memory_order_relaxed);
    timer.run();
  }

  /// Stops the metrics report timer.
  void stop()
  {
    if (!report_period.count()) {
      return;
    }
    if (not stopped.exchange(true, std::memory_order_relaxed)) {
      auto fut = exit_signal.get_future();
      if (fut.wait_for(std::chrono::seconds(1)) == std::future_status::timeout) {
        srslog::fetch_basic_logger("ALL").warning("Timeout waiting for metrics report controller to stop");
      }
      timer.stop();
    }
  }

private:
  /// Trigger metrics report in all registered producers.
  void report_metrics()
  {
    if (stopped.load(std::memory_order_relaxed)) {
      exit_signal.set_value();
      return;
    }
    // Rearm the timer.
    timer.run();

    // Command the producers to report their accumulated metrics.
    for (auto* producer : producers) {
      producer->on_new_report_period();
    }
  }

  /// Timer object armed for configured report period.
  unique_timer timer;
  /// Metrics report period.
  std::chrono::milliseconds report_period{0};
  std::atomic<bool>         stopped{true};
  /// List of metrics producers managed by this controller.
  std::vector<metrics_producer*> producers;

  std::promise<void> exit_signal;
};

} // namespace app_services
} // namespace srsran
