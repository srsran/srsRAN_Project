/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

class f1ap_du_connection_handler : public f1ap_message_notifier
{
public:
  f1ap_du_connection_handler(f1c_connection_client& f1c_client_handler_, f1ap_message_handler& f1ap_pdu_handler_);

  SRSRAN_NODISCARD bool connect_to_cu_cp();
  SRSRAN_NODISCARD bool is_connected() const { return f1ap_notifier != nullptr; }

  void on_new_message(const srsran::f1ap_message& msg) override;

private:
  f1c_connection_client& f1c_client_handler;
  f1ap_message_handler&  f1ap_pdu_handler;
  srslog::basic_logger&  logger;

  std::unique_ptr<f1ap_message_notifier> f1ap_notifier;
};

} // namespace srs_du
} // namespace srsran