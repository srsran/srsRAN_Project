/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "apps/units/flexible_du/o_du_high/o_du_high_unit_factory.h"

namespace srsran {

struct worker_manager;

struct o_du_high_unit_factory_dependencies {
  worker_manager&                  workers;
  srs_du::f1c_connection_client&   f1c_client_handler;
  srs_du::f1u_du_gateway&          f1u_gw;
  timer_manager&                   timer_mng;
  mac_pcap&                        mac_p;
  rlc_pcap&                        rlc_p;
  e2_connection_client&            e2_client_handler;
  e2_du_metrics_connector_manager& e2_metric_connectors;
  srslog::sink&                    json_sink;
  app_services::metrics_notifier&  metrics_notifier;
  srs_du::o_du_high_dependencies   o_du_hi_dependencies;
};

/// Creates one O-DU high per cell.
std::vector<o_du_high_unit> make_multicell_with_multi_du(const o_du_high_unit_config&          o_du_high_unit_cfg,
                                                         o_du_high_unit_factory_dependencies&& dependencies);

} // namespace srsran
