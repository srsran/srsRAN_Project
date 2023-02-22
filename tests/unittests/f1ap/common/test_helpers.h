/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/f1ap/common/f1ap_common.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/gateways/network_gateway.h"

namespace srsran {

class dummy_f1ap_rrc_message_notifier : public srs_cu_cp::f1ap_rrc_message_notifier
{
public:
  dummy_f1ap_rrc_message_notifier() = default;
  void on_new_rrc_message(asn1::unbounded_octstring<true> rrc_container) override
  {
    logger.info("Received RRC message");
    last_rrc_container = rrc_container;
  };

  asn1::unbounded_octstring<true> last_rrc_container;

private:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST");
};

class dummy_f1ap_du_processor_notifier : public srs_cu_cp::f1ap_du_processor_notifier
{
public:
  dummy_f1ap_du_processor_notifier() : logger(srslog::fetch_basic_logger("TEST")) {}

  srs_cu_cp::du_index_t get_du_index() override { return srs_cu_cp::du_index_t::min; }

  void on_f1_setup_request_received(const srs_cu_cp::f1_setup_request_message& msg) override
  {
    logger.info("Received F1SetupRequest");
    last_f1_setup_request_msg = msg;
  }

  srs_cu_cp::ue_creation_complete_message on_create_ue(const srs_cu_cp::f1ap_initial_ul_rrc_message& msg) override
  {
    logger.info("Received UeCreationRequest");
    last_ue_creation_request_msg                = msg;
    srs_cu_cp::ue_creation_complete_message ret = {};
    ret.ue_index                                = srs_cu_cp::ue_index_t::invalid;
    if (ue_id < srs_cu_cp::MAX_NOF_UES_PER_DU) {
      ret.ue_index          = srs_cu_cp::uint_to_ue_index(ue_id);
      last_created_ue_index = ret.ue_index;
      ue_id++;
      for (uint32_t i = 0; i < MAX_NOF_SRBS; i++) {
        ret.srbs[i] = rx_notifier.get();
      }
    }
    return ret;
  }

  void set_ue_id(uint16_t ue_id_) { ue_id = ue_id_; }

  srs_cu_cp::f1_setup_request_message              last_f1_setup_request_msg;
  srs_cu_cp::f1ap_initial_ul_rrc_message           last_ue_creation_request_msg;
  optional<srs_cu_cp::ue_index_t>                  last_created_ue_index;
  std::unique_ptr<dummy_f1ap_rrc_message_notifier> rx_notifier = std::make_unique<dummy_f1ap_rrc_message_notifier>();

private:
  srslog::basic_logger& logger;
  uint16_t              ue_id = ue_index_to_uint(srs_cu_cp::ue_index_t::min);
};

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
  }
};

/// Reusable notifier class that a) stores the received PDU for test inspection and b)
/// calls the registered PDU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_cu_cp_f1ap_pdu_notifier : public f1ap_message_notifier
{
public:
  dummy_cu_cp_f1ap_pdu_notifier(srs_cu_cp::cu_cp_interface* cu_cp_, f1ap_message_handler* handler_) :
    logger(srslog::fetch_basic_logger("TEST")), cu_cp(cu_cp_), handler(handler_){};

  void attach_handler(srs_cu_cp::cu_cp_interface* cu_cp_, f1ap_message_handler* handler_)
  {
    cu_cp   = cu_cp_;
    handler = handler_;
    cu_cp->handle_new_du_connection();
  };
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
  srslog::basic_logger&       logger;
  srs_cu_cp::cu_cp_interface* cu_cp   = nullptr;
  f1ap_message_handler*       handler = nullptr;
};

/// Dummy handler just printing the received PDU.
class dummy_f1ap_message_handler : public f1ap_message_handler
{
public:
  dummy_f1ap_message_handler() : logger(srslog::fetch_basic_logger("TEST")){};
  void handle_message(const f1ap_message& msg) override
  {
    last_msg = msg;
    logger.info("Received a PDU of type {}", msg.pdu.type().to_string());
  }

  f1ap_message last_msg;

private:
  srslog::basic_logger& logger;
};

} // namespace srsran
