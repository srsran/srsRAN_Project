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

#include "o_cu_up_builder.h"
#include "apps/helpers/e2/e2_metric_connector_manager.h"
#include "apps/helpers/metrics/metrics_helpers.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "cu_up/cu_up_unit_config_translators.h"
#include "cu_up/metrics/cu_up_pdcp_metrics_consumers.h"
#include "cu_up/metrics/cu_up_pdcp_metrics_producer.h"
#include "e2/o_cu_up_e2_config_translators.h"
#include "o_cu_up_unit_config.h"
#include "srsran/cu_up/o_cu_up_factory.h"

using namespace srsran;

static pdcp_metrics_notifier* build_pdcp_metrics_config(std::vector<app_services::metrics_config>& cu_up_services_cfg,
                                                        app_services::metrics_notifier&            metrics_notifier,
                                                        bool                                       e2_enabled,
                                                        e2_cu_metrics_notifier&                    e2_notifier,
                                                        const cu_up_unit_metrics_config&           cu_up_metrics_cfg,
                                                        worker_manager&                            workers,
                                                        timer_manager&                             timers)
{
  pdcp_metrics_notifier* out = nullptr;

  if (!cu_up_metrics_cfg.layers_cfg.enable_pdcp) {
    return out;
  }

  auto metrics_generator                    = std::make_unique<cu_up_pdcp_metrics_producer_impl>(metrics_notifier);
  out                                       = &(*metrics_generator);
  app_services::metrics_config& metrics_cfg = cu_up_services_cfg.emplace_back();
  metrics_cfg.metric_name                   = cu_up_pdcp_metrics_properties_impl().name();
  metrics_cfg.callback                      = cu_up_pdcp_metrics_callback;
  metrics_cfg.producers.push_back(std::move(metrics_generator));

  const app_helpers::metrics_config& unit_metrics_cfg = cu_up_metrics_cfg.common_metrics_cfg;
  if (unit_metrics_cfg.enable_json_metrics) {
    metrics_cfg.consumers.push_back(
        std::make_unique<cu_up_pdcp_metrics_consumer_json>(srslog::fetch_basic_logger("APP"),
                                                           app_helpers::fetch_json_metrics_log_channel(),
                                                           workers.get_metrics_executor(),
                                                           timers.create_unique_timer(workers.get_metrics_executor()),
                                                           cu_up_metrics_cfg.cu_up_report_period));
  }
  if (unit_metrics_cfg.enable_log_metrics) {
    metrics_cfg.consumers.push_back(
        std::make_unique<cu_up_pdcp_metrics_consumer_log>(app_helpers::fetch_logger_metrics_log_channel()));
  }

  if (e2_enabled) {
    metrics_cfg.consumers.push_back(std::make_unique<cu_up_pdcp_metrics_consumer_e2>(e2_notifier));
  }

  return out;
}

o_cu_up_unit srsran::build_o_cu_up(const o_cu_up_unit_config& unit_cfg, const o_cu_up_unit_dependencies& dependencies)
{
  o_cu_up_unit              ocu_unit = {};
  srs_cu_up::o_cu_up_config config;
  config.cu_up_cfg     = generate_cu_up_config(unit_cfg.cu_up_cfg);
  config.cu_up_cfg.qos = generate_cu_up_qos_config(unit_cfg.cu_up_cfg);

  srs_cu_up::o_cu_up_dependencies ocu_up_dependencies;
  ocu_up_dependencies.cu_dependencies.exec_mapper    = &dependencies.workers->get_cu_up_executor_mapper();
  ocu_up_dependencies.cu_dependencies.e1_conn_client = dependencies.e1ap_conn_client;
  ocu_up_dependencies.cu_dependencies.f1u_gateway    = dependencies.f1u_gateway;
  ocu_up_dependencies.cu_dependencies.gtpu_pcap      = dependencies.gtpu_pcap;
  ocu_up_dependencies.cu_dependencies.timers         = dependencies.timers;

  // Create NG-U gateway(s).
  std::vector<std::unique_ptr<gtpu_gateway>> ngu_gws;
  if (not unit_cfg.cu_up_cfg.ngu_cfg.no_core) {
    for (const cu_up_unit_ngu_socket_config& sock_cfg : unit_cfg.cu_up_cfg.ngu_cfg.ngu_socket_cfg) {
      udp_network_gateway_config n3_udp_cfg = {};
      n3_udp_cfg.bind_address               = sock_cfg.bind_addr;
      n3_udp_cfg.bind_interface             = sock_cfg.bind_interface;
      n3_udp_cfg.ext_bind_addr              = sock_cfg.ext_addr;
      n3_udp_cfg.pool_occupancy_threshold   = sock_cfg.udp_config.pool_threshold;
      n3_udp_cfg.bind_port                  = GTPU_PORT;
      n3_udp_cfg.rx_max_mmsg                = sock_cfg.udp_config.rx_max_msgs;
      n3_udp_cfg.tx_qsize                   = sock_cfg.udp_config.tx_qsize;
      n3_udp_cfg.tx_max_mmsg                = sock_cfg.udp_config.tx_max_msgs;
      n3_udp_cfg.tx_max_segments            = sock_cfg.udp_config.tx_max_segments;
      n3_udp_cfg.pool_occupancy_threshold   = sock_cfg.udp_config.pool_threshold;
      n3_udp_cfg.reuse_addr                 = false; // TODO allow reuse_addr for multiple sockets
      n3_udp_cfg.dscp                       = sock_cfg.udp_config.dscp;

      std::unique_ptr<gtpu_gateway> ngu_gw =
          create_udp_gtpu_gateway(n3_udp_cfg,
                                  *dependencies.io_brk,
                                  dependencies.workers->get_cu_up_executor_mapper().io_ul_executor(),
                                  dependencies.workers->get_cu_up_executor_mapper().n3_rx_executor());
      ngu_gws.push_back(std::move(ngu_gw));
    }
  } else {
    std::unique_ptr<gtpu_gateway> ngu_gw = create_no_core_gtpu_gateway();
    ngu_gws.push_back(std::move(ngu_gw));
  }

  // Pass NG-U gateways to CU-UP
  ocu_up_dependencies.cu_dependencies.ngu_gws.resize(ngu_gws.size());
  for (size_t i = 0; i < ngu_gws.size(); i++) {
    ocu_up_dependencies.cu_dependencies.ngu_gws[i] = ngu_gws[i].get();
  }

  auto e2_metric_connectors = std::make_unique<e2_cu_metrics_connector_manager>();

  if (unit_cfg.e2_cfg.base_config.enable_unit_e2) {
    config.e2ap_cfg = generate_e2_config(
        unit_cfg.e2_cfg.base_config, unit_cfg.cu_up_cfg.gnb_id, config.cu_up_cfg.plmn, config.cu_up_cfg.cu_up_id);
    ocu_up_dependencies.e2_client          = dependencies.e2_gw;
    ocu_up_dependencies.e2_cu_metric_iface = &(*e2_metric_connectors).get_e2_metrics_interface(0);
  }
  auto pdcp_metric_notifier = build_pdcp_metrics_config(ocu_unit.metrics,
                                                        *dependencies.metrics_notifier,
                                                        unit_cfg.e2_cfg.base_config.enable_unit_e2,
                                                        e2_metric_connectors->get_e2_metric_notifier(0),
                                                        unit_cfg.cu_up_cfg.metrics,
                                                        *dependencies.workers,
                                                        *dependencies.timers);

  for (auto& qos_ : config.cu_up_cfg.qos) {
    qos_.second.pdcp_custom_cfg.metrics_notifier = pdcp_metric_notifier;
    if (!pdcp_metric_notifier) {
      qos_.second.pdcp_custom_cfg.metrics_period = std::chrono::milliseconds(0);
    }
  }

  ocu_unit.unit =
      std::make_unique<o_cu_up_unit_impl>(std::move(ngu_gws),
                                          std::move(e2_metric_connectors),
                                          srs_cu_up::create_o_cu_up(config, std::move(ocu_up_dependencies)));

  return ocu_unit;
}
