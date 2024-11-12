/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "multicell_flexible_du_factory.h"
#include "apps/services/e2/e2_metric_connector_manager.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/units/flexible_du/flexible_du_commands.h"
#include "apps/units/flexible_du/o_du_high/du_high/du_high_config_translators.h"
#include "apps/units/flexible_du/o_du_high/o_du_high_unit_factory.h"
#include "apps/units/flexible_du/o_du_low/o_du_low_unit_factory.h"
#include "multicell_flexible_du_impl.h"
#include "srsran/du/o_du_factory.h"
#include "srsran/e2/e2_du_metrics_connector.h"

using namespace srsran;

o_du_unit multicell_flexible_du_factory::create_flexible_du(const du_unit_dependencies& dependencies)
{
  o_du_unit o_du;
  o_du.e2_metric_connectors = std::make_unique<
      e2_metric_connector_manager<e2_du_metrics_connector, e2_du_metrics_notifier, e2_du_metrics_interface>>(
      config.odu_high_cfg.du_high_cfg.config.cells_cfg.size());

  const du_high_unit_config& du_hi = config.odu_high_cfg.du_high_cfg.config;
  const du_low_unit_config&  du_lo = config.du_low_cfg;

  auto du_cells = generate_du_cell_config(du_hi);

  auto du_impl = std::make_unique<multicell_flexible_du_impl>(du_cells.size());

  std::vector<srs_du::cell_prach_ports_entry> prach_ports;
  std::vector<unsigned>                       max_pusch_per_slot;
  for (const auto& high : du_hi.cells_cfg) {
    prach_ports.push_back(high.cell.prach_cfg.ports);
    max_pusch_per_slot.push_back(high.cell.pusch_cfg.max_puschs_per_slot);
  }

  static constexpr unsigned du_id       = 0U;
  o_du_low_unit_config      odu_low_cfg = {
           du_lo, prach_ports, du_cells, max_pusch_per_slot, du_id, static_cast<unsigned>(du_cells.size())};

  o_du_low_unit_dependencies odu_low_dependencies = {
      du_impl->get_upper_ru_dl_rg_adapter(), du_impl->get_upper_ru_ul_request_adapter(), *dependencies.workers};
  auto odu_lo_unit = make_o_du_low_unit(odu_low_cfg, odu_low_dependencies);

  o_du_high_unit_params odu_hi_unit_params = {config.odu_high_cfg, du_id};

  o_du_high_unit_dependencies odu_hi_unit_dependencies = {dependencies.workers->get_du_high_executor_mapper(du_id),
                                                          *dependencies.f1c_client_handler,
                                                          *dependencies.f1u_gw,
                                                          *dependencies.timer_mng,
                                                          *dependencies.mac_p,
                                                          *dependencies.rlc_p,
                                                          *dependencies.e2_client_handler,
                                                          *(o_du.e2_metric_connectors),
                                                          *dependencies.json_sink,
                                                          *dependencies.metrics_notifier,
                                                          {}};

  // Adjust the dependencies.
  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    auto& sector_dependencies             = odu_hi_unit_dependencies.o_du_hi_dependencies.sectors.emplace_back();
    sector_dependencies.gateway           = &odu_lo_unit.o_du_lo->get_slot_message_gateway(i);
    sector_dependencies.last_msg_notifier = &odu_lo_unit.o_du_lo->get_slot_last_message_notifier(i);
    sector_dependencies.fapi_executor     = config.odu_high_cfg.fapi_cfg.l2_nof_slots_ahead != 0
                                                ? std::optional(dependencies.workers->fapi_exec[i])
                                                : std::make_optional<task_executor*>();
  }

  o_du_high_unit odu_hi_unit = make_o_du_high_unit(odu_hi_unit_params, std::move(odu_hi_unit_dependencies));

  o_du.metrics  = std::move(odu_hi_unit.metrics);
  o_du.commands = std::move(odu_hi_unit.commands);

  auto odu_instance = make_o_du(std::move(odu_hi_unit.o_du_hi), std::move(odu_lo_unit.o_du_lo));
  report_error_if_not(odu_instance, "Invalid Distributed Unit");

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
  o_du.commands.push_back(std::make_unique<change_log_level_app_command>());
  o_du.commands.push_back(std::make_unique<ru_metrics_app_command>(ru->get_controller()));
  o_du.commands.push_back(std::make_unique<tx_gain_app_command>(ru->get_controller()));
  o_du.commands.push_back(std::make_unique<rx_gain_app_command>(ru->get_controller()));

  // Configure the RU and DU in the dynamic DU.
  du_impl->add_ru(std::move(ru));
  du_impl->add_du(std::move(odu_instance));

  o_du.unit = std::move(du_impl);

  announce_du_high_cells(du_hi);

  return o_du;
}
