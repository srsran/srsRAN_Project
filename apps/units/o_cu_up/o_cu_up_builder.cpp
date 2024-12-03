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

#include "o_cu_up_builder.h"
#include "apps/services/e2/e2_metric_connector_manager.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "cu_up/cu_up_unit_config_translators.h"
#include "cu_up/metrics/cu_up_pdcp_metrics_consumers.h"
#include "cu_up/metrics/cu_up_pdcp_metrics_producer.h"
#include "e2/o_cu_up_e2_config_translators.h"
#include "o_cu_up_unit_config.h"
#include "srsran/cu_up/o_cu_up_factory.h"
#include "srsran/e2/e2_cu_metrics_connector.h"

using namespace srsran;

static pdcp_metrics_notifier* build_pdcp_metrics(std::vector<app_services::metrics_config>& cu_up_services_cfg,
                                                 app_services::metrics_notifier&            metrics_notifier,
                                                 bool                                       e2_enabled,
                                                 e2_cu_metrics_notifier&                    e2_notifier)
{
  pdcp_metrics_notifier* out = nullptr;

  // Do not instantiate the metrics if the E2 is not enabled.
  if (!e2_enabled) {
    return out;
  }

  auto metrics_generator                    = std::make_unique<pdcp_metrics_producer_impl>(metrics_notifier);
  out                                       = &(*metrics_generator);
  app_services::metrics_config& metrics_cfg = cu_up_services_cfg.emplace_back();
  metrics_cfg.metric_name                   = pdcp_metrics_properties_impl().name();
  metrics_cfg.callback                      = pdcp_metrics_callback;
  metrics_cfg.producers.push_back(std::move(metrics_generator));

  metrics_cfg.consumers.push_back(std::make_unique<pdcp_metrics_consumer_e2>(e2_notifier));

  return out;
}

o_cu_up_unit srsran::build_o_cu_up(const o_cu_up_unit_config& unit_cfg, const o_cu_up_unit_dependencies& dependencies)
{
  o_cu_up_unit              ocu_unit = {};
  srs_cu_up::o_cu_up_config config;
  config.cu_up_cfg     = generate_cu_up_config(unit_cfg.cu_up_cfg);
  config.cu_up_cfg.qos = generate_cu_up_qos_config(unit_cfg.cu_up_cfg);

  srs_cu_up::o_cu_up_dependencies ocu_up_dependencies;
  ocu_up_dependencies.cu_dependencies.exec_mapper    = dependencies.workers->cu_up_exec_mapper.get();
  ocu_up_dependencies.cu_dependencies.e1_conn_client = dependencies.e1ap_conn_client;
  ocu_up_dependencies.cu_dependencies.f1u_gateway    = dependencies.f1u_gateway;
  ocu_up_dependencies.cu_dependencies.gtpu_pcap      = dependencies.gtpu_pcap;
  ocu_up_dependencies.cu_dependencies.timers         = dependencies.timers;

  auto address = dependencies.f1u_gateway->get_cu_bind_address();
  srsran_assert(address.has_value(), "Invalid F1-U bind address");

  config.cu_up_cfg.net_cfg.f1u_bind_addr = address.value();
  // Create NG-U gateway.
  std::unique_ptr<srs_cu_up::ngu_gateway> ngu_gw;
  if (not unit_cfg.cu_up_cfg.upf_cfg.no_core) {
    udp_network_gateway_config ngu_gw_config = {};
    ngu_gw_config.bind_address               = config.cu_up_cfg.net_cfg.n3_bind_addr;
    ngu_gw_config.bind_port                  = config.cu_up_cfg.net_cfg.n3_bind_port;
    ngu_gw_config.bind_interface             = config.cu_up_cfg.net_cfg.n3_bind_interface;
    ngu_gw_config.rx_max_mmsg                = config.cu_up_cfg.net_cfg.n3_rx_max_mmsg;
    ngu_gw                                   = srs_cu_up::create_udp_ngu_gateway(
        ngu_gw_config, *dependencies.io_brk, dependencies.workers->cu_up_exec_mapper->io_ul_executor());
  } else {
    ngu_gw = srs_cu_up::create_no_core_ngu_gateway();
  }
  ocu_up_dependencies.cu_dependencies.ngu_gw = ngu_gw.get();

  auto e2_metric_connectors = std::make_unique<e2_cu_metrics_connector_manager>();

  if (unit_cfg.e2_cfg.base_config.enable_unit_e2) {
    config.e2ap_cfg                        = generate_e2_config(unit_cfg.e2_cfg.base_config);
    ocu_up_dependencies.e2_client          = dependencies.e2_gw;
    ocu_up_dependencies.e2_cu_metric_iface = &(*e2_metric_connectors).get_e2_metrics_interface(0);
  }
  auto pdcp_metric_notifier = build_pdcp_metrics(ocu_unit.metrics,
                                                 *dependencies.metrics_notifier,
                                                 unit_cfg.e2_cfg.base_config.enable_unit_e2,
                                                 e2_metric_connectors->get_e2_metric_notifier(0));

  for (auto& qos_ : config.cu_up_cfg.qos) {
    qos_.second.pdcp_custom_cfg.metrics_notifier = pdcp_metric_notifier;
    if (!pdcp_metric_notifier) { // Disable PDCP metrics if E2 is not enabled since there is no consumer.
      qos_.second.pdcp_custom_cfg.metrics_period = std::chrono::milliseconds(0);
    }
  }

  ocu_unit.unit =
      std::make_unique<o_cu_up_unit_impl>(std::move(ngu_gw),
                                          std::move(e2_metric_connectors),
                                          srs_cu_up::create_o_cu_up(config, std::move(ocu_up_dependencies)));

  return ocu_unit;
}