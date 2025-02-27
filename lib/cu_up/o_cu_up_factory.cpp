/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/cu_up/o_cu_up_factory.h"
#include "o_cu_up_impl.h"
#include "srsran/cu_up/cu_up_factory.h"
#include "srsran/cu_up/o_cu_up.h"
#include "srsran/e2/e2_cu_up_factory.h"

using namespace srsran;
using namespace srs_cu_up;

std::unique_ptr<o_cu_up> srs_cu_up::create_o_cu_up(const o_cu_up_config& config, o_cu_up_dependencies&& dependencies)
{
  auto cu_up = create_cu_up(config.cu_up_cfg, dependencies.cu_dependencies);

  // E2 is not enabled.
  if (dependencies.e2_client == nullptr) {
    return std::make_unique<o_cu_up_impl>(std::move(cu_up));
  }

  auto e2agent = create_e2_cu_up_agent(
      config.e2ap_cfg,
      *dependencies.e2_client,
      dependencies.e2_cu_metric_iface,
      nullptr,
      timer_factory{*dependencies.cu_dependencies.timers, dependencies.cu_dependencies.exec_mapper->ctrl_executor()},
      dependencies.cu_dependencies.exec_mapper->e2_executor());

  return std::make_unique<o_cu_up_with_e2_impl>(std::move(cu_up), std::move(e2agent));
}
