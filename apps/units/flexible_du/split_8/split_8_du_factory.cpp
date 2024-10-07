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

#include "split_8_du_factory.h"
#include "apps/services/e2_metric_connector_manager.h"
#include "apps/services/worker_manager.h"
#include "apps/units/flexible_du/du_high/du_high_commands.h"
#include "apps/units/flexible_du/du_high/du_high_config_translators.h"
#include "apps/units/flexible_du/du_high/du_high_wrapper_config_helper.h"
#include "apps/units/flexible_du/du_low/du_low_config_translator.h"
#include "apps/units/flexible_du/du_low/du_low_wrapper_config_helper.h"
#include "apps/units/flexible_du/flexible_du_commands.h"
#include "apps/units/flexible_du/split_8/helpers/ru_sdr_factories.h"
#include "split_8_du_impl.h"
#include "srsran/du/du_wrapper.h"
#include "srsran/du/du_wrapper_factory.h"
#include "srsran/pcap/rlc_pcap.h"
#include "srsran/ru/ru_dummy_factory.h"

using namespace srsran;

static std::unique_ptr<radio_unit> create_radio_unit(const ru_sdr_unit_config&           ru_cfg,
                                                     worker_manager&                     workers,
                                                     span<const srs_du::du_cell_config>  du_cells,
                                                     ru_uplink_plane_rx_symbol_notifier& symbol_notifier,
                                                     ru_timing_notifier&                 timing_notifier,
                                                     ru_error_notifier&                  error_notifier,
                                                     unsigned                            max_processing_delay)
{
  ru_sdr_factory_config config;
  config.du_cells                   = du_cells;
  config.ru_cfg                     = ru_cfg;
  config.max_processing_delay_slots = max_processing_delay;

  ru_sdr_factory_dependencies dependencies;
  dependencies.workers         = &workers;
  dependencies.error_notifier  = &error_notifier;
  dependencies.symbol_notifier = &symbol_notifier;
  dependencies.timing_notifier = &timing_notifier;

  return create_sdr_radio_unit(config, dependencies);
}

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

du_unit srsran::create_split_8_du(const split_8_du_unit_config& du_8_cfg, const du_unit_dependencies& dependencies)
{
  du_unit du_cmd_wrapper;

  const du_high_unit_config& du_hi    = du_8_cfg.du_high_cfg.config;
  const du_low_unit_config&  du_lo    = du_8_cfg.du_low_cfg;
  const fapi_unit_config&    fapi_cfg = du_8_cfg.fapi_cfg;

  auto du_cells = generate_du_cell_config(du_hi);

  std::vector<std::unique_ptr<srs_du::du_wrapper>> du_insts;
  auto                                             du_impl = std::make_unique<split_8_du_impl>(du_cells.size());

  std::vector<srs_du::cell_prach_ports_entry> prach_ports;
  std::vector<unsigned>                       max_pusch_per_slot;
  for (const auto& high : du_hi.cells_cfg) {
    prach_ports.push_back(high.cell.prach_cfg.ports);
    max_pusch_per_slot.push_back(high.cell.pusch_cfg.max_puschs_per_slot);
  }

  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    // Create one DU per cell.
    srs_du::du_wrapper_config du_cfg  = {};
    du_high_unit_config       tmp_cfg = du_hi;
    tmp_cfg.cells_cfg.resize(1);
    tmp_cfg.cells_cfg[0] = du_hi.cells_cfg[i];

    make_du_low_wrapper_config_and_dependencies(du_cfg.du_low_cfg,
                                                du_lo,
                                                {prach_ports[i]},
                                                span<const srs_du::du_cell_config>(&du_cells[i], 1),
                                                span<const unsigned>(&max_pusch_per_slot[i], 1),
                                                du_impl->get_upper_ru_dl_rg_adapter(),
                                                du_impl->get_upper_ru_ul_request_adapter(),
                                                *dependencies.workers,
                                                i);

    auto cell_services_cfg = fill_du_high_wrapper_config(du_cfg.du_high_cfg,
                                                         tmp_cfg,
                                                         i,
                                                         dependencies.workers->get_du_high_executor_mapper(i),
                                                         *dependencies.f1c_client_handler,
                                                         *dependencies.f1u_gw,
                                                         *dependencies.timer_mng,
                                                         *dependencies.mac_p,
                                                         *dependencies.rlc_p,
                                                         *dependencies.e2_client_handler,
                                                         *dependencies.e2_metric_connectors,
                                                         *dependencies.json_sink,
                                                         *dependencies.metrics_notifier);

    update_du_metrics(du_cmd_wrapper.metrics, std::move(cell_services_cfg.first), tmp_cfg.e2_cfg.enable_du_e2);

    // Use the commands of the first cell.
    if (i == 0) {
      for (auto& command : cell_services_cfg.second)
        du_cmd_wrapper.commands.push_back(std::move(command));
    }

    // FAPI configuration.
    du_cfg.du_high_cfg.fapi.log_level = fapi_cfg.fapi_level;
    if (fapi_cfg.l2_nof_slots_ahead != 0) {
      // As the temporal configuration contains only one cell, pick the data from that cell.
      du_cfg.du_high_cfg.fapi.l2_nof_slots_ahead = fapi_cfg.l2_nof_slots_ahead;
      du_cfg.du_high_cfg.fapi.executor.emplace(dependencies.workers->fapi_exec[i]);
    }

    du_insts.push_back(make_du_wrapper(du_cfg));
    report_error_if_not(du_insts.back(), "Invalid Distributed Unit");
  }

  std::unique_ptr<radio_unit> ru = create_radio_unit(du_8_cfg.ru_cfg,
                                                     *dependencies.workers,
                                                     du_cells,
                                                     du_impl->get_upper_ru_ul_adapter(),
                                                     du_impl->get_upper_ru_timing_adapter(),
                                                     du_impl->get_upper_ru_error_adapter(),
                                                     du_lo.expert_phy_cfg.max_processing_delay_slots);

  srsran_assert(ru, "Invalid Radio Unit");

  // Add RU commands.
  du_cmd_wrapper.commands.push_back(std::make_unique<change_log_level_app_command>());
  du_cmd_wrapper.commands.push_back(std::make_unique<ru_metrics_app_command>(ru->get_controller()));
  du_cmd_wrapper.commands.push_back(std::make_unique<tx_gain_app_command>(ru->get_controller()));
  du_cmd_wrapper.commands.push_back(std::make_unique<rx_gain_app_command>(ru->get_controller()));

  du_impl->add_ru(std::move(ru));
  du_impl->add_dus(std::move(du_insts));

  du_cmd_wrapper.unit = std::move(du_impl);

  // Configure the application unit metrics for the DU high.
  announce_du_high_cells(du_hi);

  return du_cmd_wrapper;
}
