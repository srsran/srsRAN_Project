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
#include "srsgnb/f1ap/common/f1c_common.h"
#include "srsgnb/f1ap/cu_cp/f1ap_cu.h"
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

/// Reusable notifier class that a) stores the received PDU for test inspection and b)
/// calls the registered PDU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_f1c_pdu_notifier : public f1c_message_notifier
{
public:
  explicit dummy_f1c_pdu_notifier(f1c_message_handler* handler_ = nullptr) : handler(handler_) {}
  void attach_handler(f1c_message_handler* handler_) { handler = handler_; }
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
  srslog::basic_logger& logger  = srslog::fetch_basic_logger("TEST");
  f1c_message_handler*  handler = nullptr;
};

/// Reusable class implementing the notifier interface.
class f1c_null_notifier : public f1c_message_notifier
{
public:
  f1c_message last_f1c_msg;

  void on_new_message(const f1c_message& msg) override
  {
    srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
    test_logger.info("Received F1AP PDU");
    last_f1c_msg = msg;
  }
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
    cu_cp->on_new_du_connection();
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

} // namespace srsgnb
