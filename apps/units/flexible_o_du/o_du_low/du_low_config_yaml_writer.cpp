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

#include "du_low_config_yaml_writer.h"
#include "apps/helpers/metrics/metrics_config_yaml_writer.h"
#include "du_low_config.h"

using namespace srsran;

static void fill_du_low_log_section(YAML::Node node, const du_low_unit_logger_config& config)
{
  node["phy_level"]         = srslog::basic_level_to_string(config.phy_level);
  node["hal_level"]         = srslog::basic_level_to_string(config.hal_level);
  node["broadcast_enabled"] = config.broadcast_enabled;
  node["hex_max_size"]      = config.hex_max_size;
  if (!config.phy_rx_symbols_filename.empty()) {
    node["phy_rx_symbols_filename"] = config.phy_rx_symbols_filename;
  }
  if (config.phy_rx_symbols_port.has_value()) {
    node["phy_rx_symbols_port"] = config.phy_rx_symbols_port.value();
  }
  node["phy_rx_symbols_prach"] = config.phy_rx_symbols_prach;
}

static void fill_du_low_expert_execution_section(YAML::Node node, const du_low_unit_expert_execution_config& config)
{
  {
    YAML::Node threads_node                     = node["threads"];
    YAML::Node upper_node                       = threads_node["upper_phy"];
    upper_node["pdsch_processor_type"]          = config.threads.pdsch_processor_type;
    upper_node["pdsch_cb_batch_length"]         = config.threads.pdsch_cb_batch_length;
    upper_node["max_pucch_concurrency"]         = config.threads.max_pucch_concurrency;
    upper_node["max_pusch_and_srs_concurrency"] = config.threads.max_pusch_and_srs_concurrency;
    upper_node["max_pdsch_concurrency"]         = config.threads.max_pdsch_concurrency;
  }

  auto cell_affinities_node = node["cell_affinities"];
  while (config.cell_affinities.size() > cell_affinities_node.size()) {
    cell_affinities_node.push_back(YAML::Node());
  }

  unsigned index = 0;
  for (auto cell : cell_affinities_node) {
    const auto& expert = config.cell_affinities[index];

    if (expert.l1_dl_cpu_cfg.mask.any()) {
      cell["l1_dl_cpus"] = fmt::format("{:,}", span<const size_t>(expert.l1_dl_cpu_cfg.mask.get_cpu_ids()));
    }
    cell["l1_dl_pinning"] = to_string(expert.l1_dl_cpu_cfg.pinning_policy);

    ++index;
  }
}

static void fill_du_low_expert_section(YAML::Node node, const du_low_unit_expert_upper_phy_config& config)
{
  node["max_proc_delay"]                           = config.max_processing_delay_slots;
  node["pusch_dec_max_iterations"]                 = config.pusch_decoder_max_iterations;
  node["pusch_dec_enable_early_stop"]              = config.pusch_decoder_early_stop;
  node["pusch_decoder_force_decoding"]             = config.pusch_decoder_force_decoding;
  node["pusch_sinr_calc_method"]                   = config.pusch_sinr_calc_method;
  node["pusch_channel_estimator_fd_strategy"]      = config.pusch_channel_estimator_fd_strategy;
  node["pusch_channel_estimator_td_strategy"]      = config.pusch_channel_estimator_td_strategy;
  node["pusch_channel_estimator_cfo_compensation"] = config.pusch_channel_estimator_cfo_compensation;
  node["pusch_channel_equalizer_algorithm"]        = config.pusch_channel_equalizer_algorithm;
  node["max_request_headroom_slots"]               = config.nof_slots_request_headroom;
  node["allow_request_on_empty_uplink_slot"]       = config.allow_request_on_empty_uplink_slot;
  node["enable_phy_tap"]                           = config.enable_phy_tap;
}

static void fill_du_low_bbdev_pdsch_enc_section(YAML::Node node, const hwacc_pdsch_appconfig& config)
{
  node["nof_hwacc"]       = config.nof_hwacc;
  node["cb_mode"]         = config.cb_mode;
  node["dedicated_queue"] = config.dedicated_queue;
  if (config.max_buffer_size) {
    node["max_buffer_size"] = config.max_buffer_size.value();
  }
}

static void fill_du_low_bbdev_pusch_dec_section(YAML::Node node, const hwacc_pusch_appconfig& config)
{
  node["nof_hwacc"]        = config.nof_hwacc;
  node["force_local_harq"] = config.force_local_harq;
  node["dedicated_queue"]  = config.dedicated_queue;
  if (config.harq_context_size) {
    node["harq_context_size"] = config.harq_context_size.value();
  }
}

static void fill_du_low_bbdev_section(YAML::Node node, const bbdev_appconfig& config)
{
  node["hwacc_type"] = config.hwacc_type;
  node["id"]         = config.id;

  if (config.pdsch_enc) {
    fill_du_low_bbdev_pdsch_enc_section(node["pdsch_enc"], config.pdsch_enc.value());
  }
  if (config.pusch_dec) {
    fill_du_low_bbdev_pusch_dec_section(node["pusch_dec"], config.pusch_dec.value());
  }
  if (config.msg_mbuf_size) {
    node["msg_mbuf_size"] = config.msg_mbuf_size.value();
  }
  if (config.rm_mbuf_size) {
    node["rm_mbuf_size"] = config.rm_mbuf_size.value();
  }
  if (config.nof_mbuf) {
    node["nof_mbuf"] = config.nof_mbuf.value();
  }
}

static void fill_du_low_hal_section(YAML::Node node, const du_low_unit_hal_config& config)
{
  if (config.bbdev_hwacc) {
    fill_du_low_bbdev_section(node["bbdev_hwacc"], config.bbdev_hwacc.value());
  }
}

static void fill_du_low_metrics_section(YAML::Node node, const du_low_unit_metrics_config& config)
{
  app_helpers::fill_metrics_appconfig_in_yaml_schema(node, config.common_metrics_cfg);

  auto metrics_node            = node["metrics"];
  auto layers_node             = metrics_node["layers"];
  layers_node["enable_du_low"] = config.enable_du_low;
}

void srsran::fill_du_low_config_in_yaml_schema(YAML::Node& node, const du_low_unit_config& config)
{
  fill_du_low_metrics_section(node, config.metrics_cfg);
  fill_du_low_log_section(node["log"], config.loggers);
  fill_du_low_expert_execution_section(node["expert_execution"], config.expert_execution_cfg);
  fill_du_low_expert_section(node["expert_phy"], config.expert_phy_cfg);
  if (config.hal_config) {
    fill_du_low_hal_section(node["hal"], config.hal_config.value());
  }
}
