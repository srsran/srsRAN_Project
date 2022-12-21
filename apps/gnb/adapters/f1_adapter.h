/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/f1ap/common/f1ap_common.h"

namespace srsgnb {

/// \brief F1C bridge between DU and CU-CP using fast-path message passing.
class f1c_local_adapter : public f1c_message_notifier
{
public:
  explicit f1c_local_adapter(const std::string& log_name) : logger(srslog::fetch_basic_logger(log_name)) {}

  void attach_handler(f1c_message_handler* handler_) { handler = handler_; }
  void on_new_message(const f1c_message& msg) override
  {
    report_fatal_error_if_not(handler, "F1C message handler not set.");
    logger.debug("Received a F1AP PDU of type {}", msg.pdu.type().to_string());
    handler->handle_message(msg);
  }

private:
  srslog::basic_logger& logger;
  f1c_message_handler*  handler = nullptr;
};

}; // namespace srsgnb
