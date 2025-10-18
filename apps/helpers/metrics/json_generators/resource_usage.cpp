/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
