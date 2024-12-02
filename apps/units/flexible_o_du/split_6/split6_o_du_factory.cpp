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

#include "split6_o_du_factory.h"
#include "apps/services/e2/e2_metric_connector_manager.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/units/flexible_o_du/flexible_o_du_commands.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_translators.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_factory.h"
#include "apps/units/flexible_o_du/split_helpers/o_du_high_factory.h"
#include "split6_o_du_impl.h"
#include "split6_o_du_unit_config.h"
#include "srsran/e2/e2_du_metrics_connector.h"

using namespace srsran;

o_du_unit srsran::create_o_du_split6(const split6_o_du_unit_config&                   du_unit_cfg,
                                     const o_du_unit_dependencies&                    du_dependencies,
                                     std::vector<std::unique_ptr<fapi::fapi_adaptor>> fapi_adaptors)
{
  o_du_unit odu_unit;
  odu_unit.e2_metric_connectors = std::make_unique<
      e2_metric_connector_manager<e2_du_metrics_connector, e2_du_metrics_notifier, e2_du_metrics_interface>>(
      du_unit_cfg.odu_high_cfg.du_high_cfg.config.cells_cfg.size());

  std::vector<o_du_high_unit> du_high_insts;

  // Create O-DU high.
  o_du_high_unit_factory_dependencies odu_hi_unit_dependencies = {*du_dependencies.workers,
                                                                  *du_dependencies.f1c_client_handler,
                                                                  *du_dependencies.f1u_gw,
                                                                  *du_dependencies.timer_mng,
                                                                  *du_dependencies.mac_p,
                                                                  *du_dependencies.rlc_p,
                                                                  *du_dependencies.e2_client_handler,
                                                                  *(odu_unit.e2_metric_connectors),
                                                                  *du_dependencies.json_sink,
                                                                  *du_dependencies.metrics_notifier,
                                                                  {}};

  const auto& du_hi_unit_cfg = du_unit_cfg.odu_high_cfg.du_high_cfg.config;
  for (unsigned i = 0, e = du_hi_unit_cfg.cells_cfg.size(); i != e; ++i) {
    auto& sector_deps   = odu_hi_unit_dependencies.o_du_hi_dependencies.sectors.emplace_back();
    sector_deps.gateway = &fapi_adaptors[i]->get_message_interface_collection().get_slot_message_gateway();
    sector_deps.last_msg_notifier =
        &fapi_adaptors[i]->get_message_interface_collection().get_slot_last_message_notifier();
    sector_deps.fapi_executor = du_unit_cfg.odu_high_cfg.fapi_cfg.l2_nof_slots_ahead != 0
                                    ? std::optional(du_dependencies.workers->fapi_exec[i])
                                    : std::make_optional<task_executor*>();
  }

  du_high_insts = make_multicell_with_multi_du(du_unit_cfg.odu_high_cfg, std::move(odu_hi_unit_dependencies));

  odu_unit.commands = std::move(du_high_insts.front().commands);
  odu_unit.metrics  = std::move(du_high_insts.front().metrics);

  std::vector<std::unique_ptr<srs_du::o_du_high>> odu_high_insts;
  for (auto& sector : du_high_insts) {
    odu_high_insts.push_back(std::move(sector.o_du_hi));
  }

  // Create the DU.
  odu_unit.unit = std::make_unique<split6_o_du_impl>(std::move(fapi_adaptors), std::move(odu_high_insts));
  report_error_if_not(odu_unit.unit, "Invalid Distributed Unit");

  announce_du_high_cells(du_hi_unit_cfg);

  return odu_unit;
}
