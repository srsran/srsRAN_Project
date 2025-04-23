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

#include "stdout_metrics_command.h"

using namespace srsran;
using namespace app_services;

static std::string generate_command_description(span<const std::string_view> subcommands)
{
  return fmt::format(
      " <metric>:                           start/stop console trace. List of available metric [{:,}]. Default={}",
      span<const std::string_view>(subcommands),
      subcommands.front());
}

static std::vector<std::string_view>
get_subcommand_names(span<const std::unique_ptr<toggle_stdout_metrics_app_command::metrics_subcommand>> subcommands)
{
  std::vector<std::string_view> metrics_list;
  for (const auto& subcommand : subcommands) {
    metrics_list.push_back(subcommand->get_name());
  }

  return metrics_list;
}

toggle_stdout_metrics_app_command::toggle_stdout_metrics_app_command(
    std::vector<std::unique_ptr<metrics_subcommand>> available_metric_commands_,
    bool                                             is_auto_start_enabled) :
  metric_subcommands(std::move(available_metric_commands_)),
  subcommand_active_status(metric_subcommands.size()),
  metric_subcommand_names(get_subcommand_names(metric_subcommands)),
  description(generate_command_description(metric_subcommand_names))
{
  srsran_assert(!metric_subcommands.empty(), "Metrics subcommand list empty");

  if (is_auto_start_enabled) {
    execute_subcommand(0);
  }
}

void toggle_stdout_metrics_app_command::execute(span<const std::string> args)
{
  if (args.size() > 1) {
    fmt::println("Invalid metrics command. Usage: t <metric>. Supported metrics [{}]",
                 span<const std::string_view>(metric_subcommand_names));
    return;
  }

  // Use default index.
  int dst_idx = 0;

  // Parse subcommand type when passed.
  if (args.size() == 1) {
    dst_idx = get_subcommand_index(args[0]);

    // Subcommand argument provided but not found in the list. Report available subcommands and return.
    if (dst_idx == -1) {
      fmt::println("Invalid metric type. Supported metrics [{}]",
                   span<const std::string_view>(metric_subcommand_names));
      return;
    }
  }

  execute_subcommand(dst_idx);
}

void toggle_stdout_metrics_app_command::execute_subcommand(unsigned index)
{
  // Stop previously metric printing if needed.
  bool print_header = stop_current_active_subcommand(index);

  subcommand_active_status.flip(index);

  // Print header when a change of subcommand is detected.
  if (print_header) {
    metric_subcommands[index]->print_header();
  }

  if (subcommand_active_status.test(index)) {
    metric_subcommands[index]->enable();
  } else {
    metric_subcommands[index]->disable();
  }
}

bool toggle_stdout_metrics_app_command::stop_current_active_subcommand(unsigned index)
{
  // Sanity check.
  srsran_assert(subcommand_active_status.count() < 2, "Only one metrics must be active at a given time");

  int current_active = subcommand_active_status.find_highest();

  // No metric active. Nothing to do.
  if (current_active == -1) {
    return false;
  }

  // Requested subcommand is the active one. Do nothing.
  if (static_cast<int>(index) == current_active) {
    return false;
  }

  // In this case the active subcommand changes. Disable current active subcommand.
  subcommand_active_status.flip(current_active);
  metric_subcommands[current_active]->disable();

  srsran_assert(subcommand_active_status.none(), "No subcommand must be active at this point");
  return true;
}

int toggle_stdout_metrics_app_command::get_subcommand_index(const std::string& args) const
{
  auto I = std::find_if(metric_subcommands.begin(),
                        metric_subcommands.end(),
                        [&args](const std::unique_ptr<toggle_stdout_metrics_app_command::metrics_subcommand>& cmd) {
                          return args == cmd->get_name();
                        });

  return I == metric_subcommands.end() ? -1 : std::distance(metric_subcommands.begin(), I);
}

std::unique_ptr<cmdline_command> srsran::app_services::create_stdout_metrics_app_command(
    std::vector<span<std::unique_ptr<toggle_stdout_metrics_app_command::metrics_subcommand>>> metrics_subcommand,
    bool                                                                                      is_auto_start_enabled)
{
  std::vector<std::unique_ptr<toggle_stdout_metrics_app_command::metrics_subcommand>> subcommands;
  for (auto& unit : metrics_subcommand) {
    for (auto& subcmd : unit) {
      subcommands.push_back(std::move(subcmd));
    }
  }

  // Return nulls if no subcommand is passed.
  return subcommands.empty()
             ? nullptr
             : std::make_unique<toggle_stdout_metrics_app_command>(std::move(subcommands), is_auto_start_enabled);
}
