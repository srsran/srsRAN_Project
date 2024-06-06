/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/scheduler/scheduler_metrics.h"

namespace srsran {

/// Class used to receive metrics reports from scheduler and pretty-print them to the console.
class metrics_plotter_stdout : public scheduler_ue_metrics_notifier
{
public:
  explicit metrics_plotter_stdout(bool print_metrics_) : print_metrics(print_metrics_) {}

  /// Notifier called from the scheduler.
  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override;

  /// This can be called from another execution context to turn on/off the actual plotting.
  void toggle_print();

private:
  unsigned          nof_lines     = 10;
  std::atomic<bool> print_metrics = {false};
};

} // namespace srsran
