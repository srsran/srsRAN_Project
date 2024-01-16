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

#pragma once

#include "srsran/du_high/du_high.h"
#include "srsran/du_high/du_high_configuration.h"
#include "srsran/du_manager/du_manager.h"
#include "srsran/e2/e2.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/mac/mac.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include <memory>

namespace srsran {
namespace srs_du {

class du_high_impl final : public du_high
{
public:
  explicit du_high_impl(const du_high_configuration& cfg_);
  ~du_high_impl();

  void start() override;

  void stop() override;

  f1ap_message_handler& get_f1ap_message_handler() override;

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

  std::unique_ptr<scheduler_ue_metrics_notifier> hub_metrics;
  std::unique_ptr<scheduler_ue_metrics_notifier> metrics_notifier;

  // DU-high Layers.
  std::unique_ptr<du_manager_interface> du_manager;
  std::unique_ptr<f1ap_du>              f1ap;
  std::unique_ptr<mac_interface>        mac;

  std::unique_ptr<mac_cell_slot_handler> main_cell_slot_handler;

  // E2 interface
  std::unique_ptr<e2_interface> e2ap_entity;
};

} // namespace srs_du
} // namespace srsran
