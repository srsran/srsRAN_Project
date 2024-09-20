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

#include "srsran/f1ap/f1ap_message.h"
#include "srsran/f1ap/f1ap_message_handler.h"
#include "srsran/f1ap/f1ap_message_notifier.h"
#include "srsran/gateways/network_gateway.h"
#include "srsran/support/error_handling.h"

namespace srsran {

inline bool is_f1ap_pdu_packable(const asn1::f1ap::f1ap_pdu_c& pdu)
{
  byte_buffer   buffer;
  asn1::bit_ref bref(buffer);
  return pdu.pack(bref) == asn1::SRSASN_SUCCESS;
}

/// Reusable notifier class that a) stores the received PDU for test inspection and b)
/// calls the registered PDU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_f1ap_pdu_notifier : public f1ap_message_notifier
{
public:
  explicit dummy_f1ap_pdu_notifier(f1ap_message_handler* handler_ = nullptr) : handler(handler_) {}
  void attach_handler(f1ap_message_handler* handler_) { handler = handler_; }
  void on_new_message(const f1ap_message& msg) override
  {
    logger.info("Received a PDU of type {}", msg.pdu.type().to_string());
    last_f1ap_msg = msg; // store msg

    if (handler != nullptr) {
      logger.info("Forwarding PDU");
      handler->handle_message(msg);
    }
  }

  f1ap_message last_f1ap_msg;

private:
  srslog::basic_logger& logger  = srslog::fetch_basic_logger("TEST");
  f1ap_message_handler* handler = nullptr;
};

/// Reusable class implementing the notifier interface.
class f1ap_null_notifier : public f1ap_message_notifier
{
public:
  f1ap_message last_f1ap_msg;

  void on_new_message(const f1ap_message& msg) override
  {
    srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
    test_logger.info("Received PDU");
    last_f1ap_msg = msg;
    if (not is_f1ap_pdu_packable(msg.pdu)) {
      report_fatal_error("Output F1AP message is not packable");
    }
  }
};

/// Dummy handler just printing the received PDU.
class dummy_f1ap_message_handler : public f1ap_message_handler
{
public:
  void handle_message(const f1ap_message& msg) override
  {
    last_msg = msg;
    logger.info("Received a PDU of type {}", msg.pdu.type().to_string());
    if (not is_f1ap_pdu_packable(msg.pdu)) {
      report_fatal_error("Output F1AP message is not packable");
    }
  }

  f1ap_message last_msg;

private:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST");
};

} // namespace srsran
