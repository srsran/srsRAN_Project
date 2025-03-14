/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "flexible_o_du_factory.h"
#include "apps/helpers/e2/e2_metric_connector_manager.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/units/flexible_o_du/flexible_o_du_commands.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_translators.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_factory.h"
#include "apps/units/flexible_o_du/o_du_low/o_du_low_unit_factory.h"
#include "flexible_o_du_impl.h"
#include "metrics/flexible_o_du_metrics_builder.h"
#include "srsran/du/o_du_factory.h"
#include "srsran/e2/e2_du_metrics_connector.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_sector_adaptor.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_sector_adaptor.h"

using namespace srsran;

o_du_unit flexible_o_du_factory::create_flexible_o_du(const o_du_unit_dependencies& dependencies)
{
  o_du_unit      o_du;
  const unsigned nof_cells  = config.odu_high_cfg.du_high_cfg.config.cells_cfg.size();
  o_du.e2_metric_connectors = std::make_unique<
      e2_metric_connector_manager<e2_du_metrics_connector, e2_du_metrics_notifier, e2_du_metrics_interface>>(nof_cells);

  const du_high_unit_config& du_hi = config.odu_high_cfg.du_high_cfg.config;
  const du_low_unit_config&  du_lo = config.du_low_cfg;

  auto du_cells = generate_du_cell_config(du_hi);

  std::vector<pci_t> pci_cell_mapper;

  for (const auto& cell : du_cells) {
    pci_cell_mapper.push_back(cell.pci);
  }
  // Create flexible O-DU metrics configuration.
  flexible_o_du_metrics_notifier* flexible_odu_metrics_notifier =
      build_flexible_o_du_metrics_config(o_du.metrics,
                                         *dependencies.metrics_notifier,
                                         config.odu_high_cfg.du_high_cfg.config.metrics.common_metrics_cfg,
                                         std::move(pci_cell_mapper));

  // Create flexible O-DU implementation.
  auto du_impl = std::make_unique<flexible_o_du_impl>(nof_cells, flexible_odu_metrics_notifier);

  std::vector<srs_du::cell_prach_ports_entry> prach_ports;
  std::vector<unsigned>                       max_pusch_per_slot;
  for (const auto& high : du_hi.cells_cfg) {
    prach_ports.push_back(high.cell.prach_cfg.ports);
    max_pusch_per_slot.push_back(high.cell.pusch_cfg.max_puschs_per_slot);
  }

  o_du_low_unit_config       odu_low_cfg          = {du_lo, prach_ports, du_cells, max_pusch_per_slot};
  o_du_low_unit_dependencies odu_low_dependencies = {
      du_impl->get_upper_ru_dl_rg_adapter(), du_impl->get_upper_ru_ul_request_adapter(), *dependencies.workers};
  o_du_low_unit_factory odu_low_factory(du_lo.hal_config, nof_cells);
  auto                  odu_lo_unit = odu_low_factory.create(odu_low_cfg, odu_low_dependencies);

  std::for_each(odu_lo_unit.metrics.begin(), odu_lo_unit.metrics.end(), [&](auto& e) {
    o_du.metrics.emplace_back(std::move(e));
  });

  o_du_high_unit_dependencies odu_hi_unit_dependencies = {dependencies.workers->get_du_high_executor_mapper(0),
                                                          *dependencies.f1c_client_handler,
                                                          *dependencies.f1u_gw,
                                                          *dependencies.timer_mng,
                                                          *dependencies.mac_p,
                                                          *dependencies.rlc_p,
                                                          *dependencies.e2_client_handler,
                                                          *(o_du.e2_metric_connectors),
                                                          *dependencies.metrics_notifier,
                                                          {}};

  // Adjust the dependencies.
  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    auto& sector_dependencies = odu_hi_unit_dependencies.o_du_hi_dependencies.sectors.emplace_back();
    sector_dependencies.gateway =
        &odu_lo_unit.o_du_lo->get_phy_fapi_adaptor().get_sector_adaptor(i).get_slot_message_gateway();
    sector_dependencies.last_msg_notifier =
        &odu_lo_unit.o_du_lo->get_phy_fapi_adaptor().get_sector_adaptor(i).get_slot_last_message_notifier();
    sector_dependencies.fapi_executor = config.odu_high_cfg.fapi_cfg.l2_nof_slots_ahead != 0
                                            ? std::optional(dependencies.workers->fapi_exec[i])
                                            : std::make_optional<task_executor*>();
  }

  o_du_high_unit odu_hi_unit = make_o_du_high_unit(config.odu_high_cfg, std::move(odu_hi_unit_dependencies));

  // Connect the adaptors.
  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    fapi_adaptor::phy_fapi_sector_adaptor& odu_lo = odu_lo_unit.o_du_lo->get_phy_fapi_adaptor().get_sector_adaptor(i);
    fapi_adaptor::mac_fapi_sector_adaptor& odu_hi = odu_hi_unit.o_du_hi->get_mac_fapi_adaptor().get_sector_adaptor(i);

    // Connect O-DU low with O-DU high.
    odu_lo.set_slot_time_message_notifier(odu_hi.get_slot_time_message_notifier());
    odu_lo.set_slot_error_message_notifier(odu_hi.get_slot_error_message_notifier());
    odu_lo.set_slot_data_message_notifier(odu_hi.get_slot_data_message_notifier());
  }

  std::for_each(odu_hi_unit.metrics.begin(), odu_hi_unit.metrics.end(), [&](auto& e) {
    o_du.metrics.emplace_back(std::move(e));
  });

  o_du.commands = std::move(odu_hi_unit.commands);

  srs_du::o_du_dependencies odu_dependencies;
  odu_dependencies.odu_hi           = std::move(odu_hi_unit.o_du_hi);
  odu_dependencies.odu_lo           = std::move(odu_lo_unit.o_du_lo);
  odu_dependencies.metrics_notifier = &du_impl->get_o_du_metrics_notifier();

  auto odu_instance = make_o_du(std::move(odu_dependencies));
  report_error_if_not(odu_instance, "Invalid Distributed Unit");

  flexible_o_du_ru_config       ru_config{{du_cells},
                                    du_lo.expert_phy_cfg.max_processing_delay_slots,
                                    static_cast<unsigned>(du_hi.cells_cfg.front().cell.prach_cfg.ports.size())};
  flexible_o_du_ru_dependencies ru_dependencies{*dependencies.workers,
                                                du_impl->get_upper_ru_ul_adapter(),
                                                du_impl->get_upper_ru_timing_adapter(),
                                                du_impl->get_upper_ru_error_adapter()};

  std::unique_ptr<radio_unit> ru = create_radio_unit(ru_config, ru_dependencies);

  srsran_assert(ru, "Invalid Radio Unit");

  // Add RU command-line commands.
  o_du.commands.cmdline.push_back(std::make_unique<change_log_level_app_command>());
  o_du.commands.cmdline.push_back(std::make_unique<ru_metrics_app_command>(ru->get_controller()));

  // Create the RU gain commands.
  if (auto* controller = ru->get_controller().get_gain_controller()) {
    o_du.commands.cmdline.push_back(std::make_unique<tx_gain_app_command>(*controller));
    o_du.commands.cmdline.push_back(std::make_unique<rx_gain_app_command>(*controller));
  }

  // Create the RU CFO command.
  if (auto* controller = ru->get_controller().get_cfo_controller()) {
    o_du.commands.cmdline.push_back(std::make_unique<cfo_app_command>(*controller));
  }

  // Configure the RU and DU in the dynamic DU.
  du_impl->add_ru(std::move(ru));
  du_impl->add_du(std::move(odu_instance));

  o_du.unit = std::move(du_impl);

  announce_du_high_cells(du_hi);

  return o_du;
}
