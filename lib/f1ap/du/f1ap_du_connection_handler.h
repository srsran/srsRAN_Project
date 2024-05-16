/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/f1ap/du/f1ap_du_connection_manager.h"
#include "srsran/f1ap/du/f1c_connection_client.h"
#include "srsran/support/async/manual_event.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {
namespace srs_du {

class f1ap_du_configurator;

/// \brief Handler of TNL connection between DU and CU-CP.
class f1ap_du_connection_handler
{
public:
  f1ap_du_connection_handler(f1c_connection_client& f1c_client_handler_,
                             f1ap_message_handler&  f1ap_pdu_handler_,
                             f1ap_du_configurator&  du_mng_,
                             task_executor&         ctrl_exec_);
  ~f1ap_du_connection_handler();

  SRSRAN_NODISCARD std::unique_ptr<f1ap_message_notifier> connect_to_cu_cp();

  async_task<void> handle_tnl_association_removal();

  /// \brief Check if the connection is active.
  SRSRAN_NODISCARD bool is_connected() const { return connected_flag; }

private:
  // Called by the F1-C GW when the F1-C TNL association drops.
  void handle_connection_loss();

  void handle_connection_loss_impl();

  f1c_connection_client& f1c_client_handler;
  f1ap_message_handler&  f1ap_pdu_handler;
  f1ap_du_configurator&  du_mng;
  task_executor&         ctrl_exec;
  srslog::basic_logger&  logger;

  bool connected_flag{false};

  std::unique_ptr<f1ap_message_notifier> tx_pdu_notifier;

  manual_event_flag rx_path_disconnected;
};

} // namespace srs_du
} // namespace srsran