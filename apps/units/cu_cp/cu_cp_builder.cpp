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
#include "cu_cp_config_translators.h"
#include "srsran/cu_cp/cu_cp_factory.h"

using namespace srsran;

std::unique_ptr<srs_cu_cp::cu_cp> srsran::build_cu_cp(const cu_cp_unit_config&  cu_cp_unit_cfg,
                                                      cu_cp_build_dependencies& dependencies)
{
  srsran_assert(dependencies.cu_cp_executor, "Invalid CU-CP executor");
  srsran_assert(dependencies.cu_cp_e2_exec, "Invalid E2 executor");
  srsran_assert(dependencies.cu_cp_e2_exec, "Invalid E2 executor");

  srs_cu_cp::cu_cp_configuration cu_cp_cfg = generate_cu_cp_config(cu_cp_unit_cfg);
  cu_cp_cfg.cu_cp_executor                 = dependencies.cu_cp_executor;
  cu_cp_cfg.cu_cp_e2_exec                  = dependencies.cu_cp_e2_exec;
  cu_cp_cfg.ngap_notifier                  = dependencies.ngap_notifier;
  cu_cp_cfg.timers                         = dependencies.timers;

  return create_cu_cp(cu_cp_cfg);
}
