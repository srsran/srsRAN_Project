/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/du/du_high/du_high.h"
#include "srsran/du/du_high/du_high_configuration.h"
#include "srsran/du/du_high/du_manager/du_manager.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/mac/mac.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include <memory>

namespace srsran {
namespace srs_du {

class du_high_impl final : public du_high
{
public:
  explicit du_high_impl(const du_high_configuration& cfg_, const du_high_dependencies& dependencies);
  ~du_high_impl() override;

  void start() override;

  void stop() override;

  f1ap_du& get_f1ap_du() override;

  mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index) override;

  mac_cell_rach_handler& get_rach_handler(du_cell_index_t cell_index) override;

  mac_pdu_handler& get_pdu_handler() override;

  mac_cell_control_information_handler& get_control_info_handler(du_cell_index_t cell_index) override;

  du_configurator& get_du_configurator() override;

private:
  class layer_connector;

  du_high_configuration cfg;

  srslog::basic_logger& logger;

  timer_manager& timers;

  std::atomic<bool> is_running{true};

  // Connection between DU-high layers.
  std::unique_ptr<layer_connector> adapters;

  std::unique_ptr<scheduler_metrics_notifier> hub_metrics;

  // DU-high Layers.
  std::unique_ptr<du_manager_interface> du_manager;
  std::unique_ptr<f1ap_du>              f1ap;
  std::unique_ptr<mac_interface>        mac;

  std::unique_ptr<mac_cell_slot_handler> main_cell_slot_handler;
};

} // namespace srs_du
} // namespace srsran
