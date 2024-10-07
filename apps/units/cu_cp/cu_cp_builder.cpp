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

#include "cu_cp_builder.h"
#include "cu_cp_commands.h"
#include "cu_cp_config_translators.h"
#include "cu_cp_unit_config.h"
#include "cu_cp_wrapper.h"
#include "srsran/cu_cp/cu_cp_factory.h"

using namespace srsran;

cu_cp_unit srsran::build_cu_cp(const cu_cp_unit_config& cu_cp_unit_cfg, cu_cp_build_dependencies& dependencies)
{
  srsran_assert(dependencies.cu_cp_executor, "Invalid CU-CP executor");
  srsran_assert(dependencies.cu_cp_e2_exec, "Invalid E2 executor");
  srsran_assert(dependencies.cu_cp_e2_exec, "Invalid E2 executor");
  srsran_assert(dependencies.ngap_pcap, "Invalid NGAP PCAP");
  srsran_assert(dependencies.broker, "Invalid IO broker");

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

  cu_cp_unit cu_cmd_wrapper;
  cu_cmd_wrapper.unit = std::make_unique<cu_cp_wrapper>(std::move(n2_clients), create_cu_cp(cu_cp_cfg));

  // Add the commands;
  cu_cmd_wrapper.commands.push_back(std::make_unique<handover_app_command>(cu_cmd_wrapper.unit->get_command_handler()));

  return cu_cmd_wrapper;
}
