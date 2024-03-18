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

#include "adapters/e1ap_adapters.h"
#include "adapters/gtpu_adapters.h"
#include "adapters/gw_adapters.h"
#include "ue_manager.h"
#include "srsran/cu_up/cu_up.h"
#include "srsran/cu_up/cu_up_configuration.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"
#include "srsran/gateways/udp_network_gateway.h"
#include "srsran/gtpu/gtpu_echo.h"
#include "srsran/gtpu/gtpu_teid_pool.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"
#include "srsran/support/executors/task_executor.h"
#include <memory>
#include <unordered_map>

namespace srsran {
namespace srs_cu_up {

class cu_up final : public cu_up_interface
{
public:
  explicit cu_up(const cu_up_configuration& cfg_);
  ~cu_up() override;

  // cu_up_interface
  void start() override;
  void stop() override;

  optional<uint16_t> get_n3_bind_port() override { return ngu_gw->get_bind_port(); }

  // cu_up_e1ap_interface
  e1ap_message_handler& get_e1ap_message_handler() override { return *e1ap; }

  e1ap_bearer_context_setup_response
  handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg) override;

  e1ap_bearer_context_modification_response
  handle_bearer_context_modification_request(const e1ap_bearer_context_modification_request& msg) override;

  void handle_bearer_context_release_command(const e1ap_bearer_context_release_command& msg) override;

  // cu_up_e1ap_connection_notifier
  void on_e1ap_connection_establish() override;
  void on_e1ap_connection_drop() override;
  bool e1ap_is_connected() override { return e1ap_connected; }

  // cu_up_ngu_interface
  gtpu_demux_rx_upper_layer_interface& get_ngu_pdu_handler() override { return *ngu_demux; }

private:
  void on_statistics_report_timer_expired();

  cu_up_configuration cfg;

  // logger
  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-UP", false);

  // Holds DL executor for the control TEID.
  std::unique_ptr<ue_executor_mapper> ctrl_exec_mapper;

  // Components
  std::atomic<bool>                    e1ap_connected = {false};
  std::unique_ptr<e1ap_interface>      e1ap;
  std::unique_ptr<udp_network_gateway> ngu_gw;
  std::unique_ptr<gtpu_demux>          ngu_demux;
  std::unique_ptr<gtpu_echo>           ngu_echo;
  std::unique_ptr<gtpu_teid_pool>      f1u_teid_allocator;
  std::unique_ptr<ue_manager>          ue_mng;

  // Adapters
  network_gateway_data_gtpu_demux_adapter gw_data_gtpu_demux_adapter;
  gtpu_network_gateway_adapter            gtpu_gw_adapter;
  e1ap_cu_up_adapter                      e1ap_cu_up_ev_notifier;

  std::mutex mutex;
  bool       running{false};

  // Handler for CU-UP tasks.
  fifo_async_task_scheduler main_ctrl_loop;

  unique_timer statistics_report_timer;
};

} // namespace srs_cu_up
} // namespace srsran
