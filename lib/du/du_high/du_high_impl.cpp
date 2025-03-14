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
#include "srsran/du/du_high/du_manager/du_manager_factory.h"
#include "srsran/f1ap/du/f1ap_du_factory.h"
#include "srsran/mac/mac_metrics_notifier.h"
#include "srsran/support/executors/task_redispatcher.h"
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
    mac_ev_notifier.connect(du_mng);
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

/// Cell slot handler that additionally increments the DU high timers.
class du_high_slot_handler final : public mac_cell_slot_handler
{
public:
  du_high_slot_handler(timer_manager& timers_, mac_interface& mac_, task_executor& tick_exec_) :
    timers(timers_),
    mac(mac_),
    tick_exec(tick_exec_, [this]() { timers.tick(); }),
    logger(srslog::fetch_basic_logger("MAC"))
  {
  }
  void handle_slot_indication(slot_point sl_tx) override
  {
    // Step timers by one millisecond.
    if (sl_tx.to_uint() % get_nof_slots_per_subframe(to_subcarrier_spacing(sl_tx.numerology())) == 0) {
      // The timer tick is handled in a separate execution context.
      if (not tick_exec.defer()) {
        logger.info("Discarding timer tick={} due to full queue. Retrying later...", sl_tx);
      }
    }

    // Handle slot indication in MAC & Scheduler.
    mac.get_slot_handler(to_du_cell_index(0)).handle_slot_indication(sl_tx);
  }

  void handle_error_indication(slot_point sl_tx, error_event event) override
  {
    mac.get_slot_handler(to_du_cell_index(0)).handle_error_indication(sl_tx, event);
  }

private:
  timer_manager&                    timers;
  mac_interface&                    mac;
  task_redispatcher<task_executor&> tick_exec;
  srslog::basic_logger&             logger;
};

class scheduler_ue_metrics_null_notifier final : public scheduler_metrics_notifier
{
public:
  void report_metrics(const scheduler_cell_metrics& metrics) override
  {
    // do nothing.
  }
};

class mac_metrics_null_notifier final : public mac_metrics_notifier
{
public:
  void on_new_metrics_report(const mac_metric_report& metrics) override
  {
    // do nothing.
  }
};

du_high_impl::du_high_impl(const du_high_configuration& config_, const du_high_dependencies& dependencies) :
  cfg(config_),
  logger(srslog::fetch_basic_logger("DU")),
  timers(*dependencies.timers),
  adapters(std::make_unique<layer_connector>(*dependencies.timers, dependencies.exec_mapper->du_control_executor())),
  sched_metrics_notifier(
      dependencies.sched_ue_metrics_notifier ? nullptr : std::make_unique<scheduler_ue_metrics_null_notifier>()),
  mac_metrics_notif(dependencies.mac_metrics_notif ? nullptr : std::make_unique<mac_metrics_null_notifier>())
{
  // Create layers
  mac = create_du_high_mac(
      mac_config{adapters->mac_ev_notifier,
                 dependencies.exec_mapper->ue_mapper(),
                 dependencies.exec_mapper->cell_mapper(),
                 dependencies.exec_mapper->du_control_executor(),
                 *dependencies.phy_adapter,
                 cfg.ran.mac_cfg,
                 *dependencies.mac_p,
                 timers,
                 dependencies.mac_metrics_notif ? *dependencies.mac_metrics_notif : *mac_metrics_notif,
                 cfg.ran.sched_cfg,
                 dependencies.sched_ue_metrics_notifier ? *dependencies.sched_ue_metrics_notifier
                                                        : *sched_metrics_notifier},
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
      {*f1ap, *f1ap},
      {*dependencies.f1u_gw},
      {mac->get_ue_control_info_handler(), *f1ap, *f1ap, *dependencies.rlc_p, dependencies.rlc_metrics_notif},
      {mac->get_cell_manager(), mac->get_ue_configurator(), cfg.ran.sched_cfg},
      cfg.test_cfg});

  // Connect Layer<->DU manager adapters.
  adapters->connect(*du_manager, *mac);

  // Cell slot handler.
  main_cell_slot_handler =
      std::make_unique<du_high_slot_handler>(timers, *mac, dependencies.exec_mapper->du_timer_executor());
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

  // If test mode is enabled, create a test-mode UE by injecting a Msg3.
  if (cfg.test_cfg.test_ue.has_value()) {
    for (unsigned cell_id = 0, cell_end = cfg.ran.cells.size(); cell_id != cell_end; ++cell_id) {
      if (not cfg.ran.cells[cell_id].enabled) {
        // Skip cells not enabled at startup.
        continue;
      }
      // Push an UL-CCCH message that will trigger the creation of a UE for testing purposes.
      for (unsigned ue_num = 0, nof_ues = cfg.test_cfg.test_ue->nof_ues; ue_num != nof_ues; ++ue_num) {
        auto rx_buf = byte_buffer::create({0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6, 0x06, 0x3f, 0x00, 0x00, 0x00});
        if (not rx_buf.has_value()) {
          logger.warning("Unable to allocate byte_buffer");
          continue;
        }
        mac->get_pdu_handler().handle_rx_data_indication(mac_rx_data_indication{
            slot_point{0, 0},
            to_du_cell_index(cell_id),
            {mac_rx_pdu{to_rnti(to_value(cfg.test_cfg.test_ue->rnti) + (cell_id * nof_ues) + ue_num),
                        0,
                        0,
                        std::move(rx_buf.value())}}});
      }
    }
  }
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
  if (cell_idx == 0) {
    return *main_cell_slot_handler;
  }
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
