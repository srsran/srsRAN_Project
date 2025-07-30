/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcp.h"
#include "apps/helpers/metrics/helpers.h"
#include "helpers.h"
#include "srsran/pdcp/pdcp_entity.h"
#include "srsran/pdcp/pdcp_rx_metrics.h"
#include "srsran/pdcp/pdcp_tx_metrics.h"

using namespace srsran;
using namespace app_helpers;
using namespace json_generators;

static nlohmann::json generate_pdcp_tx(const pdcp_tx_metrics_container& metrics, unsigned period, double cpu_usage)
{
  nlohmann::json json;

  json["average_latency_us"] = validate_fp_value(metrics.num_pdus ? static_cast<double>(metrics.sum_pdu_latency_ns) *
                                                                        1e-3 / static_cast<double>(metrics.num_pdus)
                                                                  : 0.0);

  if (metrics.min_pdu_latency_ns) {
    json["min_latency_us"] = validate_fp_value(static_cast<double>(metrics.min_pdu_latency_ns.value()) * 1e-3);
  }

  if (metrics.max_pdu_latency_ns) {
    json["max_latency_us"] = validate_fp_value(static_cast<double>(metrics.max_pdu_latency_ns.value()) * 1e-3);
  }

  json["average_throughput_Mbps"] =
      validate_fp_value(static_cast<double>(metrics.num_pdu_bytes) * 8.0 * 1000.0 * 1e-6 / static_cast<double>(period));
  json["cpu_usage_percent"] = validate_fp_value(cpu_usage);

  return json;
}

static nlohmann::json generate_pdcp_rx(const pdcp_rx_metrics_container& metrics, unsigned period, double cpu_usage)
{
  nlohmann::json json;

  json["average_latency_us"] = validate_fp_value(metrics.num_pdus ? static_cast<double>(metrics.sum_sdu_latency_ns) *
                                                                        1e-3 / static_cast<double>(metrics.num_pdus)
                                                                  : 0.0);

  if (metrics.min_sdu_latency_ns) {
    json["min_latency_us"] = validate_fp_value(static_cast<double>(metrics.min_sdu_latency_ns.value()) * 1e-3);
  }

  if (metrics.max_sdu_latency_ns) {
    json["max_latency_us"] = validate_fp_value(static_cast<double>(metrics.max_sdu_latency_ns.value()) * 1e-3);
  }

  json["average_throughput_Mbps"] =
      validate_fp_value(static_cast<double>(metrics.num_pdu_bytes) * 8.0 * 1000.0 * 1e-6 / static_cast<double>(period));
  json["cpu_usage_percent"] = validate_fp_value(cpu_usage);

  return json;
}

nlohmann::json srsran::app_helpers::json_generators::generate(const pdcp_tx_metrics_container& tx,
                                                              const pdcp_rx_metrics_container& rx,
                                                              double                           tx_cpu_usage,
                                                              double                           rx_cpu_usage,
                                                              timer_duration                   metrics_period)
{
  nlohmann::json json;

  json["timestamp"]          = get_time_stamp();
  nlohmann::json& cu_up_json = json["cu-up"];
  nlohmann::json& pdcp_json  = cu_up_json["pdcp"];

  pdcp_json["dl"] = generate_pdcp_tx(tx, metrics_period.count(), tx_cpu_usage);
  pdcp_json["ul"] = generate_pdcp_rx(rx, metrics_period.count(), rx_cpu_usage);

  return json;
}

std::string srsran::app_helpers::json_generators::generate_string(const pdcp_tx_metrics_container& tx,
                                                                  const pdcp_rx_metrics_container& rx,
                                                                  double                           tx_cpu_usage,
                                                                  double                           rx_cpu_usage,
                                                                  timer_duration                   metrics_period,
                                                                  int                              indent)
{
  return generate(tx, rx, tx_cpu_usage, rx_cpu_usage, metrics_period).dump(indent);
}
