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

#include "resource_usage.h"
#include "apps/helpers/metrics/helpers.h"
#include "srsran/support/resource_usage/resource_usage_metrics.h"

using namespace srsran;
using namespace app_helpers;
using namespace json_generators;

/// Bytes in one Mega Byte.
static constexpr double BYTES_IN_MB = (1 << 20);

namespace srsran {

void to_json(nlohmann::json& json, const resource_usage_metrics& metrics)
{
  // Convert used memory to Megabytes.
  double mem_usage = validate_fp_value(static_cast<double>(metrics.memory_stats.memory_usage.value()) / BYTES_IN_MB);

  json["cpu_usage_percent"] = validate_fp_value(validate_fp_value(metrics.cpu_stats.cpu_usage_percentage * 100.0));
  json["memory_usage_mb"]   = validate_fp_value(mem_usage);
  json["power_consumption_watts"] = validate_fp_value(metrics.power_usage_watts);
}

} // namespace srsran

nlohmann::json srsran::app_helpers::json_generators::generate(const resource_usage_metrics& metrics)
{
  nlohmann::json json;

  json["timestamp"]          = get_time_stamp();
  json["app_resource_usage"] = metrics;

  return json;
}

std::string srsran::app_helpers::json_generators::generate_string(const resource_usage_metrics& metrics, int indent)
{
  return generate(metrics).dump(indent);
}
