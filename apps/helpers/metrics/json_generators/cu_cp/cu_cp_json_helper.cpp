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

#include "cu_cp_json_helper.h"
#include "helpers.h"
#include "srsran/ran/gnb_du_id.h"
#include "fmt/format.h"

using namespace srsran;
using namespace app_helpers;
using namespace json_generators;

namespace srsran {

struct cu_cp_rrc_metrics_json {
  std::vector<cu_cp_metrics_report::du_info> dus;
  mobility_management_metrics                mobility;
};

struct cu_cp_ngap_metrics_json {
  std::vector<ngap_info>      ngaps;
  mobility_management_metrics mobility;
};

void to_json(nlohmann::json& json, const ngap_counter_with_cause& metrics)
{
  // NGAP counter with cause.
  unsigned cause_index = 0;
  for (const auto& count : metrics) {
    json[fmt::format("{}", metrics.get_cause(cause_index))] = count;
    cause_index++;
  }
}

void to_json(nlohmann::json& json, const ngap_metrics& metrics)
{
  // NGAP metrics.
  for (const auto& [nssai, pdu_session_metrics] : metrics.pdu_session_metrics) {
    // PDU session metrics.
    json["s-nssai"] = fmt::format(
        "(sst={} sd={})", nssai.sst.value(), nssai.sd.is_set() ? fmt::format("{}", nssai.sd.value()) : "na");
    json["nof_pdu_sessions_requested_to_setup"] = pdu_session_metrics.nof_pdu_sessions_requested_to_setup;
    json["nof_pdu_sessions_successfully_setup"] = pdu_session_metrics.nof_pdu_sessions_successfully_setup;
    json["nof_pdu_sessions_failed_to_setup"]    = pdu_session_metrics.nof_pdu_sessions_failed_to_setup;
  }
}

void to_json(nlohmann::json& json, const std::vector<plmn_identity>& plmns)
{
  // PLMN identity.
  std::string plmn_str = "[ ";
  for (const auto& plmn : plmns) {
    plmn_str += fmt::format("{}, ", plmn);
  }
  plmn_str += "]";
  json = fmt::format("{}", plmn_str);
}

void to_json(nlohmann::json& json, const ngap_info& metrics)
{
  // NGAP info.
  json["amf_name"]                                = metrics.amf_name;
  json["connected"]                               = metrics.connected;
  json["supported_plmns"]                         = metrics.supported_plmns;
  json["pdu_session_management"]                  = metrics.metrics;
  nlohmann::json& paging_json                     = json["paging_measurement"];
  paging_json["nof_cn_initiated_paging_requests"] = metrics.metrics.nof_cn_initiated_paging_requests;
}

void to_json(nlohmann::json& json, const srsran::cu_cp_ngap_metrics_json& metrics)
{
  // NGAP metrics.
  json["ngap"]                                 = metrics.ngaps;
  json["nof_handover_preparations_requested"]  = metrics.mobility.nof_handover_preparations_requested;
  json["nof_successful_handover_preparations"] = metrics.mobility.nof_successful_handover_preparations;
}

void to_json(nlohmann::json& json, const rrc_connection_establishment_counter_with_cause& metrics)
{
  // RRC connection establishment counter with cause metrics.
  unsigned cause_index = 0;
  for (const auto& count : metrics) {
    json[fmt::format("{}", metrics.get_cause(cause_index))] = count;
    cause_index++;
  }
}

void to_json(nlohmann::json& json, const srsran::cu_cp_metrics_report::du_info& metrics)
{
  // RRC-DU metrics.
  json["gnb_du_id"]                          = metrics.id;
  nlohmann::json& rrc_connection             = json["rrc_connection_number"];
  rrc_connection["mean_nof_rrc_connections"] = metrics.rrc_metrics.mean_nof_rrc_connections;
  rrc_connection["max_nof_rrc_connections"]  = metrics.rrc_metrics.max_nof_rrc_connections;

  nlohmann::json& rrc_connection_establishment = json["rrc_connection_establishment"];
  rrc_connection_establishment["attempted_rrc_connection_establishments"] =
      metrics.rrc_metrics.attempted_rrc_connection_establishments;
  rrc_connection_establishment["successful_rrc_connection_establishments"] =
      metrics.rrc_metrics.successful_rrc_connection_establishments;

  nlohmann::json& rrc_connection_reestablishment = json["rrc_connection_reestablishment"];
  rrc_connection_reestablishment["attempted_rrc_connection_reestablishments"] =
      metrics.rrc_metrics.attempted_rrc_connection_reestablishments;
  rrc_connection_reestablishment["successful_rrc_connection_reestablishments_with_ue_context"] =
      metrics.rrc_metrics.successful_rrc_connection_reestablishments_with_ue_context;
  rrc_connection_reestablishment["successful_rrc_connection_reestablishments_without_ue_context"] =
      metrics.rrc_metrics.successful_rrc_connection_reestablishments_without_ue_context;
}

void to_json(nlohmann::json& json, const cu_cp_rrc_metrics_json& metrics)
{
  // RRC-DU metrics.
  json["du"]                                 = metrics.dus;
  json["nof_handover_executions_requested"]  = metrics.mobility.nof_handover_executions_requested;
  json["nof_successful_handover_executions"] = metrics.mobility.nof_successful_handover_executions;
}

} // namespace srsran

nlohmann::json srsran::app_helpers::json_generators::generate(const cu_cp_metrics_report& report)
{
  cu_cp_rrc_metrics_json  rrc_metrics  = {report.dus, report.mobility};
  cu_cp_ngap_metrics_json ngap_metrics = {report.ngaps, report.mobility};

  nlohmann::json json;

  json["timestamp"]          = get_time_stamp();
  nlohmann::json& cu_cp_json = json["cu-cp"];

  cu_cp_json["id"]    = "srs-cu-cp";
  cu_cp_json["ngaps"] = ngap_metrics;
  cu_cp_json["rrcs"]  = rrc_metrics;

  return json;
}

std::string srsran::app_helpers::json_generators::generate_string(const cu_cp_metrics_report& report, int indent)
{
  return generate(report).dump(indent);
}
