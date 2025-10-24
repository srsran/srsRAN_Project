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

#include "cu_up/e1ap_cu_up_metrics_collector.h"
#include "e1ap_cu_up_connection_handler.h"
#include "ue_context/e1ap_cu_up_ue_context.h"
#include "srsran/asn1/e1ap/e1ap.h"
#include "srsran/e1ap/cu_up/e1ap_configuration.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"

namespace srsran::srs_cu_up {

class e1_connection_client;
class e1ap_event_manager;

class e1ap_cu_up_impl final : public e1ap_interface
{
public:
  e1ap_cu_up_impl(const e1ap_configuration&    e1ap_cfg_,
                  e1_connection_client&        e1_client_handler_,
                  e1ap_cu_up_manager_notifier& cu_up_notifier_,
                  timer_manager&               timers_,
                  task_executor&               cu_up_exec_);
  ~e1ap_cu_up_impl() override;

  // e1ap connection manager functions
  [[nodiscard]] bool connect_to_cu_cp() override;
  // E1AP interface management procedures functions as per TS38.463, Section 8.2.
  async_task<cu_up_e1_setup_response> handle_cu_up_e1_setup_request(const cu_up_e1_setup_request& request) override;
  async_task<void>                    handle_cu_up_e1ap_release_request() override;

  // e1ap message handler functions
  void handle_message(const e1ap_message& msg) override;

  // e1ap control message handler functions
  void handle_bearer_context_inactivity_notification(const e1ap_bearer_context_inactivity_notification& msg) override;

  void handle_pdcp_max_count_reached(ue_index_t ue_index) override;

  // e1ap event handler functions
  void handle_connection_loss() override
  {
    // TODO
  }

  // e1ap_statistics_handler functions
  size_t get_nof_ues() const override { return ue_ctxt_list.size(); }

private:
  /// \brief Decorator of e1ap_message_notifier that logs the transmitted E1AP messages.
  class e1ap_message_notifier_with_logging final : public e1ap_message_notifier
  {
  public:
    e1ap_message_notifier_with_logging(e1ap_cu_up_impl& parent_, e1ap_message_notifier& notifier_);

    void on_new_message(const e1ap_message& msg) override;

  private:
    e1ap_cu_up_impl&       parent;
    e1ap_message_notifier& notifier;
  };

  /// \brief Log an E1AP Tx/Rx PDU.
  void log_pdu(bool is_rx, const e1ap_message& e1ap_pdu);

  /// \brief Notify about the reception of an initiating message.
  /// \param[in] msg The received initiating message.
  void handle_initiating_message(const asn1::e1ap::init_msg_s& msg);

  /// \brief Notify about the reception of an Bearer Context Setup Request message.
  /// This starts the UE context creation at the UE manager and E1.
  /// \param[in] msg The Bearer Context Setup message.
  void handle_bearer_context_setup_request(const asn1::e1ap::bearer_context_setup_request_s& msg);

  /// \brief Notify about the reception of an Bearer Context Modification Request message.
  /// This starts the UE context creation at the UE manager and E1.
  /// \param[in] msg The Bearer Context Modification message.
  void handle_bearer_context_modification_request(const asn1::e1ap::bearer_context_mod_request_s& msg);

  /// \brief Notify about the reception of an Bearer Context Release Command message.
  /// This starts the UE context release at the UE manager and E1.
  /// \param[in] msg The Bearer Context Release Command.
  void handle_bearer_context_release_command(const asn1::e1ap::bearer_context_release_cmd_s& msg);

  void handle_cu_up_e1ap_reset(const asn1::e1ap::reset_s& msg);

  /// \brief Notify about the reception of an successful outcome.
  /// \param[in] msg The received successful outcome message.
  void handle_successful_outcome(const asn1::e1ap::successful_outcome_s& outcome);

  /// \brief Notify about the reception of an unsuccessful outcome.
  /// \param[in] msg The received unsuccessful outcome message.
  void handle_unsuccessful_outcome(const asn1::e1ap::unsuccessful_outcome_s& outcome);

  const e1ap_configuration e1ap_cfg;
  srslog::basic_logger&    logger;

  // nofifiers and handles
  e1ap_cu_up_manager_notifier& cu_up_notifier;

  timer_manager& timers;
  task_executor& cu_up_exec;

  e1ap_cu_up_connection_handler                       connection_handler;
  std::unique_ptr<e1ap_message_notifier_with_logging> pdu_notifier;

  /// Repository of UE Contexts.
  e1ap_ue_context_list ue_ctxt_list;

  std::unique_ptr<e1ap_event_manager> ev_mng;

  unique_timer metrics_timer;

  e1ap_cu_up_metrics_collector metrics;
};

} // namespace srsran::srs_cu_up
