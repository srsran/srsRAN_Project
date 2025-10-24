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

#include "ofh.h"
#include "apps/helpers/metrics/helpers.h"
#include "helpers.h"
#include "srsran/ofh/ofh_metrics.h"

using namespace srsran;
using namespace app_helpers;
using namespace json_generators;

namespace srsran {
namespace ofh {

void to_json(nlohmann::json& json, const closed_rx_window_metrics& metrics)
{
  json["nof_missed_uplink_symbols"]  = metrics.nof_missing_uplink_symbols;
  json["nof_missed_prach_occasions"] = metrics.nof_missing_prach_contexts;
}

void to_json(nlohmann::json& json, const timing_metrics& metrics)
{
  json["nof_skipped_symbols"]       = metrics.nof_skipped_symbols;
  json["skipped_symbols_max_burst"] = metrics.skipped_symbols_max_burst;
}

} // namespace ofh
} // namespace srsran

static nlohmann::json generate_received_messages(const ofh::received_messages_metrics& metrics,
                                                 std::chrono::nanoseconds              symbol_duration)
{
  nlohmann::json json;

  json["total"]           = metrics.nof_early_messages + metrics.nof_late_messages + metrics.nof_on_time_messages;
  json["early"]           = metrics.nof_early_messages;
  json["on_time"]         = metrics.nof_on_time_messages;
  json["late"]            = metrics.nof_late_messages;
  json["earliest_msg_us"] = validate_fp_value(metrics.earliest_rx_msg_in_symbols * symbol_duration.count() * 1e-3);
  json["latest_msg_us"]   = validate_fp_value(metrics.latest_rx_msg_in_symbols * symbol_duration.count() * 1e-3);

  return json;
}

static nlohmann::json generate_ethernet_rx(const ether::receiver_metrics& metrics, unsigned metrics_period_ms)
{
  nlohmann::json json;

  json["average_throughput_mbps"] = validate_fp_value(static_cast<float>(metrics.total_nof_bytes * CHAR_BIT) /
                                                      static_cast<float>(metrics_period_ms * 1e3));
  json["average_latency_us"]      = validate_fp_value(metrics.avg_packet_rx_latency_us);
  json["max_latency_us"]          = validate_fp_value(metrics.max_packet_rx_latency_us);
  json["cpu_usage_percent"]       = validate_fp_value(metrics.cpu_usage_us / (metrics_period_ms * 1e3) * 100.0f);

  return json;
}

static nlohmann::json generate_message_decoder_item(const ofh::rx_data_flow_perf_metrics& metrics,
                                                    unsigned                              metrics_period_ms)
{
  nlohmann::json json;

  json["nof_dropped_messages"] = metrics.nof_dropped_messages;
  json["average_latency_us"]   = validate_fp_value(metrics.message_unpacking_avg_latency_us);
  json["max_latency_us"]       = validate_fp_value(metrics.message_unpacking_max_latency_us);
  json["cpu_usage_percent"]    = validate_fp_value(metrics.cpu_usage_us / (metrics_period_ms * 1e3) * 100.0f);

  return json;
}

static nlohmann::json generate_message_decoder(const ofh::message_decoding_performance_metrics& metrics,
                                               unsigned                                         metrics_period_ms)
{
  nlohmann::json json;

  json["prach"] = generate_message_decoder_item(metrics.prach_processing_metrics, metrics_period_ms);
  json["data"]  = generate_message_decoder_item(metrics.data_processing_metrics, metrics_period_ms);

  auto& ecpri_json                        = json["ecpri"];
  ecpri_json["nof_future_seqid_messages"] = metrics.nof_skipped_messages;
  ecpri_json["nof_past_seqid_messages"]   = metrics.nof_dropped_messages;

  return json;
}

static nlohmann::json generate_uplink(const ofh::receiver_metrics& metrics,
                                      unsigned                     metrics_period_ms,
                                      std::chrono::nanoseconds     symbol_duration)
{
  nlohmann::json json;

  json["received_packets"]  = generate_received_messages(metrics.rx_messages_metrics, symbol_duration);
  json["ethernet_receiver"] = generate_ethernet_rx(metrics.eth_receiver_metrics, metrics_period_ms);
  json["message_decoder"]   = generate_message_decoder(metrics.rx_decoding_perf_metrics, metrics_period_ms);
  json["rx_window_stats"]   = metrics.closed_window_metrics;

  return json;
}

static nlohmann::json generate_ethernet_tx(const ether::transmitter_metrics& metrics, unsigned metrics_period_ms)
{
  nlohmann::json json;

  json["average_throughput_mbps"] = validate_fp_value(static_cast<float>(metrics.total_nof_bytes * CHAR_BIT) /
                                                      static_cast<float>(metrics_period_ms * 1e3));
  json["average_latency_us"]      = validate_fp_value(metrics.avg_packet_tx_latency_us);
  json["max_latency_us"]          = validate_fp_value(metrics.max_packet_tx_latency_us);
  json["cpu_usage_percent"] =
      validate_fp_value(validate_fp_value(metrics.cpu_usage_us / (metrics_period_ms * 1e3) * 100.0f));

  return json;
}

static nlohmann::json generate_tx_data_flow(const ofh::tx_data_flow_perf_metrics& metrics, unsigned metrics_period_ms)
{
  nlohmann::json json;

  json["average_latency_us"] = validate_fp_value(metrics.message_packing_avg_latency_us);
  json["max_latency_us"]     = validate_fp_value(metrics.message_packing_max_latency_us);
  json["cpu_usage_percent"] =
      validate_fp_value(validate_fp_value(metrics.cpu_usage_us / (metrics_period_ms * 1e3) * 100.0f));

  return json;
}

static nlohmann::json generate_message_encoder(const ofh::transmitter_metrics& metrics, unsigned metrics_period_ms)
{
  nlohmann::json json;

  json["dl_cp"] = generate_tx_data_flow(metrics.dl_metrics.dl_cp_metrics, metrics_period_ms);
  json["ul_cp"] = generate_tx_data_flow(metrics.ul_metrics.ul_cp_metrics, metrics_period_ms);
  json["dl_up"] = generate_tx_data_flow(metrics.dl_metrics.dl_up_metrics, metrics_period_ms);

  return json;
}

static nlohmann::json generate_message_transmitter(const ofh::message_transmitter_metrics& metrics,
                                                   unsigned                                metrics_period_ms)
{
  nlohmann::json json;

  json["average_latency_us"] = validate_fp_value(metrics.message_tx_avg_latency_us);
  json["max_latency_us"]     = validate_fp_value(metrics.message_tx_max_latency_us);
  json["cpu_usage_percent"] =
      validate_fp_value(validate_fp_value(metrics.cpu_usage_us / (metrics_period_ms * 1e3) * 100.0f));

  return json;
}

static nlohmann::json generate_transmitter_statistics(const ofh::transmitter_metrics& metrics)
{
  nlohmann::json json;

  json["late_dl_slots"]         = metrics.dl_metrics.nof_late_dl_grids;
  json["late_ul_slot_requests"] = metrics.ul_metrics.nof_late_ul_requests;
  json["late_cp_dl_messages"]   = metrics.dl_metrics.nof_late_cp_dl;
  json["late_up_dl_messages"]   = metrics.dl_metrics.nof_late_up_dl;
  json["late_cp_ul_messages"]   = metrics.ul_metrics.nof_late_cp_ul;

  return json;
}

static nlohmann::json generate_downlink(const ofh::transmitter_metrics& metrics, unsigned metrics_period_ms)
{
  nlohmann::json json;

  json["ethernet_transmitter"] = generate_ethernet_tx(metrics.eth_transmitter_metrics, metrics_period_ms);
  json["message_encoder"]      = generate_message_encoder(metrics, metrics_period_ms);
  json["message_transmitter"]  = generate_message_transmitter(metrics.message_tx_metrics, metrics_period_ms);
  json["transmitter_stats"]    = generate_transmitter_statistics(metrics);

  return json;
}

static nlohmann::json
generate_ofh_cell(const ofh::sector_metrics& metrics, pci_t pci, std::chrono::nanoseconds symbol_duration)
{
  nlohmann::json json;

  json["pci"] = pci;
  json["ul"]  = generate_uplink(metrics.rx_metrics, metrics.metrics_period_ms.count(), symbol_duration);
  json["dl"]  = generate_downlink(metrics.tx_metrics, metrics.metrics_period_ms.count());

  return json;
}

nlohmann::json srsran::app_helpers::json_generators::generate(const ofh::metrics&      metrics,
                                                              span<const pci_t>        pci_sector_map,
                                                              std::chrono::nanoseconds symbol_duration)
{
  nlohmann::json json;

  json["timestamp"]        = get_time_stamp();
  auto& json_ru            = json["ru"];
  auto& json_ofh           = json_ru["ofh"];
  json_ofh["timing_stats"] = metrics.timing;
  auto& cells_ofh          = json_ofh["cells"];

  for (const auto& cell : metrics.sectors) {
    srsran_assert(cell.sector_id < pci_sector_map.size(),
                  "Sector id '{}' out of range of the pci-sector mapper. Size of the mapper is '{}' ",
                  cell.sector_id,
                  pci_sector_map.size());
    cells_ofh.emplace_back(generate_ofh_cell(cell, pci_sector_map[cell.sector_id], symbol_duration));
  }

  return json;
}

std::string srsran::app_helpers::json_generators::generate_string(const ofh::metrics&      metrics,
                                                                  span<const pci_t>        pci_sector_map,
                                                                  std::chrono::nanoseconds symbol_duration,
                                                                  int                      indent)
{
  return generate(metrics, pci_sector_map, symbol_duration).dump(indent);
}
