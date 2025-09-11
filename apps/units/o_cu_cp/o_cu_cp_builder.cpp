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

#include "o_cu_cp_builder.h"
#include "apps/helpers/metrics/metrics_helpers.h"
#include "apps/services/metrics/metrics_config.h"
#include "apps/units/o_cu_cp/cu_cp/metrics/cu_cp_metrics.h"
#include "apps/units/o_cu_cp/cu_cp/metrics/cu_cp_metrics_consumers.h"
#include "apps/units/o_cu_cp/cu_cp/metrics/cu_cp_metrics_producer.h"
#include "cu_cp/cu_cp_cmdline_commands.h"
#include "cu_cp/cu_cp_config_translators.h"
#include "e2/o_cu_cp_e2_config_translators.h"
#include "o_cu_cp_unit_config.h"
#include "o_cu_cp_unit_impl.h"
#include "srsran/cu_cp/cu_cp_executor_mapper.h"
#include "srsran/cu_cp/o_cu_cp_config.h"
#include "srsran/cu_cp/o_cu_cp_factory.h"
#include "srsran/e2/e2_cu_metrics_connector.h"

using namespace srsran;

static cu_cp_metrics_report_notifier*
build_cu_cp_metrics_config(std::vector<app_services::metrics_config>& cu_cp_services_cfg,
                           app_services::metrics_notifier&            metrics_notifier,
                           const cu_cp_unit_metrics_config&           cu_cp_metrics_cfg)
{
  cu_cp_metrics_report_notifier* out = nullptr;

  auto metrics_generator                            = std::make_unique<cu_cp_metrics_producer_impl>(metrics_notifier);
  out                                               = &(*metrics_generator);
  app_services::metrics_config& metrics_service_cfg = cu_cp_services_cfg.emplace_back();
  metrics_service_cfg.metric_name                   = cu_cp_metrics_properties_impl().name();
  metrics_service_cfg.callback                      = cu_cp_metrics_callback;
  metrics_service_cfg.producers.push_back(std::move(metrics_generator));

  const app_helpers::metrics_config& unit_metrics_cfg = cu_cp_metrics_cfg.common_metrics_cfg;
  if (unit_metrics_cfg.enable_json_metrics) {
    metrics_service_cfg.consumers.push_back(
        std::make_unique<cu_cp_metrics_consumer_json>(app_helpers::fetch_json_metrics_log_channel()));
  }
  if (cu_cp_metrics_cfg.common_metrics_cfg.enable_log_metrics) {
    metrics_service_cfg.consumers.push_back(
        std::make_unique<cu_cp_metrics_consumer_log>(app_helpers::fetch_logger_metrics_log_channel()));
  }

  return out;
}

o_cu_cp_unit srsran::build_o_cu_cp(const o_cu_cp_unit_config& unit_cfg, o_cu_cp_unit_dependencies& dependencies)
{
  srsran_assert(dependencies.executor_mapper, "Invalid CU-CP executor mapper");
  srsran_assert(dependencies.ngap_pcap, "Invalid NGAP PCAP");
  srsran_assert(dependencies.broker, "Invalid IO broker");
  srsran_assert(dependencies.metrics_notifier, "Invalid metrics notifier");

  srs_cu_cp::o_cu_cp_config       o_cu_cp_cfg;
  const cu_cp_unit_config&        cucp_unit_cfg = unit_cfg.cucp_cfg;
  srs_cu_cp::cu_cp_configuration& cu_cp_cfg     = o_cu_cp_cfg.cu_cp_config;
  cu_cp_cfg                                     = generate_cu_cp_config(cucp_unit_cfg);
  cu_cp_cfg.services.cu_cp_executor             = &dependencies.executor_mapper->ctrl_executor();
  cu_cp_cfg.services.cu_cp_e2_exec              = &dependencies.executor_mapper->e2_executor();
  cu_cp_cfg.services.timers                     = dependencies.timers;

  o_cu_cp_unit ocucp;

  cu_cp_cfg.metrics.metrics_report_period = std::chrono::milliseconds(unit_cfg.cucp_cfg.metrics.cu_cp_report_period);
  cu_cp_cfg.metrics_notifier =
      build_cu_cp_metrics_config(ocucp.metrics, *dependencies.metrics_notifier, unit_cfg.cucp_cfg.metrics);

  // Create N2 Client Gateways.
  std::vector<std::unique_ptr<srs_cu_cp::n2_connection_client>> n2_clients;

  n2_clients.push_back(srs_cu_cp::create_n2_connection_client(
      generate_n2_client_config(cucp_unit_cfg.amf_config.no_core,
                                cucp_unit_cfg.amf_config.amf,
                                *dependencies.ngap_pcap,
                                *dependencies.broker,
                                dependencies.executor_mapper->n2_rx_executor())));

  for (const auto& amf : cucp_unit_cfg.extra_amfs) {
    n2_clients.push_back(srs_cu_cp::create_n2_connection_client(
        generate_n2_client_config(cucp_unit_cfg.amf_config.no_core,
                                  amf,
                                  *dependencies.ngap_pcap,
                                  *dependencies.broker,
                                  dependencies.executor_mapper->n2_rx_executor())));
  }

  for (unsigned i = 0, e = n2_clients.size(); i != e; ++i) {
    cu_cp_cfg.ngap.ngaps[i].n2_gw = n2_clients[i].get();
  }
  auto e2_metric_connectors = std::make_unique<e2_cu_metrics_connector_manager>();

  srs_cu_cp::o_cu_cp_dependencies ocu_dependencies;
  if (unit_cfg.e2_cfg.base_config.enable_unit_e2) {
    o_cu_cp_cfg.e2ap_config             = generate_e2_config(unit_cfg.e2_cfg, cucp_unit_cfg.gnb_id);
    ocu_dependencies.e2_client          = dependencies.e2_gw;
    ocu_dependencies.e2_cu_metric_iface = &(*e2_metric_connectors).get_e2_metrics_interface(0);
  }

  ocucp.unit = std::make_unique<o_cu_cp_unit_impl>(
      std::move(n2_clients), std::move(e2_metric_connectors), srs_cu_cp::create_o_cu_cp(o_cu_cp_cfg, ocu_dependencies));

  // Add the commands;
  ocucp.commands.cmdline.commands.push_back(
      std::make_unique<handover_app_command>(ocucp.unit->get_cu_cp().get_command_handler()));

  return ocucp;
}
