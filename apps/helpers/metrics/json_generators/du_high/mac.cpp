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

#include "mac.h"
#include "helpers.h"
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

nlohmann::json srsran::app_helpers::json_generators::generate(const mac_dl_metric_report& metrics)
{
  nlohmann::json json;

  if (metrics.cells.empty()) {
    return json;
  }

  json["dl"] = metrics.cells;

  return json;
}
