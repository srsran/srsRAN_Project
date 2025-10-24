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

#include "o_du_low.h"
#include "apps/helpers/metrics/helpers.h"
#include "generator_helpers.h"
#include "srsran/du/du_low/o_du_low_metrics.h"

using namespace srsran;
using namespace app_helpers;
using namespace json_generators;

static nlohmann::json generate_ldpc_encoder(const upper_phy_ldpc_metrics::ldpc_encoder_metrics& metrics,
                                            double                                              metric_period_us)
{
  nlohmann::json json;

  json["avg_cb_size_bits"]        = validate_fp_value(metrics.avg_cb_size);
  json["average_latency_us"]      = validate_fp_value(metrics.avg_cb_latency_us);
  json["min_latency_us"]          = validate_fp_value(metrics.min_cb_latency_us);
  json["max_latency_us"]          = validate_fp_value(metrics.max_cb_latency_us);
  json["average_throughput_mbps"] = validate_fp_value(metrics.encoding_rate_Mbps);
  double cpu_usage                = validate_fp_value(100.0 * metrics.cpu_usage_us / metric_period_us);
  json["cpu_usage_percent"]       = validate_fp_value(cpu_usage);

  return json;
}

static nlohmann::json generate_rate_matcher(const upper_phy_ldpc_metrics::ldpc_rate_match_metrics& metrics,
                                            double                                                 metric_period_us)
{
  nlohmann::json json;

  json["average_latency_us"]      = validate_fp_value(metrics.avg_cb_latency);
  json["min_latency_us"]          = validate_fp_value(metrics.min_cb_latency_us);
  json["max_latency_us"]          = validate_fp_value(metrics.max_cb_latency_us);
  json["average_throughput_mbps"] = validate_fp_value(metrics.processing_rate_Mbps);
  double cpu_usage                = validate_fp_value(100.0 * metrics.cpu_usage_us / metric_period_us);
  json["cpu_usage_percent"]       = validate_fp_value(cpu_usage);

  return json;
}

static nlohmann::json generate_scrambling(const upper_phy_scrambling_metrics& metrics, double metric_period_us)
{
  nlohmann::json json;

  double cpu_usage          = validate_fp_value(100.0 * metrics.cpu_usage_us / metric_period_us);
  json["cpu_usage_percent"] = validate_fp_value(cpu_usage);

  return json;
}

static nlohmann::json generate_modulation_mapper(const upper_phy_mod_demod_evm_metrics& metrics,
                                                 double                                 metric_period_us)
{
  nlohmann::json json;

  json["qpsk_mod_throughput_mbps"]   = validate_fp_value(metrics.qpsk_avg_rate_Mbps);
  json["qam16_mod_throughput_mbps"]  = validate_fp_value(metrics.qam16_avg_rate_Mbps);
  json["qam64_mod_throughput_mbps"]  = validate_fp_value(metrics.qam64_avg_rate_Mbps);
  json["qam256_mod_throughput_mbps"] = validate_fp_value(metrics.qam256_avg_rate_Mbps);

  double cpu_usage          = validate_fp_value(100.0 * metrics.cpu_usage_us / metric_period_us);
  json["cpu_usage_percent"] = validate_fp_value(cpu_usage);

  return json;
}

static nlohmann::json generate_precoding_mapping(const upper_phy_pdsch_metrics::pdsch_layermap_precode_metrics& metrics,
                                                 double   metric_period_us,
                                                 uint64_t tbs_processed)
{
  nlohmann::json json;

  json["average_latency_us"] =
      validate_fp_value(static_cast<double>(metrics.total_latency_ns) / static_cast<double>(tbs_processed) / 100.0);
  json["throughput_per_nof_layers_mresps"] = {validate_fp_value(metrics.avg_rate_MRes_one_layer),
                                              validate_fp_value(metrics.avg_rate_MRes_two_layers),
                                              validate_fp_value(metrics.avg_rate_MRes_three_layers),
                                              validate_fp_value(metrics.avg_rate_MRes_four_layers)};

  double cpu_usage          = validate_fp_value(100.0 * metrics.cpu_usage_us / metric_period_us);
  json["cpu_usage_percent"] = validate_fp_value(cpu_usage);

  return json;
}

static nlohmann::json generate_dl_fec(const upper_phy_ldpc_metrics::ldpc_encoder_metrics& metrics_enc, double cpu_usage)
{
  nlohmann::json json;

  json["average_throughput_mbps"] = validate_fp_value(metrics_enc.encoding_rate_Mbps);
  json["cpu_usage_percent"]       = validate_fp_value(cpu_usage);

  return json;
}

static nlohmann::json generate_cell_downlink(const upper_phy_metrics& metrics)
{
  nlohmann::json json;

  double metric_period_us = metrics.metrics_period.count();

  json["ldpc_encoder"]      = generate_ldpc_encoder(metrics.ldpc_metrics.encoder_metrics, metric_period_us);
  json["ldpc_rate_matcher"] = generate_rate_matcher(metrics.ldpc_metrics.rate_match_metrics, metric_period_us);
  json["scrambling"]        = generate_scrambling(metrics.pdsch_metrics.scrambling_metrics, metric_period_us);
  json["modulation_mapper"] = generate_modulation_mapper(metrics.pdsch_metrics.modulator_metrics, metric_period_us);
  json["precoding_layer_mapping"] = generate_precoding_mapping(metrics.pdsch_metrics.precoding_metrics,
                                                               metric_period_us,
                                                               metrics.pdsch_metrics.pdsch_proc_metrics.tbs_processed);

  double dl_fec_cpu_usage = validate_fp_value(100.0 *
                                              (metrics.ldpc_metrics.encoder_metrics.cpu_usage_us +
                                               metrics.ldpc_metrics.rate_match_metrics.cpu_usage_us +
                                               metrics.pdsch_metrics.crc_metrics.cpu_usage_us) /
                                              metric_period_us);

  json["fec"] = generate_dl_fec(metrics.ldpc_metrics.encoder_metrics, dl_fec_cpu_usage);

  json["average_latency_us"] = validate_fp_value(metrics.dl_processor_metrics.avg_latency_us);
  json["max_latency_us"]     = validate_fp_value(metrics.dl_processor_metrics.max_latency_us.first);
  json["max_latency_slot"]   = metrics.dl_processor_metrics.max_latency_us.second;

  double dmrs_cpu_usage = validate_fp_value(100.0 * metrics.pdsch_metrics.dmrs_metrics.cpu_usage_us / metric_period_us);
  double scrmbl_cpu_usage =
      validate_fp_value(100.0 * metrics.pdsch_metrics.scrambling_metrics.cpu_usage_us / metric_period_us);
  double mod_cpu_usage =
      validate_fp_value(100.0 * metrics.pdsch_metrics.modulator_metrics.cpu_usage_us / metric_period_us);
  double upper_phy_dl_cpu_usage =
      validate_fp_value(100.0 * metrics.pdsch_metrics.pdsch_proc_metrics.cpu_usage_us / metric_period_us +
                        dmrs_cpu_usage + dl_fec_cpu_usage + scrmbl_cpu_usage + mod_cpu_usage);
  json["cpu_usage_percent"] = upper_phy_dl_cpu_usage;

  return json;
}

static nlohmann::json generate_ldpc_decoder(const upper_phy_ldpc_metrics::ldpc_decoder_metrics& metrics,
                                            double                                              metric_period_us)
{
  nlohmann::json json;

  json["average_cb_size_bits"]    = validate_fp_value(metrics.avg_cb_size);
  json["average_latency_us"]      = validate_fp_value(metrics.avg_cb_latency_us);
  json["min_latency_us"]          = validate_fp_value(metrics.min_cb_latency_us);
  json["max_latency_us"]          = validate_fp_value(metrics.max_cb_latency_us);
  json["average_throughput_mbps"] = validate_fp_value(metrics.decoding_rate_Mbps);
  double cpu_usage                = validate_fp_value(100.0 * metrics.cpu_usage_us / metric_period_us);
  json["cpu_usage_percent"]       = validate_fp_value(cpu_usage);

  return json;
}

static nlohmann::json generate_rate_dematcher(const upper_phy_ldpc_metrics::ldpc_rate_dematch_metrics& metrics,
                                              double                                                   metric_period_us)
{
  nlohmann::json json;

  json["average_latency_us"]      = validate_fp_value(metrics.avg_cb_latency);
  json["min_latency_us"]          = validate_fp_value(metrics.min_cb_latency_us);
  json["max_latency_us"]          = validate_fp_value(metrics.max_cb_latency_us);
  json["average_throughput_mbps"] = validate_fp_value(metrics.processing_rate_Mbps);
  double cpu_usage                = validate_fp_value(100.0 * metrics.cpu_usage_us / metric_period_us);
  json["cpu_usage_percent"]       = validate_fp_value(cpu_usage);

  return json;
}

static nlohmann::json generate_descrambling(const upper_phy_scrambling_metrics& metrics, double metric_period_us)
{
  nlohmann::json json;

  double cpu_usage          = validate_fp_value(100.0 * metrics.cpu_usage_us / metric_period_us);
  json["cpu_usage_percent"] = validate_fp_value(cpu_usage);

  return json;
}

static nlohmann::json generate_demodulation_mapper(const upper_phy_mod_demod_evm_metrics& metrics,
                                                   double                                 metric_period_us)
{
  nlohmann::json json;

  json["qpsk_mod_throughput_mbps"]   = validate_fp_value(metrics.qpsk_avg_rate_Mbps);
  json["qam16_mod_throughput_mbps"]  = validate_fp_value(metrics.qam16_avg_rate_Mbps);
  json["qam64_mod_throughput_mbps"]  = validate_fp_value(metrics.qam64_avg_rate_Mbps);
  json["qam256_mod_throughput_mbps"] = validate_fp_value(metrics.qam256_avg_rate_Mbps);

  double cpu_usage          = validate_fp_value(100.0 * metrics.cpu_usage_us / metric_period_us);
  json["cpu_usage_percent"] = validate_fp_value(cpu_usage);

  return json;
}

static nlohmann::json generate_channel_estimation(const upper_phy_pusch_metrics::pusch_ch_estimator_metrics& metrics,
                                                  double metric_period_us)
{
  nlohmann::json json;

  json["average_latency_us"]      = validate_fp_value(metrics.avg_processing_latency);
  json["min_latency_us"]          = validate_fp_value(metrics.min_proc_latency_us);
  json["max_latency_us"]          = validate_fp_value(metrics.max_proc_latency_us);
  json["average_throughput_mbps"] = validate_fp_value(metrics.processing_rate_Mbps);

  double cpu_usage          = validate_fp_value(100.0 * metrics.cpu_usage_us / metric_period_us);
  json["cpu_usage_percent"] = validate_fp_value(cpu_usage);

  return json;
}

static nlohmann::json generate_transform_precoder(const upper_phy_pusch_metrics::pusch_xform_precoder_metrics& metrics,
                                                  double metric_period_us)
{
  nlohmann::json json;

  json["average_latency_us"]       = validate_fp_value(metrics.avg_latency_us);
  json["average_throughput_mreps"] = validate_fp_value(metrics.avg_rate_MREps);

  double cpu_usage          = validate_fp_value(100.0 * metrics.cpu_usage_us / metric_period_us);
  json["cpu_usage_percent"] = validate_fp_value(cpu_usage);

  return json;
}

static nlohmann::json generate_ul_fec(const upper_phy_ldpc_metrics::ldpc_decoder_metrics& metrics_enc, double cpu_usage)
{
  nlohmann::json json;

  json["average_throughput_mbps"] = validate_fp_value(metrics_enc.decoding_rate_Mbps);
  json["cpu_usage_percent"]       = validate_fp_value(cpu_usage);

  return json;
}

static nlohmann::json generate_algorithm_efficiency(const upper_phy_pusch_metrics::pusch_processing_metrics& metrics)
{
  nlohmann::json json;

  json["bler"]    = validate_fp_value(metrics.decoding_bler);
  json["evm"]     = validate_fp_value(metrics.evm);
  json["sinr_db"] = validate_fp_value(metrics.sinr_dB);

  return json;
}

static nlohmann::json generate_cell_uplink(const upper_phy_metrics& metrics)
{
  nlohmann::json json;

  double metric_period_us = metrics.metrics_period.count();

  json["ldpc_decoder"]        = generate_ldpc_decoder(metrics.ldpc_metrics.decoder_metrics, metric_period_us);
  json["ldpc_rate_dematcher"] = generate_rate_dematcher(metrics.ldpc_metrics.rate_dematch_metrics, metric_period_us);
  json["descrambling"]        = generate_descrambling(metrics.pusch_metrics.scrambling_metrics, metric_period_us);
  json["demodulation_mapper"] =
      generate_demodulation_mapper(metrics.pusch_metrics.demod_demapper_metrics, metric_period_us);
  json["channel_estimation"] =
      generate_channel_estimation(metrics.pusch_metrics.ch_estimator_metrics, metric_period_us);
  json["transform_precoder"] =
      generate_transform_precoder(metrics.pusch_metrics.xform_precoder_metrics, metric_period_us);

  double ul_fec_cpu_usage = validate_fp_value(100.0 *
                                              (metrics.ldpc_metrics.decoder_metrics.cpu_usage_us +
                                               metrics.ldpc_metrics.rate_dematch_metrics.cpu_usage_us +
                                               metrics.pusch_metrics.crc_metrics.cpu_usage_us) /
                                              metric_period_us);
  json["fec"]             = generate_ul_fec(metrics.ldpc_metrics.decoder_metrics, ul_fec_cpu_usage);

  const upper_phy_pusch_metrics::pusch_processing_metrics& pusch_proc_metrics =
      metrics.pusch_metrics.pusch_proc_metrics;

  json["algo_efficiency"] = generate_algorithm_efficiency(pusch_proc_metrics);

  json["average_latency_us"]      = validate_fp_value(pusch_proc_metrics.avg_data_latency_us);
  json["max_latency_us"]          = validate_fp_value(pusch_proc_metrics.max_data_latency_us.first);
  json["max_latency_slot"]        = pusch_proc_metrics.max_data_latency_us.second;
  json["average_throughput_mbps"] = validate_fp_value(pusch_proc_metrics.processing_rate_Mbps);

  // Since FEC is running asynchronously in a separate thread pool, we add its CPU usage to the rest of L1 processing.
  double upper_phy_ul_cpu_usage =
      validate_fp_value(100.0 * pusch_proc_metrics.cpu_usage_us / metric_period_us) + ul_fec_cpu_usage;
  json["cpu_usage_percent"] = upper_phy_ul_cpu_usage;

  return json;
}

static nlohmann::json generate_upper_phy_cell(const upper_phy_metrics& metrics, pci_t pci)
{
  nlohmann::json json;

  json["pci"] = pci;
  json["dl"]  = generate_cell_downlink(metrics);
  json["ul"]  = generate_cell_uplink(metrics);

  return json;
}

static nlohmann::json generate_upper_phy(span<const upper_phy_metrics> upper_phy, span<const pci_t> pci_sector_map)
{
  nlohmann::json json;

  for (auto& upper : upper_phy) {
    srsran_assert(upper.sector_id < pci_sector_map.size(),
                  "Invalid sector identifier '{}'. PCI look up table size is '{}'",
                  upper.sector_id,
                  pci_sector_map.size());
    json.emplace_back(generate_upper_phy_cell(upper, pci_sector_map[upper.sector_id]));
  }

  return json;
}

nlohmann::json srsran::app_helpers::json_generators::generate(const srs_du::o_du_low_metrics& metrics,
                                                              span<const pci_t>               pci_sector_map)
{
  nlohmann::json json;

  json["timestamp"]        = get_time_stamp();
  auto& json_du_low        = json["du_low"];
  json_du_low["upper_phy"] = generate_upper_phy(metrics.du_lo_metrics.sector_metrics, pci_sector_map);

  return json;
}

std::string srsran::app_helpers::json_generators::generate_string(const srs_du::o_du_low_metrics& metrics,
                                                                  span<const pci_t>               pci_sector_map,
                                                                  int                             indent)
{
  nlohmann::json json = generate(metrics, pci_sector_map);

  return json.dump(indent);
}
