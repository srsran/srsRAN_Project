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

#include "split6_du_factory.h"
#include "apps/services/e2_metric_connector_manager.h"
#include "apps/services/worker_manager.h"
#include "apps/units/flexible_du/du_high/du_high_commands.h"
#include "apps/units/flexible_du/du_high/du_high_config_translators.h"
#include "apps/units/flexible_du/du_high/du_high_wrapper_config_helper.h"
#include "apps/units/flexible_du/flexible_du_commands.h"
#include "split6_du_impl.h"
#include "split6_du_unit_config.h"
#include "srsran/du/du_high_wrapper.h"
#include "srsran/du/du_high_wrapper_factory.h"

using namespace srsran;

/// \brief Update the Flexible DU metrics configuration with the given local DU configuration and E2 configuration.
///
/// This function manages the multicell workaround for the DU high metrics. To have multicell, now one DU is
/// instantiated per cell, so the this would create multiple consumers that does not make sense, for example stdout.
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

du_unit srsran::create_du_split6(const split6_du_unit_config&                     du_unit_cfg,
                                 const du_unit_dependencies&                      du_dependencies,
                                 std::vector<std::unique_ptr<fapi::fapi_adaptor>> fapi_adaptors)
{
  du_unit du_cmd_wrapper;

  const du_high_unit_config& du_hi    = du_unit_cfg.du_high_cfg.config;
  const fapi_unit_config&    fapi_cfg = du_unit_cfg.fapi_cfg;

  auto du_cells = generate_du_cell_config(du_hi);

  std::vector<std::unique_ptr<du_high_wrapper>> du_insts;
  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    // Create one DU per cell.
    du_high_wrapper_config du_cfg  = {};
    du_high_unit_config    tmp_cfg = du_hi;
    tmp_cfg.cells_cfg.resize(1);
    tmp_cfg.cells_cfg[0] = du_hi.cells_cfg[i];

    auto cell_services_cfg = fill_du_high_wrapper_config(du_cfg,
                                                         tmp_cfg,
                                                         i,
                                                         du_dependencies.workers->get_du_high_executor_mapper(i),
                                                         *du_dependencies.f1c_client_handler,
                                                         *du_dependencies.f1u_gw,
                                                         *du_dependencies.timer_mng,
                                                         *du_dependencies.mac_p,
                                                         *du_dependencies.rlc_p,
                                                         *du_dependencies.e2_client_handler,
                                                         *du_dependencies.e2_metric_connectors,
                                                         *du_dependencies.json_sink,
                                                         *du_dependencies.metrics_notifier);

    update_du_metrics(du_cmd_wrapper.metrics, std::move(cell_services_cfg.first), tmp_cfg.e2_cfg.enable_du_e2);

    // Use the commands of the first cell.
    if (i == 0) {
      for (auto& command : cell_services_cfg.second)
        du_cmd_wrapper.commands.push_back(std::move(command));
    }

    // FAPI configuration.
    du_cfg.fapi.log_level = fapi_cfg.fapi_level;
    if (fapi_cfg.l2_nof_slots_ahead != 0) {
      // As the temporal configuration contains only one cell, pick the data from that cell.
      du_cfg.fapi.l2_nof_slots_ahead = fapi_cfg.l2_nof_slots_ahead;
      du_cfg.fapi.executor.emplace(du_dependencies.workers->fapi_exec[i]);
    } else {
      report_error_if_not(du_dependencies.workers->fapi_exec[i] == nullptr,
                          "FAPI buffered worker created for a cell with no MAC delay configured");
    }

    du_high_wrapper_dependencies sector_dependencies;
    sector_dependencies.sectors.push_back(
        {&fapi_adaptors[i]->get_message_interface_collection().get_slot_message_gateway(),
         &fapi_adaptors[i]->get_message_interface_collection().get_slot_last_message_notifier()});

    du_insts.push_back(make_du_high_wrapper(du_cfg, std::move(sector_dependencies)));
    report_error_if_not(du_insts.back(), "Invalid Distributed Unit wrapper");
  }

  // Create the DU.
  du_cmd_wrapper.unit = std::make_unique<split6_du_impl>(std::move(fapi_adaptors), std::move(du_insts));
  report_error_if_not(du_cmd_wrapper.unit, "Invalid Distributed Unit");

  announce_du_high_cells(du_hi);

  return du_cmd_wrapper;
}
