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

#include "apps/services/cmdline/cmdline_command.h"
#include "srsran/adt/bounded_bitset.h"

namespace srsran {
namespace app_services {

/// Application command to display/hide the metrics in STDOUT. Available metrics
class toggle_stdout_metrics_app_command : public app_services::cmdline_command
{
  /// [Implementation defined] Maximum number of metrics subcommands.
  static constexpr size_t MAX_NOF_METRICS_SUBCOMMANDS = 32;

public:
  /// Metrics subcommand interface for STDOUT.
  class metrics_subcommand
  {
  public:
    virtual ~metrics_subcommand() = default;

    /// Get the metrics name.
    virtual std::string_view get_name() const = 0;

    /// Prints this metrics header.
    virtual void print_header() = 0;

    /// Enables this subcommand.
    virtual void enable() = 0;

    /// Disables this subcommand.
    virtual void disable() = 0;
  };

  toggle_stdout_metrics_app_command(std::vector<std::unique_ptr<metrics_subcommand>> available_metric_commands_,
                                    bool                                             is_auto_start_enabled = false);

  // See interface for documentation.
  std::string_view get_name() const override { return "t"; }

  // See interface for documentation.
  std::string_view get_description() const override { return description; }

  // See interface for documentation.
  void execute(span<const std::string> args) override;

private:
  /// Executes subcommand for the given index.
  void execute_subcommand(unsigned index);

  /// Returns the subcommand index from the available subcommands that matches the given args.
  int get_subcommand_index(const std::string& args) const;

  /// \brief Stops the current active subcommand.
  ///
  /// Possible cases:
  ///   There is no active command. In this case do nothing.
  ///   Current active index matches the given index. In this case do nothing.
  ///   Current active index does not match given index, so current active subcommand will change. Stop current
  ///   subcommand before swapping active subcommand.
  ///
  ///   \return True if a subcommand was stopped, false otherwise.
  bool stop_current_active_subcommand(unsigned index);

private:
  std::vector<std::unique_ptr<metrics_subcommand>> metric_subcommands;
  bounded_bitset<MAX_NOF_METRICS_SUBCOMMANDS>      subcommand_active_status;
  std::vector<std::string_view>                    metric_subcommand_names;
  const std::string                                description;
};

/// Creates a STDOUT metrics application command with the given parameters.
std::unique_ptr<cmdline_command> create_stdout_metrics_app_command(
    std::vector<span<std::unique_ptr<toggle_stdout_metrics_app_command::metrics_subcommand>>> metrics_subcommand,
    bool                                                                                      is_auto_start_enabled);

} // namespace app_services
} // namespace srsran
