/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ue_manager.h"
#include "srsgnb/cu_up/cu_up.h"
#include "srsgnb/cu_up/cu_up_configuration.h"
#include "srsgnb/e1/cu_up/e1_cu_up.h"
#include "srsgnb/support/async/async_task_loop.h"
#include "srsgnb/support/executors/task_executor.h"
#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/support/timers.h"
#include <memory>
#include <unordered_map>

namespace srsgnb {
namespace srs_cu_up {

class cu_up final : public cu_up_interface
{
public:
  explicit cu_up(const cu_up_configuration& cfg_);
  ~cu_up() = default;

  // cu_up_e1_interface
  e1_message_handler& get_e1_message_handler() override { return *e1; }
  e1ap_bearer_context_setup_response_message
  handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request_message& msg) override;

  // cu_up_e1_connection_notifier
  void on_e1_connection_establish() override;
  void on_e1_connection_drop() override;
  bool e1_is_connected() override { return e1_connected; }

  // cu_up_ngu_interface
  gtpu_demux_rx_upper_layer_interface& get_ngu_pdu_handler() override { return *ngu_demux; }

private:
  cu_up_configuration cfg;
  timer_manager       timers;

  // Handler for CU-UP tasks.
  async_task_sequencer main_ctrl_loop;

  // logger
  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-UP", false);

  // Components
  std::atomic<bool>             e1_connected = {false};
  std::unique_ptr<e1_interface> e1;
  std::unique_ptr<gtpu_demux>   ngu_demux;
  timer_manager                 timer_db;
  std::unique_ptr<ue_manager>   ue_mng;
};

} // namespace srs_cu_up
} // namespace srsgnb
