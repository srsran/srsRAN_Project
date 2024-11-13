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

#include "flexible_du_factory.h"
#include "apps/services/e2/e2_metric_connector_manager.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/units/flexible_du/flexible_du_commands.h"
#include "apps/units/flexible_du/o_du_high/du_high/du_high_config_translators.h"
#include "apps/units/flexible_du/o_du_high/o_du_high_unit_factory.h"
#include "apps/units/flexible_du/o_du_low/o_du_low_unit_factory.h"
#include "apps/units/flexible_du/split_helpers/flexible_du_impl.h"
#include "o_du_high_factory.h"
#include "srsran/du/o_du.h"
#include "srsran/du/o_du_factory.h"
#include "srsran/e2/e2_du_metrics_connector.h"

using namespace srsran;

o_du_unit flexible_du_factory::create_flexible_du(const du_unit_dependencies& dependencies)
{
  o_du_unit odu_unit;
  unsigned  nof_cells           = config.odu_high_cfg.du_high_cfg.config.cells_cfg.size();
  odu_unit.e2_metric_connectors = std::make_unique<
      e2_metric_connector_manager<e2_du_metrics_connector, e2_du_metrics_notifier, e2_du_metrics_interface>>(nof_cells);

  const du_high_unit_config& du_hi = config.odu_high_cfg.du_high_cfg.config;
  const du_low_unit_config&  du_lo = config.du_low_cfg;

  auto du_cells = generate_du_cell_config(du_hi);

  std::vector<std::unique_ptr<srs_du::o_du>> du_insts;
  auto                                       du_impl = std::make_unique<flexible_du_impl>(du_cells.size());

  std::vector<srs_du::cell_prach_ports_entry> prach_ports;
  std::vector<unsigned>                       max_pusch_per_slot;
  for (const auto& high : du_hi.cells_cfg) {
    prach_ports.push_back(high.cell.prach_cfg.ports);
    max_pusch_per_slot.push_back(high.cell.pusch_cfg.max_puschs_per_slot);
  }

  // Create O-DU low.
  std::vector<o_du_low_unit> du_low_units;
  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    o_du_low_unit_config odu_low_cfg = {du_lo,
                                        {prach_ports[i]},
                                        span<const srs_du::du_cell_config>(&du_cells[i], 1),
                                        span<const unsigned>(&max_pusch_per_slot[i], 1),
                                        i,
                                        nof_cells};

    o_du_low_unit_dependencies odu_low_dependencies = {
        du_impl->get_upper_ru_dl_rg_adapter(), du_impl->get_upper_ru_ul_request_adapter(), *dependencies.workers};
    du_low_units.push_back(make_o_du_low_unit(odu_low_cfg, odu_low_dependencies));
  }

  // Create O-DU high.
  o_du_high_unit_factory_dependencies odu_hi_unit_dependencies = {*dependencies.workers,
                                                                  *dependencies.f1c_client_handler,
                                                                  *dependencies.f1u_gw,
                                                                  *dependencies.timer_mng,
                                                                  *dependencies.mac_p,
                                                                  *dependencies.rlc_p,
                                                                  *dependencies.e2_client_handler,
                                                                  *(odu_unit.e2_metric_connectors),
                                                                  *dependencies.json_sink,
                                                                  *dependencies.metrics_notifier,
                                                                  {}};

  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    auto& sector_deps             = odu_hi_unit_dependencies.o_du_hi_dependencies.sectors.emplace_back();
    sector_deps.gateway           = &du_low_units[i].o_du_lo->get_slot_message_gateway(0);
    sector_deps.last_msg_notifier = &du_low_units[i].o_du_lo->get_slot_last_message_notifier(0);
    sector_deps.fapi_executor     = config.odu_high_cfg.fapi_cfg.l2_nof_slots_ahead != 0
                                        ? std::optional(dependencies.workers->fapi_exec[i])
                                        : std::make_optional<task_executor*>();
  }
  std::vector<o_du_high_unit> du_high_units =
      make_multicell_with_multi_du(config.odu_high_cfg, std::move(odu_hi_unit_dependencies));

  // Create the O-DU.
  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    du_insts.push_back(srs_du::make_o_du(std::move(du_high_units[i].o_du_hi), std::move(du_low_units[i].o_du_lo)));
  }

  // Move commands and metrics.
  odu_unit.metrics  = std::move(du_high_units.front().metrics);
  odu_unit.commands = std::move(du_high_units.front().commands);

  flexible_du_ru_config       ru_config{{du_cells},
                                  du_lo.expert_phy_cfg.max_processing_delay_slots,
                                  static_cast<unsigned>(du_hi.cells_cfg.front().cell.prach_cfg.ports.size())};
  flexible_du_ru_dependencies ru_dependencies{*dependencies.workers,
                                              du_impl->get_upper_ru_ul_adapter(),
                                              du_impl->get_upper_ru_timing_adapter(),
                                              du_impl->get_upper_ru_error_adapter()};

  std::unique_ptr<radio_unit> ru = create_radio_unit(ru_config, ru_dependencies);
  srsran_assert(ru, "Invalid Radio Unit");

  // Add RU commands.
  odu_unit.commands.push_back(std::make_unique<change_log_level_app_command>());
  odu_unit.commands.push_back(std::make_unique<ru_metrics_app_command>(ru->get_controller()));
  odu_unit.commands.push_back(std::make_unique<tx_gain_app_command>(ru->get_controller()));
  odu_unit.commands.push_back(std::make_unique<rx_gain_app_command>(ru->get_controller()));

  du_impl->add_ru(std::move(ru));
  du_impl->add_dus(std::move(du_insts));
  odu_unit.unit = std::move(du_impl);

  return odu_unit;
}
