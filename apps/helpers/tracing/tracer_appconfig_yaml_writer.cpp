/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "tracer_appconfig_yaml_writer.h"
#include "tracer_appconfig.h"

using namespace srsran;

void srsran::fill_tracer_appconfig_in_yaml_schema(YAML::Node& node, const tracer_appconfig& config)
{
  YAML::Node log_node                         = node["trace"];
  log_node["filename"]                        = config.filename;
  log_node["max_tracing_events_per_file"]     = config.max_tracing_events_per_file;
  log_node["nof_tracing_events_after_severe"] = config.nof_tracing_events_after_severe;
}
