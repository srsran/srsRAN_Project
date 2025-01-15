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

#include "srsran/e1ap/cu_up/e1ap_cu_up.h"
#include "srsran/e1ap/gateways/e1_connection_client.h"

namespace srsran {
namespace srs_cu_up {

class e1ap_cu_up_connection_handler
{
public:
  e1ap_cu_up_connection_handler(e1_connection_client& e1ap_client_handler_, e1ap_message_handler& e1ap_pdu_handler_);

  [[nodiscard]] e1ap_message_notifier* connect_to_cu_cp();
  [[nodiscard]] bool                   is_connected() const { return e1ap_notifier != nullptr; }

private:
  e1_connection_client& e1_client_handler;
  e1ap_message_handler& e1ap_pdu_handler;
  srslog::basic_logger& logger;

  std::unique_ptr<e1ap_message_notifier> e1ap_notifier;
};

} // namespace srs_cu_up
} // namespace srsran
