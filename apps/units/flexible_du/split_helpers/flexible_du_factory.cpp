/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "flexible_du_factory.h"
#include "apps/services/e2/e2_metric_connector_manager.h"
#include "apps/services/worker_manager.h"
#include "apps/units/flexible_du/du_high/du_high_commands.h"
#include "apps/units/flexible_du/du_high/du_high_config_translators.h"
#include "apps/units/flexible_du/du_high/du_high_wrapper_config_helper.h"
#include "apps/units/flexible_du/du_low/du_low_config.h"
#include "apps/units/flexible_du/du_low/du_low_config_translator.h"
#include "apps/units/flexible_du/du_low/du_low_wrapper_config_helper.h"
#include "apps/units/flexible_du/flexible_du_commands.h"
#include "apps/units/flexible_du/split_helpers/flexible_du_impl.h"
#include "srsran/du/du_wrapper.h"
#include "srsran/du/du_wrapper_factory.h"
#include "srsran/e2/e2_du_metrics_connector.h"

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

du_unit flexible_du_factory::create_flexible_du(const du_unit_dependencies& dependencies)
{
  du_unit  du_cmd_wrapper;
  unsigned nof_cells                  = config.du_high_cfg.config.cells_cfg.size();
  du_cmd_wrapper.e2_metric_connectors = std::make_unique<
      e2_metric_connector_manager<e2_du_metrics_connector, e2_du_metrics_notifier, e2_du_metrics_interface>>(nof_cells);

  const du_high_unit_config& du_hi    = config.du_high_cfg.config;
  const du_low_unit_config&  du_lo    = config.du_low_cfg;
  const fapi_unit_config&    fapi_cfg = config.fapi_cfg;

  auto du_cells = generate_du_cell_config(du_hi);

  std::vector<std::unique_ptr<srs_du::du_wrapper>> du_insts;
  auto                                             du_impl = std::make_unique<flexible_du_impl>(du_cells.size());

  std::vector<srs_du::cell_prach_ports_entry> prach_ports;
  std::vector<unsigned>                       max_pusch_per_slot;
  for (const auto& high : du_hi.cells_cfg) {
    prach_ports.push_back(high.cell.prach_cfg.ports);
    max_pusch_per_slot.push_back(high.cell.pusch_cfg.max_puschs_per_slot);
  }

  // Initialize and configure the HAL.
  // :TODO: review it. It should be managed internally in the DU low.
  hal_upper_phy_config du_low_hal_cfg = make_du_low_hal_config_and_dependencies(du_lo, du_cells.size());

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
                                                i,
                                                du_low_hal_cfg);

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
                                                         *(du_cmd_wrapper.e2_metric_connectors),
                                                         *dependencies.json_sink,
                                                         *dependencies.metrics_notifier);

    update_du_metrics(du_cmd_wrapper.metrics, std::move(cell_services_cfg.first), tmp_cfg.e2_cfg.enable_unit_e2);

    // Use the commands of the first cell.
    if (i == 0) {
      for (auto& command : cell_services_cfg.second)
        du_cmd_wrapper.commands.push_back(std::move(command));
    }

    srs_du::du_wrapper_dependencies wrapper_deps;
    wrapper_deps.du_high_deps.sectors.resize(tmp_cfg.cells_cfg.size());

    // FAPI configuration.
    du_cfg.du_high_cfg.fapi.log_level = fapi_cfg.fapi_level;
    if (fapi_cfg.l2_nof_slots_ahead != 0) {
      auto& du_hi_cell_deps = wrapper_deps.du_high_deps.sectors.front();
      // As the temporal configuration contains only one cell, pick the data from that cell.
      du_cfg.du_high_cfg.fapi.l2_nof_slots_ahead = fapi_cfg.l2_nof_slots_ahead;
      du_hi_cell_deps.fapi_executor.emplace(dependencies.workers->fapi_exec[i]);
    }

    du_insts.push_back(make_du_wrapper(du_cfg, std::move(wrapper_deps)));
    report_error_if_not(du_insts.back(), "Invalid Distributed Unit");
  }

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