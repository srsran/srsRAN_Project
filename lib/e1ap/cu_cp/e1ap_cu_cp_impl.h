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

#include "procedures/bearer_context_modification_procedure.h"
#include "procedures/bearer_context_release_procedure.h"
#include "procedures/bearer_context_setup_procedure.h"
#include "procedures/cu_cp_e1_setup_procedure.h"
#include "ue_context/e1ap_cu_cp_ue_context.h"
#include "srsran/asn1/e1ap/e1ap.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"
#include <memory>

namespace srsran {
namespace srs_cu_cp {

class e1ap_event_manager;

class e1ap_cu_cp_impl final : public e1ap_interface
{
public:
  e1ap_cu_cp_impl(srsran::timer_manager&         timers_,
                  e1ap_message_notifier&         e1ap_pdu_notifier_,
                  e1ap_cu_up_processor_notifier& e1ap_cu_up_processor_notifier_,
                  task_executor&                 ctrl_exec_);
  ~e1ap_cu_cp_impl();

  // e1ap connection manager functions

  void handle_cu_up_e1_setup_response(const cu_up_e1_setup_response& msg) override;

  async_task<cu_cp_e1_setup_response> handle_cu_cp_e1_setup_request(const cu_cp_e1_setup_request& request) override;

  // e1ap bearer context manager functions

  async_task<e1ap_bearer_context_setup_response>
  handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg) override;

  async_task<e1ap_bearer_context_modification_response>
  handle_bearer_context_modification_request(const e1ap_bearer_context_modification_request& request) override;

  async_task<void> handle_bearer_context_release_command(const e1ap_bearer_context_release_command& command) override;

  // e1ap message handler functions

  void handle_message(const e1ap_message& msg) override;

  void handle_connection_loss() override {}

private:
  /// \brief Notify about the reception of an initiating message.
  /// \param[in] msg The received initiating message.
  void handle_initiating_message(const asn1::e1ap::init_msg_s& msg);

  /// \brief Notify about the reception of an successful outcome.
  /// \param[in] msg The received successful outcome message.
  void handle_successful_outcome(const asn1::e1ap::successful_outcome_s& outcome);

  /// \brief Notify about the reception of an unsuccessful outcome.
  /// \param[in] msg The received unsuccessful outcome message.
  void handle_unsuccessful_outcome(const asn1::e1ap::unsuccessful_outcome_s& outcome);

  srslog::basic_logger& logger;

  timer_manager& timers;

  /// Repository of UE Contexts.
  e1ap_ue_context_list ue_ctxt_list;

  e1ap_transaction_manager ev_mng;

  // nofifiers and handles
  e1ap_message_notifier&         pdu_notifier;
  e1ap_cu_up_processor_notifier& cu_up_processor_notifier;
  task_executor&                 ctrl_exec;

  unsigned current_transaction_id = 0; // store current E1AP transaction id
};

} // namespace srs_cu_cp
} // namespace srsran
