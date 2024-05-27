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
#include "srsran/cu_cp/cu_cp.h"

namespace srsran {

struct cu_cp_unit_config;
struct worker_manager;

/// CU-CP build dependencies.
struct cu_cp_build_dependencies {
  task_executor*                    cu_cp_executor = nullptr;
  task_executor*                    cu_cp_e2_exec  = nullptr;
  srs_cu_cp::ngap_message_notifier* ngap_notifier  = nullptr;
  timer_manager*                    timers         = nullptr;
};

/// Builds a CU-CP object with the given configuration.
std::unique_ptr<srs_cu_cp::cu_cp> build_cu_cp(const cu_cp_unit_config&  cu_cp_unit_cfg,
                                              cu_cp_build_dependencies& dependencies);

} // namespace srsran
