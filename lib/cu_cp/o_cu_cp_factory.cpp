/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/cu_cp/o_cu_cp_factory.h"
#include "o_cu_cp_impl.h"
#include "srsran/cu_cp/cu_cp_factory.h"
#include "srsran/cu_cp/o_cu_cp_config.h"
#include "srsran/e2/e2_cu_factory.h"

using namespace srsran;
using namespace srs_cu_cp;

std::unique_ptr<o_cu_cp> srs_cu_cp::create_o_cu_cp(const o_cu_cp_config&       config,
                                                   const o_cu_cp_dependencies& dependencies)
{
  auto cu = create_cu_cp(config.cu_cp_config);

  if (dependencies.e2_client == nullptr) {
    return std::make_unique<o_cu_cp_impl>(std::move(cu));
  }

  auto e2agent = create_e2_cu_agent(
      config.e2ap_config,
      *dependencies.e2_client,
      dependencies.e2_cu_metric_iface,
      &cu.get()->get_cu_configurator(),
      timer_factory{*config.cu_cp_config.services.timers, *config.cu_cp_config.services.cu_cp_executor},
      *config.cu_cp_config.services.cu_cp_executor);

  return std::make_unique<o_cu_cp_with_e2_impl>(std::move(e2agent), std::move(cu));
}