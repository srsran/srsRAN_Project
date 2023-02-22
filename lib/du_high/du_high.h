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

#pragma once

#include "adapters.h"
#include "f1ap_adapters.h"
#include "srsran/du_high/du_high_configuration.h"
#include "srsran/du_manager/du_manager.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/mac/mac.h"
#include "srsran/mac/mac_cell_result.h"
#include "srsran/rlc/rlc_rx.h"
#include "srsran/rlc/rlc_tx.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/timers.h"
#include <memory>

namespace srsran {
namespace srs_du {

class du_high
{
public:
  explicit du_high(const du_high_configuration& cfg_);
  ~du_high();

  void start();
  void stop();

  f1ap_message_handler& get_f1ap_message_handler();

  mac_pdu_handler& get_pdu_handler(du_cell_index_t cell_index);

  mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index);

  mac_cell_rach_handler& get_rach_handler(du_cell_index_t cell_index);

  mac_cell_control_information_handler& get_control_information_handler(du_cell_index_t cell_index);

private:
  du_high_configuration cfg;

  timer_manager& timers;

  f1ap_du_configurator_adapter f1ap_du_cfg_handler;

  std::unique_ptr<scheduler_ue_metrics_notifier> metrics_notifier;

  std::unique_ptr<du_manager_interface> du_manager;
  std::unique_ptr<f1ap_interface>       f1ap;
  std::unique_ptr<mac_interface>        mac;

  std::unique_ptr<mac_cell_slot_handler> main_cell_slot_handler;

  du_manager_mac_event_indicator mac_ev_notifier;
};

} // namespace srs_du
} // namespace srsran
