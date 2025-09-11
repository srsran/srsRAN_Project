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

#include "worker_manager_cli11_schema.h"
#include "cli11_cpu_affinities_parser_helper.h"
#include "worker_manager_appconfig.h"
#include "srsran/adt/expected.h"
#include "srsran/support/cli11_utils.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

static void configure_cli11_main_pool_threads_args(CLI::App& app, main_thread_pool_appconfig& config)
{
  add_option(app, "--nof_threads", config.nof_threads, "Number of threads for processing upper PHY and upper layers.")
      ->capture_default_str()
      ->check(CLI::Number);
  add_option(app, "--task_queue_size", config.task_queue_size, "Main thread pool task queue size.")
      ->capture_default_str()
      ->check(CLI::Number);
  add_option(app, "--backoff_period", config.backoff_period, "Main thread pool back-off period, in microseconds.")
      ->capture_default_str()
      ->check(CLI::Number);
}

static void configure_cli11_cpu_affinities_args(CLI::App& app, cpu_affinities_appconfig& config)
{
  add_option_function<std::string>(
      app,
      "--main_pool_cpus",
      [&config](const std::string& value) {
        parse_affinity_mask(config.main_pool_cpu_cfg.mask, value, "main_pool_cpus");
      },
      "CPU cores assigned to main thread pool");

  add_option_function<std::string>(
      app,
      "--main_pool_pinning",
      [&config](const std::string& value) {
        config.main_pool_cpu_cfg.pinning_policy = to_affinity_mask_policy(value);
        if (config.main_pool_cpu_cfg.pinning_policy == sched_affinity_mask_policy::last) {
          report_error("Incorrect value={} used in {} property", value, "main_pool_pinning");
        }
      },
      "Policy used for assigning CPU cores to the main thread pool");
}

static void configure_cli11_expert_execution_args(CLI::App& app, expert_execution_appconfig& config)
{
  // Affinity section.
  CLI::App* affinities_subcmd =
      add_subcommand(app, "affinities", "Application CPU affinities configuration")->configurable();
  configure_cli11_cpu_affinities_args(*affinities_subcmd, config.affinities);

  // Threads section.
  CLI::App* threads_subcmd = add_subcommand(app, "threads", "Threads configuration")->configurable();

  // Main pool threads.
  CLI::App* main_threads_subcmd =
      add_subcommand(*threads_subcmd, "main_pool", "Main thread pool configuration")->configurable();
  configure_cli11_main_pool_threads_args(*main_threads_subcmd, config.threads.main_pool);
}

void srsran::configure_cli11_with_worker_manager_appconfig_schema(CLI::App& app, expert_execution_appconfig& config)
{
  // Expert section.
  CLI::App* expert_subcmd = app.add_subcommand("expert_execution", "Expert execution configuration")->configurable();
  configure_cli11_expert_execution_args(*expert_subcmd, config);
}
