/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
