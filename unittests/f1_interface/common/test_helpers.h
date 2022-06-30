/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_F1_TEST_HELPERS_H
#define SRSGNB_LIB_F1_TEST_HELPERS_H

#include "srsgnb/f1_interface/common/f1c_common.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "test_helpers.h"

namespace srsgnb {

class dummy_f1c_init_message_notifier : public srs_cu_cp::f1c_initiating_message_notifier
{
public:
  dummy_f1c_init_message_notifier() : logger(srslog::fetch_basic_logger("TEST")) {}
  srs_cu_cp::f1_setup_request_message last_f1_setup_request_message;
  void on_f1_setup_request_received(const srs_cu_cp::f1_setup_request_message& msg) override
  {
    logger.info("Received F1SetupRequest message.");
    last_f1_setup_request_message = msg;
  }
  void on_initial_ul_rrc_message_transfer_received(const srs_cu_cp::f1ap_initial_ul_rrc_msg& msg) override
  {
    logger.info("Received Initial UL RRC Message transfer message.");
  }
  void on_ul_rrc_message_transfer_received(const srs_cu_cp::f1ap_ul_rrc_msg& msg) override
  {
    logger.info("Received UL RRC Message transfer message.");
  }

private:
  srslog::basic_logger& logger;
};

/// Reusable class implementing the notifier interface.
class f1c_null_notifier : public f1c_message_notifier
{
public:
  asn1::f1ap::f1_ap_pdu_c last_pdu;

  void on_new_message(const asn1::f1ap::f1_ap_pdu_c& msg) override
  {
    srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
    test_logger.info("Received F1 AP PDU");
    last_pdu = msg;
  }
};

/// Reusable notifier class that a) stores the received PDU for test inspection and b)
/// calls the registered PDU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_f1c_pdu_notifier : public f1c_message_notifier
{
public:
  dummy_f1c_pdu_notifier(f1c_message_handler* handler_) :
    logger(srslog::fetch_basic_logger("TEST")), handler(handler_){};

  void attach_handler(f1c_message_handler* handler_) { handler = handler_; };
  void on_new_message(const asn1::f1ap::f1_ap_pdu_c& msg) override
  {
    logger.info("Received a F1AP PDU of type {}", msg.type().to_string());
    last_pdu = msg; // store msg

    if (handler != nullptr) {
      logger.info("Forwarding F1AP PDU");
      handler->handle_message(msg);
    }
  }
  asn1::f1ap::f1_ap_pdu_c last_pdu;

private:
  srslog::basic_logger& logger;
  f1c_message_handler*  handler = nullptr;
};

/// Dummy handler just printing the received PDU.
class dummy_f1c_message_handler : public f1c_message_handler
{
public:
  dummy_f1c_message_handler() : logger(srslog::fetch_basic_logger("TEST")){};
  void handle_message(const asn1::f1ap::f1_ap_pdu_c& msg) override
  {
    logger.info("Received a F1AP PDU of type %s\n", msg.type().to_string());
  }

private:
  srslog::basic_logger& logger;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_F1_TEST_HELPERS_H
