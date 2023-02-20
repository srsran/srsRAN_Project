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

#include "srsgnb/e1ap/common/e1ap_common.h"

namespace srsgnb {

/// \brief E1AP bridge between CU-CP and CU-UP using fast-path message passing.
class e1ap_local_adapter : public e1ap_message_notifier
{
public:
  explicit e1ap_local_adapter(const std::string& log_name) : logger(srslog::fetch_basic_logger(log_name)) {}

  void attach_handler(e1ap_message_handler* handler_) { handler = handler_; }
  void on_new_message(const e1ap_message& msg) override
  {
    report_fatal_error_if_not(handler, "E1AP message handler not set.");
    logger.debug("Received a PDU of type {}", msg.pdu.type().to_string());
    handler->handle_message(msg);
  }

private:
  srslog::basic_logger& logger;
  e1ap_message_handler* handler = nullptr;
};

}; // namespace srsgnb
