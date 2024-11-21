/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "o_cu_cp_builder.h"
#include "apps/services/metrics/metrics_config.h"
#include "cu_cp/cu_cp_commands.h"
#include "cu_cp/cu_cp_config_translators.h"
#include "e2/o_cu_cp_e2_config_translators.h"
#include "o_cu_cp_unit_config.h"
#include "o_cu_cp_unit_impl.h"
#include "srsran/cu_cp/o_cu_cp_config.h"
#include "srsran/cu_cp/o_cu_cp_factory.h"
#include "srsran/e2/e2_cu_metrics_connector.h"

using namespace srsran;

o_cu_cp_unit srsran::build_o_cu_cp(const o_cu_cp_unit_config& unit_cfg, o_cu_cp_unit_dependencies& dependencies)
{
  srsran_assert(dependencies.cu_cp_executor, "Invalid CU-CP executor");
  srsran_assert(dependencies.cu_cp_e2_exec, "Invalid E2 executor");
  srsran_assert(dependencies.ngap_pcap, "Invalid NGAP PCAP");
  srsran_assert(dependencies.broker, "Invalid IO broker");
  srsran_assert(dependencies.metrics_notifier, "Invalid metrics notifier");

  srs_cu_cp::o_cu_cp_config       o_cu_cp_cfg;
  const cu_cp_unit_config&        cucp_unit_cfg = unit_cfg.cucp_cfg;
  srs_cu_cp::cu_cp_configuration& cu_cp_cfg     = o_cu_cp_cfg.cu_cp_config;
  cu_cp_cfg                                     = generate_cu_cp_config(cucp_unit_cfg);
  cu_cp_cfg.services.cu_cp_executor             = dependencies.cu_cp_executor;
  cu_cp_cfg.services.cu_cp_e2_exec              = dependencies.cu_cp_e2_exec;
  cu_cp_cfg.services.timers                     = dependencies.timers;

  // Create N2 Client Gateways.
  std::vector<std::unique_ptr<srs_cu_cp::n2_connection_client>> n2_clients;

  n2_clients.push_back(srs_cu_cp::create_n2_connection_client(generate_n2_client_config(
      cucp_unit_cfg.amf_config.no_core, cucp_unit_cfg.amf_config.amf, *dependencies.ngap_pcap, *dependencies.broker)));

  for (const auto& amf : cucp_unit_cfg.extra_amfs) {
    n2_clients.push_back(srs_cu_cp::create_n2_connection_client(generate_n2_client_config(
        cucp_unit_cfg.amf_config.no_core, amf, *dependencies.ngap_pcap, *dependencies.broker)));
  }

  for (unsigned pos = 0; pos < n2_clients.size(); pos++) {
    cu_cp_cfg.ngaps[pos].n2_gw = n2_clients[pos].get();
  }
  auto e2_metric_connectors = std::make_unique<e2_cu_metrics_connector_manager>();

  srs_cu_cp::o_cu_cp_dependencies ocu_dependencies;
  if (unit_cfg.e2_cfg.base_config.enable_unit_e2) {
    o_cu_cp_cfg.e2ap_config             = generate_e2_config(unit_cfg.e2_cfg, cucp_unit_cfg.gnb_id);
    ocu_dependencies.e2_client          = dependencies.e2_gw;
    ocu_dependencies.e2_cu_metric_iface = &(*e2_metric_connectors).get_e2_metrics_interface(0);
  }

  o_cu_cp_unit ocucp;
  ocucp.unit = std::make_unique<o_cu_cp_unit_impl>(
      std::move(n2_clients), std::move(e2_metric_connectors), srs_cu_cp::create_o_cu_cp(o_cu_cp_cfg, ocu_dependencies));

  // Add the commands;
  ocucp.commands.push_back(std::make_unique<handover_app_command>(ocucp.unit->get_cu_cp().get_command_handler()));

  return ocucp;
}
