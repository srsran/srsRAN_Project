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

#include "../common/e1ap_asn1_utils.h"
#include "e1ap_cu_up_ue_context.h"
#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/e1/cu_up/e1ap_cu_up.h"
#include "srsgnb/support/executors/task_executor.h"
#include "srsgnb/support/timers.h"
#include <unordered_map>

namespace srsgnb {
namespace srs_cu_up {

class e1_event_manager;

class e1ap_cu_up_impl final : public e1_interface
{
public:
  e1ap_cu_up_impl(e1_message_notifier& e1_pdu_notifier_,
                  e1ap_cu_cp_notifier& e1_cu_up_notifier_,
                  task_executor&       cu_up_exec_);
  ~e1ap_cu_up_impl();

  // e1 connection manager functions

  void handle_cu_cp_e1_setup_response(const cu_cp_e1_setup_response& msg) override;

  // e1 message handler functions

  void handle_message(const e1_message& msg) override;

  void handle_connection_loss() override {}

private:
  /// \brief Notify about the reception of an initiating message.
  /// \param[in] msg The received initiating message.
  void handle_initiating_message(const asn1::e1ap::init_msg_s& msg);

  /// \brief Notify about the reception of an CU-CP E1 Setup Request message.
  /// \param[in] msg The CU-CP E1 Setup Request message.
  void handle_cu_cp_e1_setup_request(const asn1::e1ap::gnb_cu_cp_e1_setup_request_s& msg);

  /// \brief Notify about the reception of an Bearer Context Setup Request message.
  /// This starts the UE context creation at the UE manager and E1.
  /// \param[in] msg The E1 Bearer Context Setup message.
  void handle_bearer_context_setup_request(const asn1::e1ap::bearer_context_setup_request_s& msg);

  /// \brief Notify about the reception of an Bearer Context Modification Request message.
  /// This starts the UE context creation at the UE manager and E1.
  /// \param[in] msg The E1 Bearer Context Modification message.
  void handle_bearer_context_modification_request(const asn1::e1ap::bearer_context_mod_request_s& msg);

  /// \brief Notify about the reception of an Bearer Context Release Command message.
  /// This starts the UE context release at the UE manager and E1.
  /// \param[in] msg The E1 Bearer Context Release Command.
  void handle_bearer_context_release_command(const asn1::e1ap::bearer_context_release_cmd_s& msg);

  /// \brief Notify about the reception of an successful outcome.
  /// \param[in] msg The received successful outcome message.
  void handle_successful_outcome(const asn1::e1ap::successful_outcome_s& outcome);

  /// \brief Notify about the reception of an unsuccessful outcome.
  /// \param[in] msg The received unsuccessful outcome message.
  void handle_unsuccessful_outcome(const asn1::e1ap::unsuccessful_outcome_s& outcome);

  task_executor& cu_up_exec;

  srslog::basic_logger& logger;

  /// Repository of UE Contexts.
  e1ap_ue_context_list ue_ctxt_list;

  // nofifiers and handles
  e1_message_notifier& pdu_notifier;
  e1ap_cu_cp_notifier& e1_cu_up_notifier;

  unsigned current_transaction_id = 0; // store current E1AP transaction id
};

} // namespace srs_cu_up
} // namespace srsgnb
