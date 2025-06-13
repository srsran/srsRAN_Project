/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac.h"
#include "../helpers.h"
#include "srsran/mac/mac_metrics.h"

using namespace srsran;
using namespace app_helpers;
using namespace json_generators;

namespace srsran {

void to_json(nlohmann::json& json, const mac_dl_cell_metric_report& metrics)
{
  json["pci"] = metrics.pci;
  json["average_latency_us"] =
      validate_fp_value(static_cast<double>(metrics.wall_clock_latency.average.count()) / 1000.0);
  json["min_latency_us"] = validate_fp_value(static_cast<double>(metrics.wall_clock_latency.min.count()) / 1000.0);
  json["max_latency_us"] = validate_fp_value(static_cast<double>(metrics.wall_clock_latency.max.count()) / 1000.0);

  double metrics_period = (metrics.slot_duration * metrics.nof_slots).count();
  double cpu_usage =
      validate_fp_value(100.0 * static_cast<double>(metrics.wall_clock_latency.average.count()) / metrics_period);
  json["cpu_usage_percent"] = cpu_usage;
}

} // namespace srsran

nlohmann::json json_generators::generate(const mac_dl_metric_report& metrics)
{
  nlohmann::json json;

  if (metrics.cells.empty()) {
    return json;
  }

  json["dl"] = metrics.cells;

  return json;
}
