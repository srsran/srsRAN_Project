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
#include "apps/services/worker_manager.h"
#include "cu_cp_commands.h"
#include "cu_cp_config_translators.h"
#include "srsran/cu_cp/cu_cp_factory.h"

using namespace srsran;

cu_cp_unit srsran::build_cu_cp(const cu_cp_unit_config& cu_cp_unit_cfg, cu_cp_build_dependencies& dependencies)
{
  srsran_assert(dependencies.cu_cp_executor, "Invalid CU-CP executor");
  srsran_assert(dependencies.cu_cp_e2_exec, "Invalid E2 executor");
  srsran_assert(dependencies.cu_cp_e2_exec, "Invalid E2 executor");

  srs_cu_cp::cu_cp_configuration cu_cp_cfg = generate_cu_cp_config(cu_cp_unit_cfg);
  cu_cp_cfg.services.cu_cp_executor        = dependencies.cu_cp_executor;
  cu_cp_cfg.services.cu_cp_e2_exec         = dependencies.cu_cp_e2_exec;
  cu_cp_cfg.services.n2_gw                 = dependencies.n2_client;
  cu_cp_cfg.services.timers                = dependencies.timers;

  cu_cp_unit cu_cmd_wrapper;
  cu_cmd_wrapper.unit = create_cu_cp(cu_cp_cfg);

  // Add the commands;
  cu_cmd_wrapper.commands.push_back(std::make_unique<handover_app_command>(cu_cmd_wrapper.unit->get_command_handler()));

  return cu_cmd_wrapper;
}
