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

#include "scheduler.h"
#include "helpers.h"
#include "json_generators/generator_helpers.h"
#include "srsran/scheduler/scheduler_metrics.h"

using namespace srsran;
using namespace app_helpers;
using namespace json_generators;

static const char* event_to_string(scheduler_cell_event::event_type ev)
{
  switch (ev) {
    case scheduler_cell_event::event_type::ue_add:
      return "ue_create";
    case scheduler_cell_event::event_type::ue_reconf:
      return "ue_reconf";
    case scheduler_cell_event::event_type::ue_rem:
      return "ue_rem";
    default:
      break;
  }
  return "invalid";
}

namespace srsran {

void to_json(nlohmann::json& json, const scheduler_cell_event& metrics)
{
  json["slot"]       = metrics.slot;
  json["rnti"]       = metrics.rnti;
  json["event_type"] = event_to_string(metrics.type);
}

void to_json(nlohmann::json& json, const scheduler_ue_metrics& metrics)
{
  json["pci"]  = metrics.pci;
  json["rnti"] = metrics.rnti;
  if (metrics.cqi_stats.get_nof_observations() > 0) {
    json["cqi"] = static_cast<uint8_t>(std::round(metrics.cqi_stats.get_mean()));
  }

  json["dl_ri"] = metrics.dl_ri_stats.get_nof_observations() > 0 ? metrics.dl_ri_stats.get_mean() : 1;
  json["ul_ri"] = metrics.ul_ri_stats.get_nof_observations() > 0 ? metrics.ul_ri_stats.get_mean() : 1;

  json["dl_mcs"]     = metrics.dl_mcs.to_uint();
  json["dl_brate"]   = metrics.dl_brate_kbps * 1e3;
  json["dl_nof_ok"]  = metrics.dl_nof_ok;
  json["dl_nof_nok"] = metrics.dl_nof_nok;
  json["dl_bs"]      = metrics.dl_bs;
  if (!std::isnan(metrics.pusch_snr_db) && !iszero(metrics.pusch_snr_db)) {
    json["pusch_snr_db"] = std::clamp(metrics.pusch_snr_db, -99.9f, 99.9f);
  }
  if (!std::isnan(metrics.pusch_rsrp_db) && !iszero(metrics.pusch_rsrp_db)) {
    json["pusch_rsrp_db"] = std::clamp(metrics.pusch_rsrp_db, -99.9f, 0.0f);
  }
  if (!std::isnan(metrics.pucch_snr_db) && !iszero(metrics.pucch_snr_db)) {
    json["pucch_snr_db"] = std::clamp(metrics.pucch_snr_db, -99.9f, 99.9f);
  }

  json["ta_ns"] =
      (metrics.ta_stats.get_nof_observations() > 0) ? std::optional{metrics.ta_stats.get_mean() * 1e9} : std::nullopt;
  json["pusch_ta_ns"]                    = (metrics.pusch_ta_stats.get_nof_observations() > 0)
                                               ? std::optional{metrics.pusch_ta_stats.get_mean() * 1e9}
                                               : std::nullopt;
  json["pucch_ta_ns"]                    = (metrics.pucch_ta_stats.get_nof_observations() > 0)
                                               ? std::optional{metrics.pucch_ta_stats.get_mean() * 1e9}
                                               : std::nullopt;
  json["srs_ta_ns"]                      = (metrics.srs_ta_stats.get_nof_observations() > 0)
                                               ? std::optional{metrics.srs_ta_stats.get_mean() * 1e9}
                                               : std::nullopt;
  json["ul_mcs"]                         = metrics.ul_mcs.to_uint();
  json["ul_brate"]                       = metrics.ul_brate_kbps * 1e3;
  json["ul_nof_ok"]                      = metrics.ul_nof_ok;
  json["ul_nof_nok"]                     = metrics.ul_nof_nok;
  json["last_phr"]                       = metrics.last_phr;
  json["bsr"]                            = metrics.bsr;
  json["nof_pucch_f0f1_invalid_harqs"]   = metrics.nof_pucch_f0f1_invalid_harqs;
  json["nof_pucch_f2f3f4_invalid_harqs"] = metrics.nof_pucch_f2f3f4_invalid_harqs;
  json["nof_pucch_f2f3f4_invalid_csis"]  = metrics.nof_pucch_f2f3f4_invalid_csis;
  json["nof_pusch_invalid_harqs"]        = metrics.nof_pusch_invalid_harqs;
  json["nof_pusch_invalid_csis"]         = metrics.nof_pusch_invalid_csis;
  json["avg_ce_delay"]                   = metrics.avg_ce_delay_ms;
  json["max_ce_delay"]                   = metrics.max_ce_delay_ms;
  json["avg_crc_delay"]                  = metrics.avg_crc_delay_ms;
  json["max_crc_delay"]                  = metrics.max_crc_delay_ms;
  json["avg_pusch_harq_delay"]           = metrics.avg_pusch_harq_delay_ms;
  json["max_pusch_harq_delay"]           = metrics.max_pusch_harq_delay_ms;
  json["avg_pucch_harq_delay"]           = metrics.avg_pucch_harq_delay_ms;
  json["max_pucch_harq_delay"]           = metrics.max_pucch_harq_delay_ms;
}

void to_json(nlohmann::json& json, const scheduler_cell_metrics& metrics)
{
  // Cell metrics.
  auto& cell_json                      = json["cell_metrics"];
  cell_json["error_indication_count"]  = metrics.nof_error_indications;
  cell_json["average_latency"]         = metrics.average_decision_latency.count();
  cell_json["max_latency"]             = metrics.max_decision_latency.count();
  cell_json["nof_failed_pdcch_allocs"] = metrics.nof_failed_pdcch_allocs;
  cell_json["nof_failed_uci_allocs"]   = metrics.nof_failed_uci_allocs;
  cell_json["latency_histogram"]       = metrics.latency_histogram;
  cell_json["msg3_nof_ok"]             = metrics.nof_msg3_ok;
  cell_json["msg3_nof_nok"]            = metrics.nof_msg3_nok;
  cell_json["avg_prach_delay"]         = metrics.avg_prach_delay_ms;
  cell_json["late_dl_harqs"]           = metrics.nof_failed_pdsch_allocs_late_harqs;
  cell_json["late_ul_harqs"]           = metrics.nof_failed_pusch_allocs_late_harqs;
  cell_json["pucch_tot_rb_usage_avg"]  = metrics.pucch_tot_rb_usage_avg;

  if (!metrics.ue_metrics.empty()) {
    json["ue_list"] = metrics.ue_metrics;
  }
  if (!metrics.events.empty()) {
    json["event_list"] = metrics.events;
  }
}

} // namespace srsran

nlohmann::json srsran::app_helpers::json_generators::generate(const scheduler_metrics_report& metrics)
{
  nlohmann::json json;

  json["timestamp"] = get_time_stamp();
  json["cells"]     = metrics.cells;

  return json;
}

std::string srsran::app_helpers::json_generators::generate_string(const scheduler_metrics_report& metrics, int indent)
{
  return generate(metrics).dump(indent);
}
