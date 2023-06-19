/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "du_high.h"
#include "du_high_executor_strategies.h"
#include "mac_test_mode_adapter.h"
#include "srsran/du_manager/du_manager_factory.h"
#include "srsran/f1ap/du/f1ap_du_factory.h"
#include "srsran/mac/mac_factory.h"
#include "srsran/support/timers.h"

using namespace srsran;
using namespace srs_du;

/// Cell slot handler that additionally increments the DU high timers.
class du_high_slot_handler final : public mac_cell_slot_handler
{
public:
  du_high_slot_handler(timer_manager& timers_, mac_interface& mac_, task_executor& tick_exec_) :
    timers(timers_), mac(mac_), tick_exec(tick_exec_)
  {
  }
  void handle_slot_indication(slot_point sl_tx) override
  {
    // Step timers by one millisecond.
    if (sl_tx.to_uint() % get_nof_slots_per_subframe(to_subcarrier_spacing(sl_tx.numerology())) == 0) {
      // The timer tick is handled in a separate execution context.
      tick_exec.execute([this]() { timers.tick(); });
    }

    // Handle slot indication in MAC & Scheduler.
    mac.get_slot_handler(to_du_cell_index(0)).handle_slot_indication(sl_tx);
  }

private:
  timer_manager& timers;
  mac_interface& mac;
  task_executor& tick_exec;
};

class scheduler_ue_metrics_null_notifier final : public scheduler_ue_metrics_notifier
{
public:
  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override
  {
    // do nothing.
  }
};

du_high::du_high(const du_high_configuration& config_) :
  cfg(config_),
  timers(*config_.timers),
  f1ap_du_cfg_handler(timer_factory{*config_.timers, config_.exec_mapper->du_control_executor()}),
  metrics_notifier(std::make_unique<scheduler_ue_metrics_null_notifier>())
{
  // Create layers
  mac = create_mac(mac_config{mac_ev_notifier,
                              cfg.exec_mapper->ue_mapper(),
                              cfg.exec_mapper->cell_mapper(),
                              cfg.exec_mapper->du_control_executor(),
                              *cfg.phy_adapter,
                              cfg.mac_cfg,
                              *cfg.pcap,
                              cfg.sched_cfg,
                              cfg.metrics_notifier ? *cfg.metrics_notifier : *metrics_notifier});
  if (cfg.test_cfg.test_ue.has_value()) {
    mac = std::make_unique<mac_test_mode_adapter>(std::move(mac), *cfg.test_cfg.test_ue);
  }

  f1ap       = create_f1ap(*cfg.f1ap_notifier,
                     f1ap_du_cfg_handler,
                     cfg.exec_mapper->du_control_executor(),
                     cfg.exec_mapper->ue_mapper(),
                     f1ap_paging_notifier);
  du_manager = create_du_manager(du_manager_params{
      {"srsgnb", 1, 1, cfg.cells, cfg.qos},
      {timers, cfg.exec_mapper->du_control_executor(), cfg.exec_mapper->ue_mapper(), cfg.exec_mapper->cell_mapper()},
      {*f1ap, *f1ap},
      {*config_.f1u_gw},
      {mac->get_ue_control_info_handler(), *f1ap, *f1ap},
      {mac->get_cell_manager(), mac->get_ue_configurator(), cfg.sched_cfg}});

  // Connect Layer<->DU manager adapters.
  mac_ev_notifier.connect(*du_manager);
  f1ap_du_cfg_handler.connect(*du_manager);
  f1ap_paging_notifier.connect(mac->get_cell_paging_info_handler());

  // Cell slot handler.
  main_cell_slot_handler = std::make_unique<du_high_slot_handler>(timers, *mac, cfg.exec_mapper->du_timer_executor());

  // If test mode is enabled.
  if (cfg.test_cfg.test_ue.has_value()) {
    // Push an UL-CCCH message that will trigger the creation of a UE for testing purposes.
    mac->get_pdu_handler().handle_rx_data_indication(mac_rx_data_indication{
        slot_point{0, 0},
        to_du_cell_index(0),
        {mac_rx_pdu{
            cfg.test_cfg.test_ue->rnti, 0, 0, {0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6, 0x06, 0x3f, 0x00, 0x00, 0x00}}}});
  }
}

du_high::~du_high()
{
  stop();
}

void du_high::start()
{
  du_manager->start();
}

void du_high::stop()
{
  du_manager->stop();
}

f1ap_message_handler& du_high::get_f1ap_message_handler()
{
  return *f1ap;
}

mac_pdu_handler& du_high::get_pdu_handler()
{
  return mac->get_pdu_handler();
}

mac_cell_slot_handler& du_high::get_slot_handler(du_cell_index_t cell_idx)
{
  if (cell_idx == 0) {
    return *main_cell_slot_handler;
  }
  return mac->get_slot_handler(cell_idx);
}

mac_cell_rach_handler& du_high::get_rach_handler(du_cell_index_t cell_index)
{
  return mac->get_rach_handler(cell_index);
}

mac_cell_control_information_handler& du_high::get_control_information_handler(du_cell_index_t cell_index)
{
  return mac->get_control_info_handler(cell_index);
}
