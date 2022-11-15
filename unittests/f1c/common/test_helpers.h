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
#include "srsgnb/f1c/common/f1c_common.h"
#include "srsgnb/f1c/cu_cp/f1c_cu.h"
#include "srsgnb/gateways/network_gateway.h"

namespace srsgnb {

class dummy_f1c_rrc_message_notifier : public srs_cu_cp::f1c_rrc_message_notifier
{
public:
  dummy_f1c_rrc_message_notifier() = default;
  void on_new_rrc_message(asn1::unbounded_octstring<true> rrc_container) override
  {
    logger.info("Received RRC message.");
    last_rrc_container = rrc_container;
  };

  asn1::unbounded_octstring<true> last_rrc_container;

private:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST");
};

class dummy_f1c_du_processor_notifier : public srs_cu_cp::f1c_du_processor_notifier
{
public:
  dummy_f1c_du_processor_notifier() : logger(srslog::fetch_basic_logger("TEST")) {}

  srs_cu_cp::du_index_t get_du_index() override { return srs_cu_cp::MIN_DU_INDEX; }

  void on_f1_setup_request_received(const srs_cu_cp::f1_setup_request_message& msg) override
  {
    logger.info("Received F1SetupRequest message.");
    last_f1_setup_request_msg = msg;
  }

  srs_cu_cp::ue_creation_complete_message on_create_ue(const srs_cu_cp::f1ap_initial_ul_rrc_message& msg) override
  {
    logger.info("Received UE creation request.");
    last_ue_creation_request_msg                = msg;
    srs_cu_cp::ue_creation_complete_message ret = {};
    ret.ue_index                                = srs_cu_cp::INVALID_UE_INDEX;
    if (ue_index < srs_cu_cp::MAX_NOF_UES) {
      ret.ue_index = srs_cu_cp::int_to_ue_index(ue_index);
      ue_index++;
      for (uint32_t i = 0; i < MAX_NOF_SRBS; i++) {
        ret.srbs[i] = rx_notifier.get();
      }
    }
    return ret;
  }

  void set_ue_index(uint16_t ue_index_) { ue_index = srs_cu_cp::int_to_ue_index(ue_index_); }

  srs_cu_cp::f1_setup_request_message             last_f1_setup_request_msg;
  srs_cu_cp::f1ap_initial_ul_rrc_message          last_ue_creation_request_msg;
  std::unique_ptr<dummy_f1c_rrc_message_notifier> rx_notifier = std::make_unique<dummy_f1c_rrc_message_notifier>();

private:
  srslog::basic_logger& logger;
  uint16_t              ue_index = srs_cu_cp::MIN_UE_INDEX;
};

/// Reusable class implementing the notifier interface.
class f1c_null_notifier : public f1c_message_notifier
{
public:
  f1c_message last_f1c_msg;

  void on_new_message(const f1c_message& msg) override
  {
    srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
    test_logger.info("Received F1 AP PDU");
    last_f1c_msg = msg;
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
class dummy_f1c_pdu_notifier : public f1c_message_notifier
{
public:
  dummy_f1c_pdu_notifier(f1c_message_handler* handler_) :
    logger(srslog::fetch_basic_logger("TEST")), handler(handler_){};

  void attach_handler(f1c_message_handler* handler_) { handler = handler_; };
  void on_new_message(const f1c_message& msg) override
  {
    logger.info("Received a F1AP PDU of type {}", msg.pdu.type().to_string());
    last_f1c_msg = msg; // store msg

    if (handler != nullptr) {
      logger.info("Forwarding F1AP PDU");
      handler->handle_message(msg);
    }
  }
  f1c_message last_f1c_msg;

private:
  srslog::basic_logger& logger;
  f1c_message_handler*  handler = nullptr;
};

/// Reusable notifier class that a) stores the received PDU for test inspection and b)
/// calls the registered PDU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_cu_cp_f1c_pdu_notifier : public f1c_message_notifier
{
public:
  dummy_cu_cp_f1c_pdu_notifier(srs_cu_cp::cu_cp_interface* cu_cp_, f1c_message_handler* handler_) :
    logger(srslog::fetch_basic_logger("TEST")), cu_cp(cu_cp_), handler(handler_){};

  void attach_handler(srs_cu_cp::cu_cp_interface* cu_cp_, f1c_message_handler* handler_)
  {
    cu_cp   = cu_cp_;
    handler = handler_;
    cu_cp->on_new_connection();
  };
  void on_new_message(const f1c_message& msg) override
  {
    logger.info("Received a F1AP PDU of type {}", msg.pdu.type().to_string());
    last_f1c_msg = msg; // store msg

    if (handler != nullptr) {
      logger.info("Forwarding F1AP PDU");
      handler->handle_message(msg);
    }
  }
  f1c_message last_f1c_msg;

private:
  srslog::basic_logger&       logger;
  srs_cu_cp::cu_cp_interface* cu_cp   = nullptr;
  f1c_message_handler*        handler = nullptr;
};

/// Reusable notifier class that a) stores the received du_index for test inspection and b)
/// calls the registered DU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_f1c_du_management_notifier : public srs_cu_cp::f1c_du_management_notifier
{
public:
  dummy_f1c_du_management_notifier(srs_cu_cp::cu_cp_du_handler* handler_) :
    logger(srslog::fetch_basic_logger("TEST")), handler(handler_){};

  void attach_handler(srs_cu_cp::cu_cp_du_handler* handler_) { handler = handler_; };
  void on_du_remove_request_received(const srs_cu_cp::du_index_t idx) override
  {
    logger.info("Received a du remove request for du {}", idx);
    last_du_idx = idx; // store idx

    if (handler != nullptr) {
      logger.info("Forwarding remove request");
      handler->handle_du_remove_request(idx);
    }
  }
  srs_cu_cp::du_index_t last_du_idx;

private:
  srslog::basic_logger&        logger;
  srs_cu_cp::cu_cp_du_handler* handler = nullptr;
};

/// Dummy handler just printing the received PDU.
class dummy_f1c_message_handler : public f1c_message_handler
{
public:
  dummy_f1c_message_handler() : logger(srslog::fetch_basic_logger("TEST")){};
  void handle_message(const f1c_message& msg) override
  {
    last_msg = msg;
    logger.info("Received a F1AP PDU of type {}", msg.pdu.type().to_string());
  }

  f1c_message last_msg;

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
