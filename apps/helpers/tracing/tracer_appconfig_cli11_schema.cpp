/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "tracer_appconfig_cli11_schema.h"
#include "tracer_appconfig.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;

static void configure_cli11_tracing_args(CLI::App& app, tracer_appconfig& tracing_params)
{
  app.add_option("--filename",
                 tracing_params.filename,
                 "Set to a valid file path to enable tracing and write the trace to the file")
      ->capture_default_str();
  app.add_option("--max_tracing_events_per_file",
                 tracing_params.max_tracing_events_per_file,
                 "Maximum number of events per file. Set to zero for no limit")
      ->capture_default_str();
  app.add_option("--nof_tracing_events_after_severe",
                 tracing_params.nof_tracing_events_after_severe,
                 "Number of events to write prior to a severe event. Set to zero for writing all events")
      ->capture_default_str();
}

void srsran::configure_cli11_with_tracer_appconfig_schema(CLI::App& app, tracer_appconfig& config)
{
  // Tracing section.
  CLI::App* trace_subcmd = add_subcommand(app, "trace", "General tracer configuration")->configurable();
  configure_cli11_tracing_args(*trace_subcmd, config);
}
