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

#include "executors.h"
#include "apps/helpers/metrics/helpers.h"
#include "srsran/support/executors/metrics/executor_metrics.h"

using namespace srsran;
using namespace app_helpers;
using namespace json_generators;

namespace srsran {

void to_json(nlohmann::json& json, const executor_metrics& metrics)
{
  json["name"]                  = metrics.name;
  json["nof_executes"]          = metrics.nof_executes;
  json["nof_defers"]            = metrics.nof_defers;
  json["enqueue_avg"]           = metrics.avg_enqueue_latency_us.count();
  json["enqueue_max"]           = metrics.max_enqueue_latency_us.count();
  json["task_avg"]              = metrics.avg_task_us.count();
  json["task_max"]              = metrics.max_task_us.count();
  json["cpu_load"]              = validate_fp_value(metrics.cpu_load);
  json["nof_vol_ctxt_switch"]   = metrics.total_rusg.vol_ctxt_switch_count;
  json["nof_invol_ctxt_switch"] = metrics.total_rusg.invol_ctxt_switch_count;
}

} // namespace srsran

nlohmann::json srsran::app_helpers::json_generators::generate(const executor_metrics& metrics)
{
  nlohmann::json json;

  json["timestamp"]        = get_time_stamp();
  json["executor_metrics"] = metrics;

  return json;
}

std::string srsran::app_helpers::json_generators::generate_string(const executor_metrics& metrics, int indent)
{
  return generate(metrics).dump(indent);
}
