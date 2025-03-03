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

#include "apps/services/cmdline/cmdline_command.h"
#include "srsran/adt/bounded_bitset.h"

namespace srsran {
namespace app_services {

/// Application command to display/hide the metrics in STDOUT. Available metrics
class toggle_stdout_metrics_app_command : public app_services::cmdline_command
{
  /// [Implementation defined] Maximum number of metrics subcommands.
  static constexpr size_t MAX_NOF_METRICS_SUBCOMMANDS = 32;

  std::vector<std::unique_ptr<cmdline_command>> metric_subcommands;
  bounded_bitset<MAX_NOF_METRICS_SUBCOMMANDS>   subcommand_active_status;
  std::vector<std::string_view>                 metric_subcommand_names;
  const std::string                             description;

public:
  toggle_stdout_metrics_app_command(std::vector<std::unique_ptr<cmdline_command>> available_metric_commands_,
                                    bool                                          is_auto_start_enabled = false);

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
};

} // namespace app_services
} // namespace srsran
