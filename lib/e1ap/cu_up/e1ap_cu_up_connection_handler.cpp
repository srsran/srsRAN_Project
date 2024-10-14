/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e1ap_cu_up_connection_handler.h"

using namespace srsran;
using namespace srs_cu_up;

namespace {

/// Adapter used to convert E1AP Rx PDUs coming from the CU-CP into E1AP messages.
class e1ap_rx_pdu_adapter final : public e1ap_message_notifier
{
public:
  e1ap_rx_pdu_adapter(e1ap_message_handler& msg_handler_) : msg_handler(msg_handler_) {}

  void on_new_message(const e1ap_message& msg) override { msg_handler.handle_message(msg); }

private:
  e1ap_message_handler& msg_handler;
};

} // namespace

e1ap_cu_up_connection_handler::e1ap_cu_up_connection_handler(e1_connection_client& e1_client_handler_,
                                                             e1ap_message_handler& e1ap_pdu_handler_) :
  e1_client_handler(e1_client_handler_),
  e1ap_pdu_handler(e1ap_pdu_handler_),
  logger(srslog::fetch_basic_logger("CU-UP-E1"))
{
}

e1ap_message_notifier* e1ap_cu_up_connection_handler::connect_to_cu_cp()
{
  e1ap_notifier =
      e1_client_handler.handle_cu_up_connection_request(std::make_unique<e1ap_rx_pdu_adapter>(e1ap_pdu_handler));

  return e1ap_notifier.get();
}
