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

#include "adapters/e1ap_adapters.h"
#include "adapters/gtpu_adapters.h"
#include "adapters/gw_adapters.h"
#include "ue_manager.h"
#include "srsgnb/cu_up/cu_up.h"
#include "srsgnb/cu_up/cu_up_configuration.h"
#include "srsgnb/e1/cu_up/e1ap_cu_up.h"
#include "srsgnb/gateways/udp_network_gateway.h"
#include "srsgnb/support/async/async_task_loop.h"
#include "srsgnb/support/executors/task_executor.h"
#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/support/io_broker/io_broker.h"
#include "srsgnb/support/timers.h"
#include <memory>
#include <unordered_map>

namespace srsgnb {
namespace srs_cu_up {

class cu_up final : public cu_up_interface
{
public:
  explicit cu_up(const cu_up_configuration& cfg_);
  ~cu_up();

  // cu_up_interface
  int get_n3_bind_port() override
  {
    uint16_t port = {};
    ngu_gw->get_bind_port(port);
    return port;
  }

  // cu_up_e1ap_interface
  e1ap_message_handler& get_e1ap_message_handler() override { return *e1ap; }

  cu_cp_e1_setup_response handle_cu_cp_e1_setup_request(const cu_cp_e1_setup_request& msg) override;

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
  cu_up_configuration cfg;
  timer_manager       timers;

  // Handler for CU-UP tasks.
  async_task_sequencer main_ctrl_loop;

  // logger
  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-UP", false);

  // Components
  std::atomic<bool>                    e1ap_connected = {false};
  std::unique_ptr<e1ap_interface>      e1ap;
  std::unique_ptr<udp_network_gateway> ngu_gw;
  std::unique_ptr<gtpu_demux>          ngu_demux;
  timer_manager                        timer_db;
  std::unique_ptr<ue_manager>          ue_mng;

  // Adapters
  network_gateway_data_gtpu_demux_adapter gw_data_gtpu_demux_adapter;
  gtpu_network_gateway_adapter            gtpu_gw_adapter;
  e1ap_cu_up_adapter                      e1ap_cu_up_ev_notifier;
};

} // namespace srs_cu_up
} // namespace srsgnb
