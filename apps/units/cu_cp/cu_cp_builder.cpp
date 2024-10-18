/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_builder.h"
#include "apps/services/metrics/metrics_config.h"
#include "apps/units/cu_cp/cu_cp_unit_config.h"
#include "apps/units/cu_cp/metrics/cu_cp_pdcp_metrics_consumers.h"
#include "apps/units/cu_cp/metrics/cu_cp_pdcp_metrics_producer.h"
#include "cu_cp_commands.h"
#include "cu_cp_config_translators.h"
#include "cu_cp_unit_config.h"
#include "cu_cp_wrapper.h"
#include "srsran/cu_cp/cu_cp_factory.h"
#include "srsran/e2/e2_cu_metrics_connector.h"

using namespace srsran;

static pdcp_metrics_notifier* build_pdcp_metrics(std::vector<app_services::metrics_config>& cu_cp_services_cfg,
                                                 app_services::metrics_notifier&            metrics_notifier,
                                                 const cu_cp_unit_config&                   cu_cp_unit_cfg,
                                                 e2_cu_metrics_notifier&                    e2_notifier)
{
  pdcp_metrics_notifier* out = nullptr;

  // Do not instantiate the metrics if the E2 is not enabled.
  if (!cu_cp_unit_cfg.e2_cfg.enable_unit_e2) {
    return out;
  }

  auto metrics_generator                    = std::make_unique<pdcp_metrics_producer_impl>(metrics_notifier);
  out                                       = &(*metrics_generator);
  app_services::metrics_config& metrics_cfg = cu_cp_services_cfg.emplace_back();
  metrics_cfg.metric_name                   = pdcp_metrics_properties_impl().name();
  metrics_cfg.callback                      = pdcp_metrics_callback;
  metrics_cfg.producers.push_back(std::move(metrics_generator));

  metrics_cfg.consumers.push_back(std::make_unique<pdcp_metrics_consumer_e2>(e2_notifier));

  return out;
}

cu_cp_unit srsran::build_cu_cp(const cu_cp_unit_config& cu_cp_unit_cfg, cu_cp_build_dependencies& dependencies)
{
  srsran_assert(dependencies.cu_cp_executor, "Invalid CU-CP executor");
  srsran_assert(dependencies.cu_cp_e2_exec, "Invalid E2 executor");
  srsran_assert(dependencies.ngap_pcap, "Invalid NGAP PCAP");
  srsran_assert(dependencies.broker, "Invalid IO broker");
  srsran_assert(dependencies.metrics_notifier, "Invalid metrics notifier");

  srs_cu_cp::cu_cp_configuration cu_cp_cfg = generate_cu_cp_config(cu_cp_unit_cfg);
  cu_cp_cfg.services.cu_cp_executor        = dependencies.cu_cp_executor;
  cu_cp_cfg.services.cu_cp_e2_exec         = dependencies.cu_cp_e2_exec;
  cu_cp_cfg.services.timers                = dependencies.timers;

  // Create N2 Client Gateways.
  std::vector<std::unique_ptr<srs_cu_cp::n2_connection_client>> n2_clients;
  n2_clients.push_back(
      srs_cu_cp::create_n2_connection_client(generate_n2_client_config(cu_cp_unit_cfg.amf_config.no_core,
                                                                       cu_cp_unit_cfg.amf_config.amf,
                                                                       *dependencies.ngap_pcap,
                                                                       *dependencies.broker)));

  for (const auto& amf : cu_cp_unit_cfg.extra_amfs) {
    n2_clients.push_back(srs_cu_cp::create_n2_connection_client(generate_n2_client_config(
        cu_cp_unit_cfg.amf_config.no_core, amf, *dependencies.ngap_pcap, *dependencies.broker)));
  }

  for (unsigned pos = 0; pos < n2_clients.size(); pos++) {
    cu_cp_cfg.ngaps[pos].n2_gw = n2_clients[pos].get();
  }
  auto e2_metric_connectors = std::make_unique<e2_cu_metrics_connector_manager>();

  if (cu_cp_unit_cfg.e2_cfg.enable_unit_e2) {
    cu_cp_cfg.e2_client          = dependencies.e2_gw;
    cu_cp_cfg.e2ap_config        = generate_e2_config(cu_cp_unit_cfg);
    cu_cp_cfg.e2_cu_metric_iface = &(*e2_metric_connectors).get_e2_metrics_interface(0);
  }

  cu_cp_unit cu_cmd_wrapper;
  cu_cp_cfg.pdcp_metric_notifier = build_pdcp_metrics(cu_cmd_wrapper.metrics,
                                                      *dependencies.metrics_notifier,
                                                      cu_cp_unit_cfg,
                                                      e2_metric_connectors->get_e2_metric_notifier(0));

  cu_cmd_wrapper.unit =
      std::make_unique<cu_cp_wrapper>(std::move(n2_clients), std::move(e2_metric_connectors), create_cu_cp(cu_cp_cfg));

  // Add the commands;
  cu_cmd_wrapper.commands.push_back(std::make_unique<handover_app_command>(cu_cmd_wrapper.unit->get_command_handler()));

  return cu_cmd_wrapper;
}
