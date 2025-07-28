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

#include "metrics/split6_flexible_o_du_low_metrics_notifier.h"
#include "srsran/ru/ru.h"
#include "srsran/support/timers.h"

namespace srsran {
class task_executor;

namespace srs_du {
class o_du_low_metrics_collector;
}

/// Split 6 O-DU low metrics collector implementation.
class split6_o_du_low_metrics_collector_impl
{
public:
  split6_o_du_low_metrics_collector_impl() = default;
  split6_o_du_low_metrics_collector_impl(srs_du::o_du_low_metrics_collector*        odu_low_,
                                         ru_metrics_collector*                      ru_,
                                         split6_flexible_o_du_low_metrics_notifier* notifier_,
                                         unique_timer                               timer_,
                                         std::chrono::milliseconds                  report_period_);

  split6_o_du_low_metrics_collector_impl& operator=(split6_o_du_low_metrics_collector_impl&& other) noexcept;

  /// Delete unused constructors.
  split6_o_du_low_metrics_collector_impl(const split6_o_du_low_metrics_collector_impl&)            = delete;
  split6_o_du_low_metrics_collector_impl(split6_o_du_low_metrics_collector_impl&& other)           = delete;
  split6_o_du_low_metrics_collector_impl& operator=(const split6_o_du_low_metrics_collector_impl&) = delete;

  ~split6_o_du_low_metrics_collector_impl();

private:
  void collect_metrics();

private:
  unique_timer                               timer;
  std::chrono::milliseconds                  report_period{0};
  std::atomic<bool>                          stopped{true};
  split6_flexible_o_du_low_metrics_notifier* notifier = nullptr;
  srs_du::o_du_low_metrics_collector*        odu_low  = nullptr;
  ru_metrics_collector*                      ru       = nullptr;
};

} // namespace srsran
