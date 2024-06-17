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

#include "srsran/ngap/ngap.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/manual_event.h"

namespace srsran {

class task_executor;

namespace srs_cu_cp {

class n2_connection_client;

class ngap_connection_handler
{
public:
  ngap_connection_handler(n2_connection_client& client_handler_,
                          ngap_message_handler& rx_pdu_handler_,
                          ngap_cu_cp_notifier&  cu_cp_notifier_,
                          task_executor&        ctrl_exec_);
  ~ngap_connection_handler();

  /// Initiate new TNL association to the AMF via N2 interface.
  /// \return Notifier used by the NGAP to send NGAP PDUs to the N2 gateway.
  std::unique_ptr<ngap_message_notifier> connect_to_amf();

  async_task<void> handle_tnl_association_removal();

  /// \brief Check if the connection is active.
  [[nodiscard]] bool is_connected() const { return connected_flag; }

private:
  // Called by the N2 GW when the N2 TNL association drops.
  void handle_connection_loss();

  // Called from within NGAP execution context to handle a TNL association loss.
  void handle_connection_loss_impl();

  n2_connection_client& client_handler;
  ngap_message_handler& rx_pdu_handler;
  ngap_cu_cp_notifier&  cu_cp_notifier;
  task_executor&        ctrl_exec;
  srslog::basic_logger& logger;

  bool connected_flag{false};

  std::unique_ptr<ngap_message_notifier> tx_pdu_notifier;

  manual_event_flag rx_path_disconnected;
};

} // namespace srs_cu_cp
} // namespace srsran