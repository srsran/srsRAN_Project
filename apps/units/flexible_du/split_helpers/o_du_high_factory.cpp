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

#include "o_du_high_factory.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/units/flexible_du/o_du_high/du_high/du_high_config_translators.h"
#include "apps/units/flexible_du/o_du_high/o_du_high_unit_config.h"

using namespace srsran;

/// \brief Update the Flexible DU metrics configuration with the given local DU configuration and E2 configuration.
///
/// This function manages the multi cell workaround for the DU high metrics. To have multi cell, now one DU is
/// instantiated per cell, so this would create multiple consumers that does not make sense, for example stdout.
/// With this we avoid having 2 different objects that write in the stdout.
static void update_du_metrics(std::vector<app_services::metrics_config>& flexible_du_cfg,
                              std::vector<app_services::metrics_config>  local_du_cfg,
                              bool                                       is_e2_enabled)
{
  // First call, copy everything.
  if (flexible_du_cfg.empty()) {
    flexible_du_cfg = std::move(local_du_cfg);
    return;
  }

  // Safe check that all the DUs provides the same amount of metrics.
  srsran_assert(flexible_du_cfg.size() == local_du_cfg.size(),
                "Flexible DU metrics size '{}' does not match DU metrics size '{}'",
                flexible_du_cfg.size(),
                local_du_cfg.size());

  // Iterate the metrics configs of each DU. Each DU should ha
  for (unsigned i = 0, e = local_du_cfg.size(); i != e; ++i) {
    // Store the metrics producers for each DU.
    flexible_du_cfg[i].producers.push_back(std::move(local_du_cfg[i].producers.back()));

    // Move E2 consumers for each DU to the common output config. E2 Consumers occupy the last position.
    if (is_e2_enabled) {
      flexible_du_cfg[i].consumers.push_back(std::move(local_du_cfg[i].consumers.back()));
    }
  }
}

std::vector<o_du_high_unit> srsran::make_multicell_with_multi_du(const o_du_high_unit_config& o_du_high_unit_cfg,
                                                                 o_du_high_unit_factory_dependencies&& dependencies)
{
  const du_high_unit_config& du_hi = o_du_high_unit_cfg.du_high_cfg.config;

  auto du_cells = generate_du_cell_config(du_hi);

  std::vector<o_du_high_unit> du_insts;

  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    // Create one DU per cell.
    o_du_high_unit_config tmp_cfg;
    tmp_cfg.du_high_cfg.config = du_hi;
    tmp_cfg.du_high_cfg.config.cells_cfg.resize(1);
    tmp_cfg.du_high_cfg.config.cells_cfg[0] = du_hi.cells_cfg[i];
    tmp_cfg.fapi_cfg                        = o_du_high_unit_cfg.fapi_cfg;
    srs_du::o_du_high_dependencies sector_deps;
    sector_deps.sectors.push_back(dependencies.o_du_hi_dependencies.sectors[i]);

    o_du_high_unit_params       odu_hi_unit_params       = {tmp_cfg, i};
    o_du_high_unit_dependencies odu_hi_unit_dependencies = {dependencies.workers.get_du_high_executor_mapper(i),
                                                            dependencies.f1c_client_handler,
                                                            dependencies.f1u_gw,
                                                            dependencies.timer_mng,
                                                            dependencies.mac_p,
                                                            dependencies.rlc_p,
                                                            dependencies.e2_client_handler,
                                                            dependencies.e2_metric_connectors,
                                                            dependencies.json_sink,
                                                            dependencies.metrics_notifier,
                                                            sector_deps};

    du_insts.push_back(make_o_du_high_unit(odu_hi_unit_params, std::move(odu_hi_unit_dependencies)));

    if (i != 0) {
      update_du_metrics(du_insts.front().metrics,
                        std::move(du_insts.back().metrics),
                        tmp_cfg.du_high_cfg.config.e2_cfg.enable_unit_e2);

      // Use commands of the first cell only.
      du_insts.back().commands.clear();
    }
  }

  // Configure the application unit metrics for the DU high.
  announce_du_high_cells(du_hi);

  return du_insts;
}
