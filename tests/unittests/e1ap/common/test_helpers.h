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

#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/cu_up/cu_up.h"
#include "srsgnb/cu_up/cu_up_types.h"
#include "srsgnb/e1ap/common/e1ap_common.h"
#include "srsgnb/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsgnb/e1ap/cu_up/e1ap_cu_up.h"
#include "srsgnb/e1ap/cu_up/e1ap_cu_up_bearer_context_update.h"
#include "srsgnb/gateways/network_gateway.h"

namespace srsgnb {

class dummy_e1ap_cu_up_processor_notifier : public srs_cu_cp::e1ap_cu_up_processor_notifier
{
public:
  dummy_e1ap_cu_up_processor_notifier() : logger(srslog::fetch_basic_logger("TEST")) {}

  void on_cu_up_e1_setup_request_received(const cu_up_e1_setup_request& msg) override
  {
    logger.info("Received E1SetupRequest");
    last_cu_up_e1_setup_request = msg;
  }

  void set_ue_index(uint16_t ue_index_) { ue_index = ue_index_; }

  cu_up_e1_setup_request last_cu_up_e1_setup_request;

private:
  srslog::basic_logger& logger;
  uint16_t              ue_index = srs_cu_cp::ue_index_to_uint(srs_cu_cp::ue_index_t::min);
};

class dummy_e1ap_cu_up_notifier : public srs_cu_up::e1ap_cu_up_notifier
{
public:
  dummy_e1ap_cu_up_notifier() : logger(srslog::fetch_basic_logger("TEST")) {}

  cu_cp_e1_setup_response on_cu_cp_e1_setup_request_received(const cu_cp_e1_setup_request& msg) override
  {
    logger.info("Received E1SetupRequest");
    last_cu_cp_e1_setup_request = msg;

    cu_cp_e1_setup_response res = {};
    return res;
  }

  srs_cu_up::e1ap_bearer_context_setup_response
  on_bearer_context_setup_request_received(const srs_cu_up::e1ap_bearer_context_setup_request& msg) override
  {
    logger.info("Received BearerContextSetupRequest");
    last_bearer_context_setup_request.security_info.security_algorithm = msg.security_info.security_algorithm;
    last_bearer_context_setup_request.security_info.up_security_key.encryption_key =
        msg.security_info.up_security_key.encryption_key.copy();
    last_bearer_context_setup_request.security_info.up_security_key.integrity_protection_key =
        msg.security_info.up_security_key.integrity_protection_key.copy();
    last_bearer_context_setup_request.ue_dl_aggregate_maximum_bit_rate = msg.ue_dl_aggregate_maximum_bit_rate;
    last_bearer_context_setup_request.serving_plmn                     = msg.serving_plmn;
    last_bearer_context_setup_request.activity_notif_level             = msg.activity_notif_level;
    last_bearer_context_setup_request.pdu_session_res_to_setup_list    = msg.pdu_session_res_to_setup_list;

    srs_cu_up::e1ap_bearer_context_setup_response response = {};
    response.ue_index                                      = ue_index;
    response.success                                       = true;

    return response;
  }

  srs_cu_up::e1ap_bearer_context_modification_response on_bearer_context_modification_request_received(
      const srs_cu_up::e1ap_bearer_context_modification_request& msg) override
  {
    logger.info("Received BearerContextModificationRequest");
    last_bearer_context_modification_request                      = msg;
    srs_cu_up::e1ap_bearer_context_modification_response response = {};
    response.ue_index                                             = ue_index;
    response.success                                              = true;

    return response;
  }

  void on_bearer_context_release_command_received(const srs_cu_up::e1ap_bearer_context_release_command& msg) override
  {
    logger.info("Received BearerContextReleaseCommand");
    last_bearer_context_release_command = msg;

    return;
  }

  void set_ue_index(uint16_t ue_index_) { ue_index = srs_cu_up::int_to_ue_index(ue_index_); }

  srs_cu_up::e1ap_bearer_context_release_command      last_bearer_context_release_command;
  srs_cu_up::e1ap_bearer_context_modification_request last_bearer_context_modification_request;
  srs_cu_up::e1ap_bearer_context_setup_request        last_bearer_context_setup_request;
  cu_cp_e1_setup_request                              last_cu_cp_e1_setup_request;

private:
  srslog::basic_logger& logger;
  srs_cu_up::ue_index_t ue_index = srs_cu_up::MIN_UE_INDEX;
};

/// Reusable class implementing the notifier interface.
class e1ap_null_notifier : public e1ap_message_notifier
{
public:
  e1ap_message last_e1ap_msg;

  void on_new_message(const e1ap_message& msg) override
  {
    srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
    test_logger.info("Received PDU");
    last_e1ap_msg = msg;
  }
};

/// Reusable notifier class that a) stores the received PDU for test inspection and b)
/// calls the registered PDU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_e1ap_pdu_notifier : public e1ap_message_notifier
{
public:
  dummy_e1ap_pdu_notifier() : logger(srslog::fetch_basic_logger("TEST")){};

  void attach_handler(e1ap_message_handler* handler_) { handler = handler_; };
  void on_new_message(const e1ap_message& msg) override
  {
    logger.info("Received a PDU of type {}", msg.pdu.type().to_string());
    last_e1ap_msg = msg; // store msg

    if (handler != nullptr) {
      logger.info("Forwarding PDU");
      handler->handle_message(msg);
    }
  }
  e1ap_message last_e1ap_msg;

private:
  srslog::basic_logger& logger;
  e1ap_message_handler* handler = nullptr;
};

/// Reusable notifier class that a) stores the received PDU for test inspection and b)
/// calls the registered PDU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_cu_cp_e1ap_pdu_notifier : public e1ap_message_notifier
{
public:
  dummy_cu_cp_e1ap_pdu_notifier(srs_cu_cp::cu_cp_interface* cu_cp_, e1ap_message_handler* handler_) :
    logger(srslog::fetch_basic_logger("TEST")), cu_cp(cu_cp_), handler(handler_){};

  void attach_handler(srs_cu_cp::cu_cp_interface* cu_cp_, e1ap_message_handler* handler_)
  {
    cu_cp   = cu_cp_;
    handler = handler_;
    cu_cp->handle_new_cu_up_connection();
  };
  void on_new_message(const e1ap_message& msg) override
  {
    logger.info("Received a PDU of type {}", msg.pdu.type().to_string());
    last_e1ap_msg = msg; // store msg

    if (handler != nullptr) {
      logger.info("Forwarding PDU");
      handler->handle_message(msg);
    }
  }
  e1ap_message last_e1ap_msg;

private:
  srslog::basic_logger&       logger;
  srs_cu_cp::cu_cp_interface* cu_cp   = nullptr;
  e1ap_message_handler*       handler = nullptr;
};

/// Dummy handler just printing the received PDU.
class dummy_e1ap_message_handler : public e1ap_message_handler
{
public:
  dummy_e1ap_message_handler() : logger(srslog::fetch_basic_logger("TEST")){};
  void handle_message(const e1ap_message& msg) override
  {
    last_msg = msg;
    logger.info("Received a PDU of type {}", msg.pdu.type().to_string());
  }

  e1ap_message last_msg;

private:
  srslog::basic_logger& logger;
};

/// Dummy notifier just printing the received msg.
class dummy_e1ap_message_notifier : public e1ap_message_notifier
{
public:
  dummy_e1ap_message_notifier() : logger(srslog::fetch_basic_logger("TEST")){};
  void on_new_message(const e1ap_message& msg) override
  {
    last_msg = msg;
    logger.info("Received a PDU of type {}", msg.pdu.type().to_string());
  }

  e1ap_message last_msg;

private:
  srslog::basic_logger& logger;
};

} // namespace srsgnb
