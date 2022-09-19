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

#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/ngap/ngap.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Reusable notifier class that a) stores the received msg for test inspection and b)
/// calls the registered msg handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_ngc_amf_notifier : public ngc_message_notifier
{
public:
  // FIXME: Add handler when ngap exists
  dummy_ngc_amf_notifier(ngc_message_handler* handler_) :
    logger(srslog::fetch_basic_logger("TEST")), handler(handler_){};

  void attach_handler(ngc_message_handler* handler_) { handler = handler_; };

  void on_new_message(const ngc_message& msg) override
  {
    logger.info("Received NGAP message");
    last_ngc_msg = msg;

    if (handler != nullptr) {
      logger.info("Forwarding NGAP PDU");
      handler->handle_message(msg);
    }
  };

  ngc_message last_ngc_msg;

private:
  srslog::basic_logger& logger;
  ngc_message_handler*  handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsgnb