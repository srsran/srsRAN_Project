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

#include "cu_up_unit_config_translators.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "cu_up_unit_config.h"
#include "srsran/cu_up/cu_up_configuration_helpers.h"

using namespace srsran;

srs_cu_up::cu_up_config srsran::generate_cu_up_config(const cu_up_unit_config& config)
{
  srs_cu_up::cu_up_config out_cfg;
  out_cfg.gnb_id     = config.gnb_id;
  out_cfg.cu_up_id   = config.gnb_cu_up_id;
  out_cfg.cu_up_name = fmt::format("srs_cu_up_{}", fmt::underlying(config.gnb_cu_up_id));

  out_cfg.statistics_report_period = std::chrono::seconds{config.metrics.cu_up_report_period};

  out_cfg.n3_cfg.gtpu_reordering_timer = std::chrono::milliseconds{config.ngu_cfg.gtpu_cfg.gtpu_reordering_timer_ms};
  out_cfg.n3_cfg.gtpu_rate_limiting_period = config.ngu_cfg.gtpu_cfg.rate_limiter_period;
  out_cfg.n3_cfg.gtpu_ignore_ue_ambr       = config.ngu_cfg.gtpu_cfg.ignore_ue_ambr;
  out_cfg.n3_cfg.gtpu_queue_size           = config.ngu_cfg.gtpu_cfg.gtpu_queue_size;
  out_cfg.n3_cfg.gtpu_batch_size           = config.ngu_cfg.gtpu_cfg.gtpu_batch_size;
  out_cfg.n3_cfg.warn_on_drop              = config.warn_on_drop;

  // E1AP-CU-UP config.
  // JSON metrics are not supported at E1AP for now, so only enable if log metrics are enabled.
  out_cfg.e1ap.json_log_enabled = config.loggers.e1ap_json_enabled;
  out_cfg.e1ap.metrics_period =
      timer_duration{config.metrics.layers_cfg.enable_e1ap && config.metrics.common_metrics_cfg.enable_log_metrics
                         ? config.metrics.cu_up_report_period
                         : 0};

  out_cfg.test_mode_cfg.enabled              = config.test_mode_cfg.enabled;
  out_cfg.test_mode_cfg.integrity_enabled    = config.test_mode_cfg.integrity_enabled;
  out_cfg.test_mode_cfg.ciphering_enabled    = config.test_mode_cfg.ciphering_enabled;
  out_cfg.test_mode_cfg.nea_algo             = config.test_mode_cfg.nea_algo;
  out_cfg.test_mode_cfg.nia_algo             = config.test_mode_cfg.nia_algo;
  out_cfg.test_mode_cfg.ue_ambr              = config.test_mode_cfg.ue_ambr;
  out_cfg.test_mode_cfg.attach_detach_period = config.test_mode_cfg.attach_detach_period;
  return out_cfg;
}

std::map<five_qi_t, srs_cu_up::cu_up_qos_config>
srsran::generate_cu_up_qos_config(const cu_up_unit_config& cu_up_config)
{
  std::map<five_qi_t, srs_cu_up::cu_up_qos_config> out_cfg = {};
  if (cu_up_config.qos_cfg.empty()) {
    out_cfg = config_helpers::make_default_cu_up_qos_config_list(
        cu_up_config.warn_on_drop,
        timer_duration(cu_up_config.metrics.layers_cfg.enable_pdcp ? cu_up_config.metrics.cu_up_report_period : 0),
        cu_up_config.test_mode_cfg.enabled);
    return out_cfg;
  }

  for (const cu_up_unit_qos_config& qos : cu_up_config.qos_cfg) {
    if (out_cfg.find(qos.five_qi) != out_cfg.end()) {
      report_error("Duplicate 5QI configuration: {}\n", qos.five_qi);
    }

    // Convert PDCP custom config
    pdcp_custom_config& out_pdcp_custom = out_cfg[qos.five_qi].pdcp_custom_cfg;
    out_pdcp_custom.metrics_period =
        timer_duration(cu_up_config.metrics.layers_cfg.enable_pdcp ? cu_up_config.metrics.cu_up_report_period : 0);
    out_pdcp_custom.tx.warn_on_drop = cu_up_config.warn_on_drop;
    out_pdcp_custom.tx.test_mode    = cu_up_config.test_mode_cfg.enabled;

    // Convert F1-U config
    srs_cu_up::f1u_config& f1u_cfg = out_cfg[qos.five_qi].f1u_cfg;
    f1u_cfg.warn_on_drop           = cu_up_config.warn_on_drop;
    f1u_cfg.dl_t_notif_timer       = std::chrono::milliseconds(qos.f1u_cu_up.t_notify);
    f1u_cfg.queue_size             = qos.f1u_cu_up.queue_size;
    f1u_cfg.batch_size             = qos.f1u_cu_up.batch_size;
  }
  return out_cfg;
}

void srsran::fill_cu_up_worker_manager_config(worker_manager_config& config, const cu_up_unit_config& unit_cfg)
{
  config.cu_up_cfg = worker_manager_config::cu_up_config{};

  config.cu_up_cfg->dl_ue_executor_queue_size   = unit_cfg.exec_cfg.dl_ue_executor_queue_size;
  config.cu_up_cfg->ul_ue_executor_queue_size   = unit_cfg.exec_cfg.ul_ue_executor_queue_size;
  config.cu_up_cfg->ctrl_ue_executor_queue_size = unit_cfg.exec_cfg.ctrl_ue_executor_queue_size;
  config.cu_up_cfg->strand_batch_size           = unit_cfg.exec_cfg.strand_batch_size;
  config.cu_up_cfg->executor_tracing_enable     = unit_cfg.exec_cfg.executor_tracing_enable;
  config.cu_up_cfg->skip_cu_up_executor         = unit_cfg.metrics.layers_cfg.skip_cu_up_executor;

  auto& pcap_cfg = config.pcap_cfg;
  if (unit_cfg.pcap_cfg.e1ap.enabled) {
    pcap_cfg.is_e1ap_enabled = true;
  }
  if (unit_cfg.pcap_cfg.n3.enabled) {
    pcap_cfg.is_n3_enabled = true;
  }
  if (unit_cfg.pcap_cfg.f1u.enabled) {
    pcap_cfg.is_f1u_enabled = true;
  }
}
