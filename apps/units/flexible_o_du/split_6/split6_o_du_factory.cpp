/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split6_o_du_factory.h"
#include "apps/helpers/e2/e2_metric_connector_manager.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/units/flexible_o_du/flexible_o_du_commands.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_translators.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_factory.h"
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

  o_du_high_unit_dependencies odu_hi_unit_dependencies = {du_dependencies.workers->get_du_high_executor_mapper(0),
                                                          *du_dependencies.f1c_client_handler,
                                                          *du_dependencies.f1u_gw,
                                                          *du_dependencies.timer_mng,
                                                          *du_dependencies.mac_p,
                                                          *du_dependencies.rlc_p,
                                                          *du_dependencies.e2_client_handler,
                                                          *(odu_unit.e2_metric_connectors),
                                                          *du_dependencies.metrics_notifier,
                                                          {}};

  // Adjust the dependencies.
  const auto& du_hi_unit_cfg = du_unit_cfg.odu_high_cfg.du_high_cfg.config;
  for (unsigned i = 0, e = du_hi_unit_cfg.cells_cfg.size(); i != e; ++i) {
    auto& sector_dependencies   = odu_hi_unit_dependencies.o_du_hi_dependencies.sectors.emplace_back();
    sector_dependencies.gateway = &fapi_adaptors[i]->get_message_interface_collection().get_slot_message_gateway();
    sector_dependencies.last_msg_notifier =
        &fapi_adaptors[i]->get_message_interface_collection().get_slot_last_message_notifier();
    sector_dependencies.fapi_executor = du_unit_cfg.odu_high_cfg.fapi_cfg.l2_nof_slots_ahead != 0
                                            ? std::optional(du_dependencies.workers->fapi_exec[i])
                                            : std::make_optional<task_executor*>();
  }

  o_du_high_unit odu_hi_unit = make_o_du_high_unit(du_unit_cfg.odu_high_cfg, std::move(odu_hi_unit_dependencies));

  odu_unit.commands = std::move(odu_hi_unit.commands);
  odu_unit.metrics  = std::move(odu_hi_unit.metrics);

  // Create the DU.
  odu_unit.unit = std::make_unique<split6_o_du_impl>(std::move(fapi_adaptors), std::move(odu_hi_unit.o_du_hi));
  report_error_if_not(odu_unit.unit, "Invalid Distributed Unit");

  announce_du_high_cells(du_hi_unit_cfg);

  return odu_unit;
}
