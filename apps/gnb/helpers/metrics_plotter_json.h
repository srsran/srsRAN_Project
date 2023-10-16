/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/scheduler/scheduler_metrics.h"

namespace srsran {

/// Class used to receive metrics reports from scheduler and format them into a JSON file.
class metrics_plotter_json : public scheduler_ue_metrics_notifier
{
public:
  explicit metrics_plotter_json(srslog::log_channel& log_chan_) : log_chan(log_chan_) {}

  /// Notifier called from the scheduler.
  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override;

private:
  srslog::log_channel& log_chan;
};

} // namespace srsran
