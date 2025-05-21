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

#include "apps/services/cmdline/stdout_metrics_command.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/metrics/du_metrics_consumers.h"

namespace srsran {

/// Application subcommand to display/hide the DU high metrics in STDOUT.
class du_high_metrics_subcommand_stdout : public app_services::toggle_stdout_metrics_app_command::metrics_subcommand
{
  du_metrics_consumer_stdout& printer;

public:
  explicit du_high_metrics_subcommand_stdout(du_metrics_consumer_stdout& printer_) : printer(printer_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "ue"; }

  // See interface for documentation.
  void print_header() override { printer.print_header(); }

  // See interface for documentation.
  void enable() override { printer.enable(); }

  // See interface for documentation.
  void disable() override { printer.disable(); }
};

} // namespace srsran
