/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_builder.h"
#include "apps/services/e2/e2_metric_connector_manager.h"
#include "apps/services/worker_manager.h"
#include "apps/units/cu_up/metrics/cu_up_pdcp_metrics_consumers.h"
#include "apps/units/cu_up/metrics/cu_up_pdcp_metrics_producer.h"
#include "cu_up_unit_config.h"
#include "cu_up_unit_config_translators.h"
#include "srsran/cu_up/cu_up_factory.h"
#include "srsran/e2/e2_cu_metrics_connector.h"

using namespace srsran;

using e2_cu_metrics_connector_manager =
    e2_metric_connector_manager<e2_cu_metrics_connector, e2_cu_metrics_notifier, e2_cu_metrics_interface>;

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

cu_up_unit srsran::build_cu_up(const cu_up_unit_config& unit_cfg, const cu_up_unit_dependencies& dependencies)
{
  cu_up_unit                     wrapper = {};
  srs_cu_up::cu_up_configuration config  = generate_cu_up_config(unit_cfg);
  config.exec_mapper                     = dependencies.workers->cu_up_exec_mapper.get();
  config.e1ap.e1_conn_client             = dependencies.e1ap_conn_client;
  config.f1u_gateway                     = dependencies.f1u_gateway;
  config.gtpu_pcap                       = dependencies.gtpu_pcap;
  config.timers                          = dependencies.timers;
  config.qos                             = generate_cu_up_qos_config(unit_cfg);

  auto address = config.f1u_gateway->get_cu_bind_address();
  srsran_assert(address.has_value(), "Invalid F1-U bind address");

  config.net_cfg.f1u_bind_addr = address.value();
  // Create NG-U gateway.
  std::unique_ptr<srs_cu_up::ngu_gateway> ngu_gw;
  if (not unit_cfg.upf_cfg.no_core) {
    udp_network_gateway_config ngu_gw_config = {};
    ngu_gw_config.bind_address               = config.net_cfg.n3_bind_addr;
    ngu_gw_config.bind_port                  = config.net_cfg.n3_bind_port;
    ngu_gw_config.bind_interface             = config.net_cfg.n3_bind_interface;
    ngu_gw_config.rx_max_mmsg                = config.net_cfg.n3_rx_max_mmsg;
    ngu_gw                                   = srs_cu_up::create_udp_ngu_gateway(
        ngu_gw_config, *dependencies.io_brk, dependencies.workers->cu_up_exec_mapper->io_ul_executor());
  } else {
    ngu_gw = srs_cu_up::create_no_core_ngu_gateway();
  }
  config.ngu_gw = ngu_gw.get();

  auto e2_metric_connectors = std::make_unique<e2_cu_metrics_connector_manager>();

  if (unit_cfg.e2_cfg.enable_unit_e2) {
    config.e2_client          = dependencies.e2_gw;
    config.e2ap_config        = generate_e2_config(unit_cfg);
    config.e2_cu_metric_iface = &(*e2_metric_connectors).get_e2_metrics_interface(0);
  }
  auto pdcp_metric_notifier = build_pdcp_metrics(wrapper.metrics,
                                                 *dependencies.metrics_notifier,
                                                 unit_cfg.e2_cfg.enable_unit_e2,
                                                 e2_metric_connectors->get_e2_metric_notifier(0));

  for (auto& qos_ : config.qos) {
    qos_.second.pdcp_custom_cfg.metrics_notifier = pdcp_metric_notifier;
  }

  wrapper.unit =
      std::make_unique<cu_up_wrapper>(std::move(ngu_gw), std::move(e2_metric_connectors), create_cu_up(config));

  return wrapper;
}
