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

#include "dynamic_du_factory.h"
#include "apps/services/e2_metric_connector_manager.h"
#include "apps/services/metrics_log_helper.h"
#include "apps/units/flexible_du/du_high/du_high_commands.h"
#include "apps/units/flexible_du/du_high/du_high_config_translators.h"
#include "apps/units/flexible_du/du_high/du_high_wrapper_config_helper.h"
#include "apps/units/flexible_du/du_low/du_low_config_translator.h"
#include "apps/units/flexible_du/du_low/du_low_wrapper_config_helper.h"
#include "apps/units/flexible_du/flexible_du_commands.h"
#include "apps/units/flexible_du/split_7_2/ru_ofh_factories.h"
#include "apps/units/flexible_du/split_8/ru_sdr_factories.h"
#include "dynamic_du_impl.h"
#include "dynamic_du_translators.h"
#include "srsran/du/du_wrapper.h"
#include "srsran/du/du_wrapper_factory.h"
#include "srsran/pcap/rlc_pcap.h"
#include "srsran/ru/ru_dummy_factory.h"

using namespace srsran;

static std::unique_ptr<radio_unit> create_dummy_radio_unit(const ru_dummy_unit_config& ru_cfg,
                                                           unsigned                    max_processing_delay_slots,
                                                           unsigned                    nof_prach_ports,
                                                           worker_manager&             workers,
                                                           span<const du_cell_config>  du_cells,
                                                           ru_uplink_plane_rx_symbol_notifier& symbol_notifier,
                                                           ru_timing_notifier&                 timing_notifier,
                                                           ru_error_notifier&                  error_notifier)
{
  ru_dummy_dependencies dependencies;
  dependencies.logger          = &srslog::fetch_basic_logger("RU");
  dependencies.executor        = workers.radio_exec;
  dependencies.timing_notifier = &timing_notifier;
  dependencies.symbol_notifier = &symbol_notifier;

  return create_dummy_ru(generate_ru_dummy_config(ru_cfg, du_cells, max_processing_delay_slots, nof_prach_ports),
                         dependencies);
}

static std::unique_ptr<radio_unit>
create_radio_unit(const std::variant<ru_sdr_unit_config, ru_ofh_unit_parsed_config, ru_dummy_unit_config>& ru_cfg,
                  worker_manager&                                                                          workers,
                  span<const du_cell_config>                                                               du_cells,
                  ru_uplink_plane_rx_symbol_notifier& symbol_notifier,
                  ru_timing_notifier&                 timing_notifier,
                  ru_error_notifier&                  error_notifier,
                  unsigned                            max_processing_delay,
                  unsigned                            prach_nof_ports)
{
  if (std::holds_alternative<ru_ofh_unit_parsed_config>(ru_cfg)) {
    ru_ofh_factory_config config;
    config.ru_cfg                     = std::get<ru_ofh_unit_parsed_config>(ru_cfg).config;
    config.max_processing_delay_slots = max_processing_delay;
    config.du_cells                   = du_cells;

    ru_ofh_factory_dependencies dependencies;
    dependencies.workers         = &workers;
    dependencies.error_notifier  = &error_notifier;
    dependencies.symbol_notifier = &symbol_notifier;
    dependencies.timing_notifier = &timing_notifier;

    return create_ofh_radio_unit(config, dependencies);
  }

  if (std::holds_alternative<ru_sdr_unit_config>(ru_cfg)) {
    ru_sdr_factory_config config;
    config.du_cells                   = du_cells;
    config.ru_cfg                     = std::get<ru_sdr_unit_config>(ru_cfg);
    config.max_processing_delay_slots = max_processing_delay;

    ru_sdr_factory_dependencies dependencies;
    dependencies.workers         = &workers;
    dependencies.error_notifier  = &error_notifier;
    dependencies.symbol_notifier = &symbol_notifier;
    dependencies.timing_notifier = &timing_notifier;

    return create_sdr_radio_unit(config, dependencies);
  }

  return create_dummy_radio_unit(std::get<ru_dummy_unit_config>(ru_cfg),
                                 max_processing_delay,
                                 prach_nof_ports,
                                 workers,
                                 du_cells,
                                 symbol_notifier,
                                 timing_notifier,
                                 error_notifier);
}

du_unit srsran::create_du(const dynamic_du_unit_config&  dyn_du_cfg,
                          worker_manager&                workers,
                          srs_du::f1c_connection_client& f1c_client_handler,
                          srs_du::f1u_du_gateway&        f1u_gw,
                          timer_manager&                 timer_mng,
                          mac_pcap&                      mac_p,
                          rlc_pcap&                      rlc_p,
                          metrics_plotter_stdout&        metrics_stdout,
                          metrics_plotter_json&          metrics_json,
                          metrics_log_helper&            metrics_logger,
                          e2_connection_client&          e2_client_handler,
                          e2_metric_connector_manager&   e2_metric_connectors,
                          rlc_metrics_notifier&          rlc_json_metrics,
                          metrics_hub&                   metrics_hub)
{
  du_unit du_cmd_wrapper;

  const du_high_unit_config& du_hi    = dyn_du_cfg.du_high_cfg.config;
  const du_low_unit_config&  du_lo    = dyn_du_cfg.du_low_cfg;
  const fapi_unit_config&    fapi_cfg = dyn_du_cfg.fapi_cfg;

  // Configure the application unit metrics for the DU high.
  configure_du_high_metrics(
      du_hi, metrics_stdout, metrics_logger, metrics_json, rlc_json_metrics, e2_metric_connectors, metrics_hub);

  auto du_cells = generate_du_cell_config(du_hi);

  std::vector<std::unique_ptr<du_wrapper>> du_insts;
  auto                                     du_impl = std::make_unique<dynamic_du_impl>(du_cells.size());

  std::vector<cell_prach_ports_entry> prach_ports;
  std::vector<unsigned>               max_pusch_per_slot;
  for (const auto& high : du_hi.cells_cfg) {
    prach_ports.push_back(high.cell.prach_cfg.ports);
    max_pusch_per_slot.push_back(high.cell.pusch_cfg.max_puschs_per_slot);
  }

  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    // Create one DU per cell.
    du_wrapper_config   du_cfg  = {};
    du_high_unit_config tmp_cfg = du_hi;
    tmp_cfg.cells_cfg.resize(1);
    tmp_cfg.cells_cfg[0] = du_hi.cells_cfg[i];

    make_du_low_wrapper_config_and_dependencies(du_cfg.du_low_cfg,
                                                du_lo,
                                                {prach_ports[i]},
                                                span<const du_cell_config>(&du_cells[i], 1),
                                                span<const unsigned>(&max_pusch_per_slot[i], 1),
                                                du_impl->get_upper_ru_dl_rg_adapter(),
                                                du_impl->get_upper_ru_ul_request_adapter(),
                                                workers,
                                                i);

    fill_du_high_wrapper_config(du_cfg.du_high_cfg,
                                tmp_cfg,
                                i,
                                workers.get_du_high_executor_mapper(i),
                                f1c_client_handler,
                                f1u_gw,
                                timer_mng,
                                mac_p,
                                rlc_p,
                                e2_client_handler,
                                e2_metric_connectors,
                                metrics_hub);

    // FAPI configuration.
    du_cfg.du_high_cfg.fapi.log_level = fapi_cfg.fapi_level;
    if (fapi_cfg.l2_nof_slots_ahead != 0) {
      // As the temporal configuration contains only one cell, pick the data from that cell.
      du_cfg.du_high_cfg.fapi.l2_nof_slots_ahead = fapi_cfg.l2_nof_slots_ahead;
      du_cfg.du_high_cfg.fapi.executor.emplace(workers.fapi_exec[i]);
    } else {
      report_error_if_not(workers.fapi_exec[i] == nullptr,
                          "FAPI buffered worker created for a cell with no MAC delay configured");
    }

    du_insts.push_back(make_du_wrapper(du_cfg));
    report_error_if_not(du_insts.back(), "Invalid Distributed Unit");
  }

  std::unique_ptr<radio_unit> ru = create_radio_unit(dyn_du_cfg.ru_cfg,
                                                     workers,
                                                     du_cells,
                                                     du_impl->get_upper_ru_ul_adapter(),
                                                     du_impl->get_upper_ru_timing_adapter(),
                                                     du_impl->get_upper_ru_error_adapter(),
                                                     du_lo.expert_phy_cfg.max_processing_delay_slots,
                                                     du_hi.cells_cfg.front().cell.prach_cfg.ports.size());

  srsran_assert(ru, "Invalid Radio Unit");

  // Add RU commands.
  du_cmd_wrapper.commands.push_back(std::make_unique<change_log_level_app_command>());
  du_cmd_wrapper.commands.push_back(std::make_unique<ru_metrics_app_command>(ru->get_controller()));
  du_cmd_wrapper.commands.push_back(std::make_unique<tx_gain_app_command>(ru->get_controller()));
  du_cmd_wrapper.commands.push_back(std::make_unique<rx_gain_app_command>(ru->get_controller()));

  // Add DU command.
  du_cmd_wrapper.commands.push_back(std::make_unique<toggle_stdout_metrics_app_command>(metrics_stdout));

  du_impl->add_ru(std::move(ru));
  du_impl->add_dus(std::move(du_insts));

  du_cmd_wrapper.unit = std::move(du_impl);

  return du_cmd_wrapper;
}
