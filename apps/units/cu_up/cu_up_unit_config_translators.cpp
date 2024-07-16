/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "cu_up_unit_config.h"
#include "srsran/cu_up/cu_up_configuration_helpers.h"
#include "srsran/rlc/rlc_config.h"

using namespace srsran;

srs_cu_up::cu_up_configuration srsran::generate_cu_up_config(const cu_up_unit_config& config)
{
  srs_cu_up::cu_up_configuration out_cfg;
  out_cfg.statistics_report_period     = std::chrono::seconds{config.metrics.cu_up_statistics_report_period};
  out_cfg.n3_cfg.gtpu_reordering_timer = std::chrono::milliseconds{config.gtpu_reordering_timer_ms};
  out_cfg.n3_cfg.warn_on_drop          = config.warn_on_drop;

  if (config.upf_cfg.n3_bind_addr == "auto") {
    out_cfg.net_cfg.n3_bind_addr = config.upf_cfg.bind_addr;
  } else {
    out_cfg.net_cfg.n3_bind_addr = config.upf_cfg.n3_bind_addr;
  }
  out_cfg.net_cfg.n3_ext_addr       = config.upf_cfg.n3_ext_addr;
  out_cfg.net_cfg.n3_bind_interface = config.upf_cfg.n3_bind_interface;
  out_cfg.net_cfg.n3_rx_max_mmsg    = config.upf_cfg.udp_rx_max_msgs;

  out_cfg.test_mode_cfg.enabled           = config.test_mode_cfg.enabled;
  out_cfg.test_mode_cfg.integrity_enabled = config.test_mode_cfg.integrity_enabled;
  return out_cfg;
}

std::map<five_qi_t, srs_cu_up::cu_up_qos_config>
srsran::generate_cu_up_qos_config(const cu_up_unit_config& cu_up_config)
{
  std::map<five_qi_t, srs_cu_up::cu_up_qos_config> out_cfg = {};
  if (cu_up_config.qos_cfg.empty()) {
    out_cfg =
        config_helpers::make_default_cu_up_qos_config_list(cu_up_config.warn_on_drop,
                                                           timer_duration(cu_up_config.metrics.pdcp.report_period),
                                                           cu_up_config.test_mode_cfg.enabled);
    return out_cfg;
  }

  for (const auto& qos : cu_up_config.qos_cfg) {
    if (out_cfg.find(qos.five_qi) != out_cfg.end()) {
      report_error("Duplicate 5QI configuration: {}\n", qos.five_qi);
    }

    // Convert PDCP custom config
    pdcp_custom_config& out_pdcp_custom = out_cfg[qos.five_qi].pdcp_custom_cfg;
    out_pdcp_custom.tx.warn_on_drop     = cu_up_config.warn_on_drop;
    out_pdcp_custom.tx.rlc_sdu_queue    = qos.rlc_sdu_queue;
    out_pdcp_custom.tx.test_mode        = cu_up_config.test_mode_cfg.enabled;
    out_pdcp_custom.metrics_period      = timer_duration(cu_up_config.metrics.pdcp.report_period);

    // Convert F1-U config
    srs_cu_up::f1u_config& f1u_cfg = out_cfg[qos.five_qi].f1u_cfg;
    f1u_cfg.warn_on_drop           = cu_up_config.warn_on_drop;
  }
  return out_cfg;
}
