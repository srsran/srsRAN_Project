/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/scheduler/scheduler_metrics.h"

namespace srsran {

/// Class used to receive metrics reports from scheduler and pretty-print them to the console.
class metrics_plotter_stdout : public scheduler_ue_metrics_notifier
{
public:
  metrics_plotter_stdout() = default;

  /// Notifier called from the scheduler.
  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override;

  /// This can be called from another execution context to turn on/off the actual plotting.
  void toggle_print();

private:
  unsigned          nof_lines     = 10;
  std::atomic<bool> print_metrics = {false};
};

} // namespace srsran
