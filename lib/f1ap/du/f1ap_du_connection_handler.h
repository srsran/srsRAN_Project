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

namespace srsran {
namespace srs_du {

/// \brief Handler of TNL connection between DU and CU-CP.
class f1ap_du_connection_handler
{
public:
  f1ap_du_connection_handler(f1c_connection_client& f1c_client_handler_, f1ap_message_handler& f1ap_pdu_handler_);

  SRSRAN_NODISCARD std::unique_ptr<f1ap_message_notifier> connect_to_cu_cp();

  /// \brief Check if the connection is active.
  SRSRAN_NODISCARD bool is_connected() const { return connected; }

private:
  f1c_connection_client& f1c_client_handler;
  f1ap_message_handler&  f1ap_pdu_handler;
  srslog::basic_logger&  logger;

  bool connected = false;
};

} // namespace srs_du
} // namespace srsran