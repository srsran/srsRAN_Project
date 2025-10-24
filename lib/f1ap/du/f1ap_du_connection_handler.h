/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/f1ap/du/f1ap_du_connection_manager.h"
#include "srsran/f1ap/f1ap_message_handler.h"
#include "srsran/f1ap/gateways/f1c_connection_client.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/async/manual_event.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {
namespace srs_du {

class f1ap_du_configurator;
struct f1ap_du_context;

/// \brief Handler of TNL connection between DU and CU-CP.
class f1ap_du_connection_handler
{
public:
  f1ap_du_connection_handler(f1c_connection_client& f1c_client_handler_,
                             f1ap_message_handler&  f1ap_pdu_handler_,
                             f1ap_du_configurator&  du_mng_,
                             f1ap_du_context&       du_ctxt_,
                             task_executor&         ctrl_exec_);
  ~f1ap_du_connection_handler();

  [[nodiscard]] std::unique_ptr<f1ap_message_notifier> connect_to_cu_cp();

  async_task<void> handle_tnl_association_removal();

  /// \brief Check if the connection is active.
  [[nodiscard]] bool is_connected() const { return connected_flag; }

private:
  // Called by the F1-C GW when the F1-C TNL association drops.
  void handle_connection_loss();

  void handle_connection_loss_impl();

  f1c_connection_client& f1c_client_handler;
  f1ap_message_handler&  f1ap_pdu_handler;
  f1ap_du_configurator&  du_mng;
  f1ap_du_context&       du_ctxt;
  task_executor&         ctrl_exec;
  srslog::basic_logger&  logger;

  bool connected_flag{false};

  std::unique_ptr<f1ap_message_notifier> tx_pdu_notifier;
  unsigned                               f1c_session_epoch{0};

  manual_event_flag rx_path_disconnected;
};

} // namespace srs_du
} // namespace srsran
