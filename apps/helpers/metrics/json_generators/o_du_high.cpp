/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "o_du_high.h"
#include "du_high/mac.h"
#include "helpers.h"
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

std::string json_generators::generate_string(const srs_du::du_metrics_report& metrics, int indent)
{
  return generate(metrics).dump(indent);
}
