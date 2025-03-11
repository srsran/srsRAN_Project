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

#include "srsran/e2/e2.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/manual_event.h"

namespace srsran {

class task_executor;

class e2_connection_client;

class e2_connection_handler
{
public:
  e2_connection_handler(e2_connection_client&  client_handler_,
                        e2_message_handler&    rx_pdu_handler_,
                        e2ap_e2agent_notifier& agent_notifier_,
                        task_executor&         ctrl_exec_);
  ~e2_connection_handler();

  /// Initiate new TNL association to the Near-RT RIC via E2 interface.
  /// \return Notifier used by the E2AP to E2AP NGAP PDUs to the E2 gateway.
  std::unique_ptr<e2_message_notifier> connect_to_ric();

  async_task<void> handle_tnl_association_removal();

  /// \brief Check if the connection is active.
  [[nodiscard]] bool is_connected() const { return connected_flag; }

private:
  // Called by the E2 GW when the E2 TNL association drops.
  void handle_connection_loss();

  // Called from within E2AP execution context to handle a TNL association loss.
  void handle_connection_loss_impl();

  e2_connection_client&  client_handler;
  e2_message_handler&    rx_pdu_handler;
  e2ap_e2agent_notifier& agent_notifier;
  task_executor&         ctrl_exec;
  srslog::basic_logger&  logger;

  bool connected_flag{false};

  std::unique_ptr<e2_message_notifier> tx_pdu_notifier;

  manual_event_flag rx_path_disconnected;
};

} // namespace srsran
