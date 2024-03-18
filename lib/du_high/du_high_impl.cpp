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

#include "du_high_impl.h"
#include "adapters/adapters.h"
#include "adapters/du_high_adapter_factories.h"
#include "adapters/f1ap_adapters.h"
#include "du_high_executor_strategies.h"
#include "srsran/du_manager/du_manager_factory.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2_factory.h"
#include "srsran/f1ap/du/f1ap_du_factory.h"
#include "srsran/mac/mac_factory.h"
#include "srsran/support/executors/task_redispatcher.h"
#include "srsran/support/timers.h"

using namespace srsran;
using namespace srs_du;

/// \brief This class is responsible for providing the necessary adapters to connect layers in the DU-high.
class du_high_impl::layer_connector
{
public:
  explicit layer_connector(const du_high_configuration& config_) :
    f1_to_du_notifier(timer_factory{*config_.timers, config_.exec_mapper->du_control_executor()})
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

class scheduler_ue_metrics_null_notifier final : public scheduler_ue_metrics_notifier
{
public:
  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override
  {
    // do nothing.
  }
};

du_high_impl::du_high_impl(const du_high_configuration& config_) :
  cfg(config_),
  logger(srslog::fetch_basic_logger("DU")),
  timers(*config_.timers),
  adapters(std::make_unique<layer_connector>(config_)),
  metrics_notifier(std::make_unique<scheduler_ue_metrics_null_notifier>())
{
  // Create layers
  mac =
      create_du_high_mac(mac_config{adapters->mac_ev_notifier,
                                    cfg.exec_mapper->ue_mapper(),
                                    cfg.exec_mapper->cell_mapper(),
                                    cfg.exec_mapper->du_control_executor(),
                                    *cfg.phy_adapter,
                                    cfg.mac_cfg,
                                    *cfg.mac_p,
                                    cfg.sched_cfg,
                                    cfg.sched_ue_metrics_notifier ? *cfg.sched_ue_metrics_notifier : *metrics_notifier},
                         cfg.test_cfg);
  f1ap       = create_f1ap(*cfg.f1c_client,
                     adapters->f1_to_du_notifier,
                     cfg.exec_mapper->du_control_executor(),
                     cfg.exec_mapper->ue_mapper(),
                     adapters->f1ap_paging_notifier);
  du_manager = create_du_manager(du_manager_params{
      {cfg.gnb_du_name, cfg.gnb_du_id, 1, cfg.du_bind_addr, cfg.cells, cfg.srbs, cfg.qos},
      {timers, cfg.exec_mapper->du_control_executor(), cfg.exec_mapper->ue_mapper(), cfg.exec_mapper->cell_mapper()},
      {*f1ap, *f1ap},
      {*config_.f1u_gw},
      {mac->get_ue_control_info_handler(), *f1ap, *f1ap, *cfg.rlc_p, cfg.rlc_metrics_notif},
      {mac->get_cell_manager(), mac->get_ue_configurator(), cfg.sched_cfg}});

  // Connect Layer<->DU manager adapters.
  adapters->connect(*du_manager, *mac);

  // Cell slot handler.
  main_cell_slot_handler = std::make_unique<du_high_slot_handler>(timers, *mac, cfg.exec_mapper->du_timer_executor());

  if (cfg.e2_client) {
    // todo: subscribe e2_metric_manager to a metric hub (currently not present)
    e2ap_entity = create_e2_entity(cfg.e2ap_config,
                                   cfg.e2_client,
                                   *cfg.e2_du_metric_iface,
                                   *f1ap,
                                   get_du_configurator(),
                                   timer_factory{timers, cfg.exec_mapper->du_e2_executor()},
                                   cfg.exec_mapper->du_e2_executor());
  }
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

  if (e2ap_entity) {
    e2ap_entity->start();
  }

  // If test mode is enabled, create a test-mode UE by injecting a Msg3.
  if (cfg.test_cfg.test_ue.has_value()) {
    // Push an UL-CCCH message that will trigger the creation of a UE for testing purposes.
    for (unsigned ue_num = 0, nof_ues = cfg.test_cfg.test_ue->nof_ues; ue_num != nof_ues; ++ue_num) {
      auto rx_buf = byte_buffer::create({0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6, 0x06, 0x3f, 0x00, 0x00, 0x00});
      if (rx_buf.is_error()) {
        logger.warning("Unable to allocate byte_buffer");
        continue;
      }
      mac->get_pdu_handler().handle_rx_data_indication(mac_rx_data_indication{
          slot_point{0, 0},
          to_du_cell_index(0),
          {mac_rx_pdu{to_rnti(to_value(cfg.test_cfg.test_ue->rnti) + ue_num), 0, 0, std::move(rx_buf.value())}}});
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
  if (e2ap_entity) {
    e2ap_entity->stop();
  }
  logger.info("DU-High stopped successfully");
}

f1ap_message_handler& du_high_impl::get_f1ap_message_handler()
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
