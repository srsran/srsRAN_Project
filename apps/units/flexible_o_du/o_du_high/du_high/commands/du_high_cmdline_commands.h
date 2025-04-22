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
