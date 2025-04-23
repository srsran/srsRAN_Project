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

#include "include/srsran/support/srsran_assert.h"
#include "include/srsran/support/timers.h"
#include "metrics_producer.h"

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
    stopped.store(true, std::memory_order_relaxed);
    timer.stop();
  }

private:
  /// Trigger metrics report in all registered producers.
  void report_metrics()
  {
    if (stopped.load(std::memory_order_relaxed)) {
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
};

} // namespace app_services
} // namespace srsran
