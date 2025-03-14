/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "e2_connection_handler.h"
#include "procedures/e2_ric_control_procedure.h"
#include "procedures/e2_setup_procedure.h"
#include "procedures/e2_subscription_delete_procedure.h"
#include "procedures/e2_subscription_setup_procedure.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2ap_configuration.h"
#include "srsran/e2/e2sm/e2sm_factory.h"
#include "srsran/e2/e2sm/e2sm_manager.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"
#include <map>
#include <memory>

namespace srsran {

class e2_event_manager;

class e2_impl final : public e2_interface
{
public:
  e2_impl(srslog::basic_logger&     logger_,
          const e2ap_configuration& cfg_,
          e2ap_e2agent_notifier&    agent_notifier_,
          timer_factory             timers_,
          e2_connection_client&     e2_client_,
          e2_subscription_manager&  subscription_mngr_,
          e2sm_manager&             e2sm_mngr_,
          task_executor&            task_exec_);

  void start() override{};
  void stop() override{};

  /// E2 connection manager functions.
  bool                                  handle_e2_tnl_connection_request() override;
  async_task<void>                      handle_e2_disconnection_request() override;
  async_task<e2_setup_response_message> handle_e2_setup_request(e2_setup_request_message& request) override;
  async_task<e2_setup_response_message> start_initial_e2_setup_routine() override;

  /// E2_event_ handler functions.
  void handle_connection_loss() override {}

  /// E2 message handler functions.
  void handle_message(const e2_message& msg) override;

  /// e2sm configuration functions.
  void add_service_model(const std::string& ran_oid, std::unique_ptr<e2sm_handler> e2sm_handler);

private:
  /// \brief Notify about the reception of an initiating message.
  /// \param[in] outcome The received initiating message.
  void handle_initiating_message(const asn1::e2ap::init_msg_s& outcome);

  /// \brief Notify about the reception of an successful outcome.
  /// \param[in] outcome The received successful outcome message.
  void handle_successful_outcome(const asn1::e2ap::successful_outcome_s& outcome);

  /// \brief Notify about the reception of an unsuccessful outcome.
  /// \param[in] outcome The received unsuccessful outcome message.
  void handle_unsuccessful_outcome(const asn1::e2ap::unsuccessful_outcome_s& outcome);

  /// \brief Notify about the reception of an ric subscription request message.
  /// \param[in] msg The received ric subscription request message.
  void handle_ric_subscription_request(const asn1::e2ap::ric_sub_request_s& msg);

  /// \brief Notify about the reception of an ric control request message.
  /// \param[in] msg The received ric control request message.
  /// \return The ric control response message.
  void handle_ric_control_request(const asn1::e2ap::ric_ctrl_request_s msg);

  /// \brief Notify about the reception of an ric subscription delete request message.
  /// \param[in] msg The received ric subscription delete request message.
  void handle_ric_subscription_delete_request(const asn1::e2ap::ric_sub_delete_request_s& msg);

  /// \brief Notify about the reception of a E2 Connection Update message.
  /// \param[in] msg The received E2 Connection Update message.
  void handle_e2_connection_update(const asn1::e2ap::e2conn_upd_s& msg);

  /// \brief handle e2 setup response message from the ric interface.
  /// @param[in] msg  The received e2 setup response message.
  void handle_e2_setup_response(const e2_setup_response_message& msg);

  /// \brief handle e2 setup failure message from the ric interface.
  /// \param[in] msg  The received e2 setup failure message.
  void handle_e2_setup_failure(const e2_setup_response_message& msg);

  /// \brief set the allowed ran functions from the e2 setuo response message.
  /// \param[in] msg The received ran_function_id from the e2 setup response message.
  void set_allowed_ran_functions(const uint16_t ran_function_id);

  srslog::basic_logger&                               logger;
  const e2ap_configuration&                           cfg;
  timer_factory                                       timers;
  std::map<uint16_t, asn1::e2ap::ran_function_item_s> candidate_ran_functions;
  std::map<uint16_t, asn1::e2ap::ran_function_item_s> allowed_ran_functions;
  e2_subscription_proc&                               subscription_proc;
  e2sm_manager&                                       e2sm_mngr;
  std::unique_ptr<e2_event_manager>                   events;
  fifo_async_task_scheduler                           async_tasks;

  e2_connection_handler                connection_handler;
  std::unique_ptr<e2_message_notifier> tx_pdu_notifier;
};

} // namespace srsran
