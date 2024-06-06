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

#include "apps/services/application_command.h"
#include "apps/services/metrics_plotter_stdout.h"

namespace srsran {

/// Application command to display/hide the DU high metrics in STDOUT.
class toggle_stdout_metrics_app_command : public app_services::application_command
{
  metrics_plotter_stdout& printer;

public:
  explicit toggle_stdout_metrics_app_command(metrics_plotter_stdout& printer_) : printer(printer_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "t"; }

  // See interface for documentation.
  std::string_view get_description() const override
  {
    return ":                                    start/stop console trace";
  }

  // See interface for documentation.
  void execute(span<const std::string> args) override { printer.toggle_print(); }
};

} // namespace srsran
