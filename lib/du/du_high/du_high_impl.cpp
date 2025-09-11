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

#include "du_high_impl.h"
#include "adapters/adapters.h"
#include "adapters/du_high_adapter_factories.h"
#include "adapters/f1ap_adapters.h"
#include "test_mode/f1ap_test_mode_adapter.h"
#include "srsran/du/du_high/du_high_clock_controller.h"
#include "srsran/du/du_high/du_manager/du_manager_factory.h"
#include "srsran/mac/mac_cell_timing_context.h"
#include "srsran/mac/mac_metrics_notifier.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/timers.h"

using namespace srsran;
using namespace srs_du;

/// \brief This class is responsible for providing the necessary adapters to connect layers in the DU-high.
class du_high_impl::layer_connector
{
public:
  explicit layer_connector(timer_manager& timers, task_executor& executor) :
    f1_to_du_notifier(timer_factory{timers, executor})
  {
  }

  /// Connect layers of the DU-high.
  void connect(du_manager_interface& du_mng, mac_interface& mac_inst)
  {
    mac_ev_notifier.connect(du_mng, du_mng.get_metrics_aggregator());
    f1_to_du_notifier.connect(du_mng);
    f1ap_paging_notifier.connect(mac_inst.get_cell_paging_info_handler());
  }

  /// Notifier MAC -> DU manager.
  du_manager_mac_event_indicator mac_ev_notifier;

  /// Notifier F1AP -> DU manager
  f1ap_du_configurator_adapter f1_to_du_notifier;

  /// Notifier F1AP -> MAC for paging PDUs.
  mac_f1ap_paging_handler f1ap_paging_notifier;
};

du_high_impl::du_high_impl(const du_high_configuration& config_, const du_high_dependencies& dependencies) :
  cfg(config_),
  logger(srslog::fetch_basic_logger("DU")),
  timers(dependencies.timer_ctrl->get_timer_manager()),
  adapters(std::make_unique<layer_connector>(timers, dependencies.exec_mapper->du_control_executor()))
{
  // Create layers
  mac  = create_du_high_mac(mac_config{adapters->mac_ev_notifier,
                                      dependencies.exec_mapper->ue_mapper(),
                                      dependencies.exec_mapper->cell_mapper(),
                                      dependencies.exec_mapper->du_control_executor(),
                                      *dependencies.phy_adapter,
                                      cfg.ran.mac_cfg,
                                      *dependencies.mac_p,
                                      *dependencies.timer_ctrl,
                                      mac_config::metrics_config{cfg.metrics.period,
                                                                 cfg.metrics.enable_mac,
                                                                 cfg.metrics.enable_sched,
                                                                 cfg.metrics.max_nof_sched_ue_events,
                                                                 adapters->mac_ev_notifier},
                                      cfg.ran.sched_cfg},
                           cfg.test_cfg,
                           cfg.ran.cells.size());
  f1ap = create_du_high_f1ap(*dependencies.f1c_client,
                             adapters->f1_to_du_notifier,
                             dependencies.exec_mapper->du_control_executor(),
                             dependencies.exec_mapper->ue_mapper(),
                             adapters->f1ap_paging_notifier,
                             timers,
                             cfg.test_cfg);

  du_manager = create_du_manager(du_manager_params{
      {cfg.ran.gnb_du_name, cfg.ran.gnb_du_id, 1, cfg.ran.cells, cfg.ran.srbs, cfg.ran.qos},
      {timers,
       dependencies.exec_mapper->du_control_executor(),
       dependencies.exec_mapper->ue_mapper(),
       dependencies.exec_mapper->cell_mapper()},
      {*f1ap, *f1ap, f1ap->get_metrics_collector()},
      {*dependencies.f1u_gw},
      {mac->get_ue_control_info_handler(), *f1ap, *f1ap, *dependencies.rlc_p, dependencies.rlc_metrics_notif},
      {mac->get_cell_manager(), mac->get_ue_configurator(), cfg.ran.sched_cfg, nullptr},
      {cfg.metrics.period,
       dependencies.du_notifier,
       cfg.metrics.enable_f1ap,
       cfg.metrics.enable_mac,
       cfg.metrics.enable_sched},
      cfg.test_cfg});

  // Connect Layer<->DU manager adapters.
  adapters->connect(*du_manager, *mac);
}

du_high_impl::~du_high_impl()
{
  stop();
}

void du_high_impl::start()
{
  logger.info("Starting DU-High...");
  du_manager->start();
  logger.info("DU-High started successfully");
}

void du_high_impl::stop()
{
  if (not is_running.exchange(false, std::memory_order::memory_order_relaxed)) {
    return;
  }

  logger.info("Stopping DU-High...");
  du_manager->stop();
  logger.info("DU-High stopped successfully");
}

f1ap_du& du_high_impl::get_f1ap_du()
{
  return *f1ap;
}

mac_pdu_handler& du_high_impl::get_pdu_handler()
{
  return mac->get_pdu_handler();
}

mac_cell_slot_handler& du_high_impl::get_slot_handler(du_cell_index_t cell_idx)
{
  return mac->get_slot_handler(cell_idx);
}

mac_cell_rach_handler& du_high_impl::get_rach_handler(du_cell_index_t cell_index)
{
  return mac->get_rach_handler(cell_index);
}

mac_cell_control_information_handler& du_high_impl::get_control_info_handler(du_cell_index_t cell_index)
{
  return mac->get_control_info_handler(cell_index);
}

du_configurator& du_high_impl::get_du_configurator()
{
  return *du_manager;
}

du_manager_time_mapper_accessor& du_high_impl::get_du_manager_time_mapper_accessor()
{
  return *du_manager;
}
