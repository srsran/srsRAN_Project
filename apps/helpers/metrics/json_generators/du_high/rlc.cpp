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

#include "rlc.h"
#include "helpers.h"
#include "srsran/rlc/rlc_metrics.h"

using namespace srsran;
using namespace app_helpers;
using namespace json_generators;

static nlohmann::json generate_tx_low_pdu_latency_histogram(unsigned id, unsigned count)
{
  nlohmann::json json;

  json["pull_latency_bin_start_usec"] = id * rlc_tx_metrics_lower::nof_usec_per_bin;
  json["pull_latency_bin_count"]      = count;

  return json;
}

namespace srsran {

void to_json(nlohmann::json& json, const rlc_rx_metrics& metrics)
{
  json["num_sdus"]           = metrics.num_sdus;
  json["num_sdu_bytes"]      = metrics.num_sdu_bytes;
  json["num_pdus"]           = metrics.num_pdus;
  json["num_pdu_bytes"]      = metrics.num_pdu_bytes;
  json["num_lost_pdus"]      = metrics.num_lost_pdus;
  json["num_malformed_pdus"] = metrics.num_malformed_pdus;
}

void to_json(nlohmann::json& json, const rlc_tx_metrics& metrics)
{
  json["num_sdus"]             = metrics.tx_high.num_sdus;
  json["num_sdu_bytes"]        = metrics.tx_high.num_sdu_bytes;
  json["num_dropped_sdus"]     = metrics.tx_high.num_dropped_sdus;
  json["num_discarded_sdus"]   = metrics.tx_high.num_discarded_sdus;
  json["num_discard_failures"] = metrics.tx_high.num_discard_failures;
  json["sum_sdu_latency_us"]   = metrics.tx_low.sum_sdu_latency_us;
  json["sum_pdu_latency_ns"]   = metrics.tx_low.sum_pdu_latency_ns;
  json["max_pdu_latency_ns"]   = metrics.tx_low.max_pdu_latency_ns;

  for (unsigned i = 0; i != rlc_tx_metrics_lower::pdu_latency_hist_bins; ++i) {
    json["pull_latency_histogram"].emplace_back(
        generate_tx_low_pdu_latency_histogram(i, metrics.tx_low.pdu_latency_hist_ns[i]));
  }
}

void to_json(nlohmann::json& json, const rlc_metrics& metrics)
{
  json["du_id"]  = metrics.du_index;
  json["ue_id"]  = metrics.ue_index;
  json["drb_id"] = metrics.rb_id.get_drb_id();
  json["tx"]     = metrics.tx;
  json["rx"]     = metrics.rx;
}

} // namespace srsran

nlohmann::json srsran::app_helpers::json_generators::generate(const rlc_metrics& metrics)
{
  nlohmann::json json;

  json["timestamp"]   = get_time_stamp();
  json["rlc_metrics"] = metrics;

  return json;
}

std::string srsran::app_helpers::json_generators::generate_string(const rlc_metrics& metrics, int indent)
{
  return generate(metrics).dump(indent);
}
