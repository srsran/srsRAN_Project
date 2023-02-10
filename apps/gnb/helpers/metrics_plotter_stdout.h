/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/scheduler/scheduler_metrics.h"

namespace srsgnb {

/// \brief Class used to receive metrics reports from scheduler and pretty-print them to the console.
class metrics_plotter_stdout : public scheduler_ue_metrics_notifier
{
public:
  metrics_plotter_stdout() = default;

  /// \brief Notifier called from the scheduler.
  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override;

  /// \brief This can be called from another execution context to turn on/off the actual plotting.
  void toggle_print();

private:
  std::string       float_to_string(float f, int digits, int field_width);
  std::string       float_to_eng_string(float f, int digits);
  void              print_header();
  int               nof_lines     = 10;
  std::atomic<bool> print_metrics = {false};
};

} // namespace srsgnb
