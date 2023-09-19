/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
  logger(srslog::fetch_basic_logger("F1-DU"))
{
}

SRSRAN_NODISCARD bool f1ap_du_connection_handler::connect_to_cu_cp()
{
  f1ap_notifier =
      f1c_client_handler.handle_du_connection_request(std::make_unique<f1ap_rx_pdu_adapter>(f1ap_pdu_handler));

  return f1ap_notifier != nullptr;
}

void f1ap_du_connection_handler::on_new_message(const f1ap_message& msg)
{
  if (is_connected()) {
    f1ap_notifier->on_new_message(msg);
  } else {
    logger.error("Discarding F1AP DU message. Cause: Connection to CU-CP is dropped");
  }
}
