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
#include "apps/services/worker_manager.h"
#include "apps/units/flexible_du/du_high/du_high_commands.h"
#include "apps/units/flexible_du/du_high/du_high_config_translators.h"
#include "apps/units/flexible_du/du_high/du_high_wrapper_config_helper.h"
#include "apps/units/flexible_du/du_low/du_low_config.h"
#include "apps/units/flexible_du/du_low/du_low_config_translator.h"
#include "apps/units/flexible_du/du_low/du_low_wrapper_config_helper.h"
#include "apps/units/flexible_du/flexible_du_commands.h"
#include "apps/units/flexible_du/split_7_2/helpers/ru_ofh_factories.h"
#include "apps/units/flexible_du/split_8/helpers/ru_sdr_factories.h"
#include "multicell_flexible_du_impl.h"
#include "srsran/du/du_wrapper.h"
#include "srsran/du/du_wrapper_factory.h"
#include "srsran/e2/e2_du_metrics_connector.h"
#include "srsran/pcap/rlc_pcap.h"
#include "srsran/ru/ru_dummy_factory.h"

using namespace srsran;

du_unit multicell_flexible_du_factory::create_flexible_du(const du_unit_dependencies& dependencies)
{
  du_unit du_cmd_wrapper;
  du_cmd_wrapper.e2_metric_connectors = std::make_unique<
      e2_metric_connector_manager<e2_du_metrics_connector, e2_du_metrics_notifier, e2_du_metrics_interface>>(
      config.du_high_cfg.config.cells_cfg.size());

  const du_high_unit_config& du_hi    = config.du_high_cfg.config;
  const du_low_unit_config&  du_lo    = config.du_low_cfg;
  const fapi_unit_config&    fapi_cfg = config.fapi_cfg;

  auto du_cells = generate_du_cell_config(du_hi);

  auto du_impl = std::make_unique<multicell_flexible_du_impl>(du_cells.size());

  std::vector<srs_du::cell_prach_ports_entry> prach_ports;
  std::vector<unsigned>                       max_pusch_per_slot;
  for (const auto& high : du_hi.cells_cfg) {
    prach_ports.push_back(high.cell.prach_cfg.ports);
    max_pusch_per_slot.push_back(high.cell.pusch_cfg.max_puschs_per_slot);
  }

  // Initialize and configure the HAL.
  // :TODO: review it. It should be managed internally in the DU low.
  hal_upper_phy_config du_low_hal_cfg = make_du_low_hal_config_and_dependencies(du_lo, du_cells.size());

  srs_du::du_wrapper_config du_cfg = {};
  static constexpr unsigned du_id  = 0U;
  make_du_low_wrapper_config_and_dependencies(du_cfg.du_low_cfg,
                                              du_lo,
                                              prach_ports,
                                              du_cells,
                                              max_pusch_per_slot,
                                              du_impl->get_upper_ru_dl_rg_adapter(),
                                              du_impl->get_upper_ru_ul_request_adapter(),
                                              *dependencies.workers,
                                              du_id,
                                              du_low_hal_cfg);

  auto cell_services_cfg = fill_du_high_wrapper_config(du_cfg.du_high_cfg,
                                                       du_hi,
                                                       du_id,
                                                       dependencies.workers->get_du_high_executor_mapper(du_id),
                                                       *dependencies.f1c_client_handler,
                                                       *dependencies.f1u_gw,
                                                       *dependencies.timer_mng,
                                                       *dependencies.mac_p,
                                                       *dependencies.rlc_p,
                                                       *dependencies.e2_client_handler,
                                                       *(du_cmd_wrapper.e2_metric_connectors),
                                                       *dependencies.json_sink,
                                                       *dependencies.metrics_notifier);

  du_cmd_wrapper.metrics  = std::move(cell_services_cfg.first);
  du_cmd_wrapper.commands = std::move(cell_services_cfg.second);

  srs_du::du_wrapper_dependencies wrapper_deps;
  wrapper_deps.du_high_deps.sectors.resize(du_hi.cells_cfg.size());

  // FAPI configuration.
  du_cfg.du_high_cfg.fapi.log_level = fapi_cfg.fapi_level;
  if (fapi_cfg.l2_nof_slots_ahead != 0) {
    du_cfg.du_high_cfg.fapi.l2_nof_slots_ahead = fapi_cfg.l2_nof_slots_ahead;
    for (unsigned i = 0, e = du_hi.cells_cfg.size(); i != e; ++i) {
      wrapper_deps.du_high_deps.sectors[i].fapi_executor.emplace(dependencies.workers->fapi_exec[i]);
    }
  }

  auto du_instance = make_du_wrapper(du_cfg, std::move(wrapper_deps));
  report_error_if_not(du_instance, "Invalid Distributed Unit");

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

  // Configure the RU and DU in the dynamic DU.
  du_impl->add_ru(std::move(ru));
  du_impl->add_du(std::move(du_instance));

  du_cmd_wrapper.unit = std::move(du_impl);

  announce_du_high_cells(du_hi);

  return du_cmd_wrapper;
}
