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
#include "srsgnb/cu_cp/ue_context.h"
#include "srsgnb/e1_interface/common/e1_common.h"
#include "srsgnb/e1_interface/cu_cp/e1_cu_cp.h"
#include "srsgnb/gateways/network_gateway.h"

namespace srsgnb {

class dummy_e1_du_processor_notifier : public srs_cu_cp::e1_du_processor_notifier
{
public:
  dummy_e1_du_processor_notifier() : logger(srslog::fetch_basic_logger("TEST")) {}

  srs_cu_cp::du_index_t get_du_index() override { return srs_cu_cp::MIN_DU_INDEX; }

  void on_e1_setup_request_received(const cu_up_e1_setup_request_message& msg) override
  {
    logger.info("Received E1SetupRequest message.");
    last_cu_up_e1_setup_request_msg = msg;
  }

  void set_ue_index(uint16_t ue_index_) { ue_index = srs_cu_cp::int_to_ue_index(ue_index_); }

  cu_up_e1_setup_request_message last_cu_up_e1_setup_request_msg;

private:
  srslog::basic_logger& logger;
  uint16_t              ue_index = srs_cu_cp::MIN_UE_INDEX;
};

/// Reusable class implementing the notifier interface.
class e1_null_notifier : public e1_message_notifier
{
public:
  e1_message last_e1_msg;

  void on_new_message(const e1_message& msg) override
  {
    srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
    test_logger.info("Received E1 AP PDU");
    last_e1_msg = msg;
  }
};

class dummy_network_gateway_notifier : public network_gateway_control_notifier, public network_gateway_data_notifier
{
public:
  dummy_network_gateway_notifier() : logger(srslog::fetch_basic_logger("TEST")){};

  void on_connection_loss() override { logger.info("Connection loss"); }
  void on_connection_established() override { logger.info("Connection established"); }
  void on_new_pdu(byte_buffer pdu) override { logger.info("Received PDU"); }

private:
  srslog::basic_logger& logger;
};

/// Reusable notifier class that a) stores the received PDU for test inspection and b)
/// calls the registered PDU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_e1_pdu_notifier : public e1_message_notifier
{
public:
  dummy_e1_pdu_notifier(e1_message_handler* handler_) : logger(srslog::fetch_basic_logger("TEST")), handler(handler_){};

  void attach_handler(e1_message_handler* handler_) { handler = handler_; };
  void on_new_message(const e1_message& msg) override
  {
    logger.info("Received a E1AP PDU of type {}", msg.pdu.type().to_string());
    last_e1_msg = msg; // store msg

    if (handler != nullptr) {
      logger.info("Forwarding E1AP PDU");
      handler->handle_message(msg);
    }
  }
  e1_message last_e1_msg;

private:
  srslog::basic_logger& logger;
  e1_message_handler*   handler = nullptr;
};

/// Reusable notifier class that a) stores the received PDU for test inspection and b)
/// calls the registered PDU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_cu_cp_e1_pdu_notifier : public e1_message_notifier
{
public:
  dummy_cu_cp_e1_pdu_notifier(srs_cu_cp::cu_cp_interface* cu_cp_, e1_message_handler* handler_) :
    logger(srslog::fetch_basic_logger("TEST")), cu_cp(cu_cp_), handler(handler_){};

  void attach_handler(srs_cu_cp::cu_cp_interface* cu_cp_, e1_message_handler* handler_)
  {
    cu_cp   = cu_cp_;
    handler = handler_;
    cu_cp->on_new_connection();
  };
  void on_new_message(const e1_message& msg) override
  {
    logger.info("Received a E1AP PDU of type {}", msg.pdu.type().to_string());
    last_e1_msg = msg; // store msg

    if (handler != nullptr) {
      logger.info("Forwarding E1AP PDU");
      handler->handle_message(msg);
    }
  }
  e1_message last_e1_msg;

private:
  srslog::basic_logger&       logger;
  srs_cu_cp::cu_cp_interface* cu_cp   = nullptr;
  e1_message_handler*         handler = nullptr;
};

/// Dummy handler just printing the received PDU.
class dummy_e1_message_handler : public e1_message_handler
{
public:
  dummy_e1_message_handler() : logger(srslog::fetch_basic_logger("TEST")){};
  void handle_message(const e1_message& msg) override
  {
    last_msg = msg;
    logger.info("Received a E1AP PDU of type {}", msg.pdu.type().to_string());
  }

  e1_message last_msg;

private:
  srslog::basic_logger& logger;
};

/// Dummy PDU handler
class dummy_network_gateway_data_handler : public srsgnb::network_gateway_data_handler
{
public:
  dummy_network_gateway_data_handler(){};
  void handle_pdu(const byte_buffer& pdu) override { last_pdu = pdu.copy(); }

  byte_buffer last_pdu;
};

} // namespace srsgnb
