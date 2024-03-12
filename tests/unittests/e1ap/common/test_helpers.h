/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#pragma once

#include "srsran/cu_cp/cu_cp.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_up/cu_up.h"
#include "srsran/cu_up/cu_up_types.h"
#include "srsran/e1ap/common/e1ap_common.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up_bearer_context_update.h"
#include "srsran/gateways/network_gateway.h"

namespace srsran {

/// \brief Generate a random gnb_cu_cp_ue_e1ap_id
gnb_cu_cp_ue_e1ap_id_t generate_random_gnb_cu_cp_ue_e1ap_id();

/// \brief Generate a random gnb_cu_up_ue_e1ap_id
gnb_cu_up_ue_e1ap_id_t generate_random_gnb_cu_up_ue_e1ap_id();

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
    for (const auto& request_setup_item : msg.pdu_session_res_to_setup_list) {
      e1ap_pdu_session_resource_setup_modification_item response_setup_item;
      response_setup_item.pdu_session_id               = request_setup_item.pdu_session_id;
      response_setup_item.ng_dl_up_tnl_info.gtp_teid   = int_to_gtpu_teid(1);
      response_setup_item.ng_dl_up_tnl_info.tp_address = transport_layer_address::create_from_string("127.0.0.1");

      for (const auto& request_drb_item : request_setup_item.drb_to_setup_list_ng_ran) {
        e1ap_drb_setup_item_ng_ran response_drb_item;
        response_drb_item.drb_id = request_drb_item.drb_id;

        response_setup_item.drb_setup_list_ng_ran.emplace(request_drb_item.drb_id, response_drb_item);
      }

      response.pdu_session_resource_setup_list.emplace(request_setup_item.pdu_session_id, response_setup_item);
    }

    return response;
  }

  srs_cu_up::e1ap_bearer_context_modification_response on_bearer_context_modification_request_received(
      const srs_cu_up::e1ap_bearer_context_modification_request& msg) override
  {
    logger.info("Received BearerContextModificationRequest");

    // copy message
    if (last_bearer_context_modification_request.security_info.has_value()) {
      e1ap_security_info security_info             = {};
      security_info.security_algorithm             = msg.security_info.value().security_algorithm;
      security_info.up_security_key.encryption_key = msg.security_info.value().up_security_key.encryption_key.copy();
      security_info.up_security_key.integrity_protection_key =
          msg.security_info.value().up_security_key.integrity_protection_key.copy();
      last_bearer_context_modification_request.security_info = security_info;
    }
    if (msg.ue_dl_aggr_max_bit_rate.has_value()) {
      last_bearer_context_modification_request.ue_dl_aggr_max_bit_rate = msg.ue_dl_aggr_max_bit_rate.value();
    }
    if (msg.ue_dl_max_integrity_protected_data_rate.has_value()) {
      last_bearer_context_modification_request.ue_dl_max_integrity_protected_data_rate =
          msg.ue_dl_max_integrity_protected_data_rate.value();
    }
    if (msg.bearer_context_status_change.has_value()) {
      last_bearer_context_modification_request.bearer_context_status_change = msg.bearer_context_status_change.value();
    }
    if (msg.new_ul_tnl_info_required.has_value()) {
      last_bearer_context_modification_request.new_ul_tnl_info_required = msg.new_ul_tnl_info_required.value();
    }
    if (msg.ue_inactivity_timer.has_value()) {
      last_bearer_context_modification_request.ue_inactivity_timer = msg.ue_inactivity_timer.value();
    }
    if (msg.data_discard_required.has_value()) {
      last_bearer_context_modification_request.data_discard_required = msg.data_discard_required.value();
    }
    if (msg.ng_ran_bearer_context_mod_request.has_value()) {
      last_bearer_context_modification_request.ng_ran_bearer_context_mod_request =
          msg.ng_ran_bearer_context_mod_request.value();
    }
    if (msg.ran_ue_id.has_value()) {
      last_bearer_context_modification_request.ran_ue_id = msg.ran_ue_id.value();
    }
    if (msg.gnb_du_id.has_value()) {
      last_bearer_context_modification_request.gnb_du_id = msg.gnb_du_id.value();
    }
    if (msg.activity_notif_level.has_value()) {
      last_bearer_context_modification_request.activity_notif_level = msg.activity_notif_level.value();
    }

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
  dummy_cu_cp_e1ap_pdu_notifier(srs_cu_cp::cu_cp* cu_cp_, e1ap_message_handler* handler_) :
    logger(srslog::fetch_basic_logger("TEST")), cu_cp(cu_cp_), handler(handler_){};

  void attach_handler(srs_cu_cp::cu_cp* cu_cp_, e1ap_message_handler* handler_)
  {
    cu_cp   = cu_cp_;
    handler = handler_;
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
  srslog::basic_logger& logger;
  srs_cu_cp::cu_cp*     cu_cp   = nullptr;
  e1ap_message_handler* handler = nullptr;
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

/// Dummy notifier just printing the received msg.
class dummy_e1ap_cu_cp_notifier : public srs_cu_cp::e1ap_cu_cp_notifier
{
public:
  dummy_e1ap_cu_cp_notifier() : logger(srslog::fetch_basic_logger("TEST")){};

  void on_e1ap_created(srs_cu_cp::e1ap_bearer_context_manager&         bearer_context_manager,
                       srs_cu_cp::e1ap_bearer_context_removal_handler& bearer_removal_handler,
                       srs_cu_cp::e1ap_statistics_handler&             e1ap_statistic_handler) override
  {
    logger.info("Received E1AP creation notification");
  }

  void on_bearer_context_inactivity_notification_received(const srs_cu_cp::cu_cp_inactivity_notification& msg) override
  {
    last_msg = msg;
    logger.info("Received an inactivity notification");
  }

  srs_cu_cp::cu_cp_inactivity_notification last_msg;

private:
  srslog::basic_logger& logger;
};

} // namespace srsran
