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

#include "o_du_high.h"
#include "apps/helpers/metrics/helpers.h"
#include "du_high/mac.h"
#include "srsran/du/du_high/du_metrics_report.h"

using namespace srsran;
using namespace app_helpers;
using namespace json_generators;

nlohmann::json srsran::app_helpers::json_generators::generate(const srs_du::du_metrics_report& metrics)
{
  nlohmann::json json;

  json["timestamp"]  = get_time_stamp();
  auto& json_du      = json["du"];
  auto& json_du_high = json_du["du_high"];

  if (auto mac_json = generate(metrics.mac->dl); !mac_json.is_null()) {
    json_du_high["mac"] = std::move(mac_json);
  }

  return json;
}

std::string srsran::app_helpers::json_generators::generate_string(const srs_du::du_metrics_report& metrics, int indent)
{
  return generate(metrics).dump(indent);
}
