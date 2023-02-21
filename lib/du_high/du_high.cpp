/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_high.h"
#include "du_high_executor_strategies.h"
#include "srsran/du_manager/du_manager_factory.h"
#include "srsran/f1ap/du/f1ap_du_factory.h"
#include "srsran/mac/mac_factory.h"

using namespace srsran;
using namespace srs_du;

void assert_du_high_configuration_valid(const du_high_configuration& cfg)
{
  srsran_assert(cfg.du_mng_executor != nullptr, "Invalid DU manager executor");
  srsran_assert(cfg.cell_executors != nullptr, "Invalid CELL executor mapper");
  srsran_assert(cfg.ue_executors != nullptr, "Invalid UE executor mapper");
}

/// Cell slot handler that additionally increments the DU high timers.
class du_high_slot_handler final : public mac_cell_slot_handler
{
public:
  du_high_slot_handler(timer_manager& timers_, mac_interface& mac_) : timers(timers_), mac(mac_) {}
  void handle_slot_indication(slot_point sl_tx) override
  {
    // Step timers by one millisecond.
    if (sl_tx.to_uint() % get_nof_slots_per_subframe(to_subcarrier_spacing(sl_tx.numerology())) == 0) {
      timers.tick_all();
    }

    // Handle slot indication in MAC & Scheduler.
    mac.get_slot_handler(to_du_cell_index(0)).handle_slot_indication(sl_tx);
  }

private:
  timer_manager& timers;
  mac_interface& mac;
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
  f1ap_du_cfg_handler(*config_.timers),
  metrics_notifier(std::make_unique<scheduler_ue_metrics_null_notifier>())
{
  assert_du_high_configuration_valid(cfg);

  // Create layers
  mac  = create_mac(mac_config{*cfg.pcap,
                              mac_ev_notifier,
                              *cfg.ue_executors,
                              *cfg.cell_executors,
                              *cfg.du_mng_executor,
                              *cfg.phy_adapter,
                              cfg.sched_cfg,
                              cfg.metrics_notifier ? *cfg.metrics_notifier : *metrics_notifier});
  f1ap = create_f1ap(*cfg.f1ap_notifier, f1ap_du_cfg_handler, *cfg.du_mng_executor, *cfg.ue_executors);
  du_manager =
      create_du_manager(du_manager_params{{"srsgnb", 1, 1, cfg.cells, cfg.qos},
                                          {timers, *cfg.du_mng_executor, *cfg.ue_executors, *cfg.cell_executors},
                                          {*f1ap, *f1ap},
                                          {*config_.f1u_gw},
                                          {mac->get_ue_control_info_handler(), *f1ap, *f1ap},
                                          {mac->get_cell_manager(), mac->get_ue_configurator(), cfg.sched_cfg}});

  // Connect Layer<->DU manager adapters.
  mac_ev_notifier.connect(*du_manager);
  f1ap_du_cfg_handler.connect(*du_manager);

  // Cell slot handler.
  main_cell_slot_handler = std::make_unique<du_high_slot_handler>(timers, *mac);
}

du_high::~du_high()
{
  stop();
}

void du_high::start()
{
  du_manager->start();
}

void du_high::stop() {}

f1ap_message_handler& du_high::get_f1ap_message_handler()
{
  return *f1ap;
}

mac_pdu_handler& du_high::get_pdu_handler(du_cell_index_t cell_idx)
{
  return mac->get_pdu_handler(cell_idx);
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
