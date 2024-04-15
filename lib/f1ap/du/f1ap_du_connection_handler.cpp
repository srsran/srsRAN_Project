/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_du_connection_handler.h"

using namespace srsran;
using namespace srs_du;

namespace {

/// Adapter used to convert F1AP Rx PDUs coming from the CU-CP into F1AP messages.
class f1ap_rx_pdu_adapter final : public f1ap_message_notifier
{
public:
  f1ap_rx_pdu_adapter(f1ap_message_handler& msg_handler_) : msg_handler(msg_handler_) {}

  void on_new_message(const f1ap_message& msg) override { msg_handler.handle_message(msg); }

private:
  f1ap_message_handler& msg_handler;
};

} // namespace

f1ap_du_connection_handler::f1ap_du_connection_handler(f1c_connection_client& f1c_client_handler_,
                                                       f1ap_message_handler&  f1ap_pdu_handler_) :
  f1c_client_handler(f1c_client_handler_),
  f1ap_pdu_handler(f1ap_pdu_handler_),
  logger(srslog::fetch_basic_logger("DU-F1"))
{
}

SRSRAN_NODISCARD std::unique_ptr<f1ap_message_notifier> f1ap_du_connection_handler::connect_to_cu_cp()
{
  std::unique_ptr<f1ap_message_notifier> f1ap_notifier =
      f1c_client_handler.handle_du_connection_request(std::make_unique<f1ap_rx_pdu_adapter>(f1ap_pdu_handler));

  if (f1ap_notifier != nullptr) {
    connected = true;
  }

  return f1ap_notifier;
}
