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

#include "procedures/cu_cp_e1_setup_procedure.h"
#include "procedures/e1_bearer_context_modification_procedure.h"
#include "procedures/e1_bearer_context_release_procedure.h"
#include "procedures/e1_bearer_context_setup_procedure.h"
#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/e1/cu_cp/e1_cu_cp.h"
#include "srsgnb/ran/nr_cgi.h"
#include "srsgnb/support/timers.h"
#include <memory>

namespace srsgnb {
namespace srs_cu_cp {

class e1_event_manager;

class e1_cu_cp_impl final : public e1_interface
{
public:
  e1_cu_cp_impl(srsgnb::timer_manager&       timers_,
                e1_message_notifier&         e1_pdu_notifier_,
                e1_cu_up_processor_notifier& e1_cu_up_processor_notifier_);
  ~e1_cu_cp_impl();

  // e1 connection manager functions

  void handle_cu_up_e1_setup_response(const cu_up_e1_setup_response_message& msg) override;

  async_task<cu_cp_e1_setup_response_message>
  handle_cu_cp_e1_setup_request(const cu_cp_e1_setup_request_message& request) override;

  // e1 bearer context manager functions

  async_task<e1ap_bearer_context_setup_response_message>
  handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request_message& request) override;

  async_task<e1ap_bearer_context_modification_response_message>
  handle_bearer_context_modification_request(const e1ap_bearer_context_modification_request_message& request) override;

  async_task<e1ap_bearer_context_release_complete_message>
  handle_bearer_context_release_command(const e1ap_bearer_context_release_command_message& command) override;

  // e1 message handler functions

  void handle_message(const e1_message& msg) override;

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

  /// \brief Get the next available CU-CP UE ID.
  /// \return The CU-CP UE ID.
  gnb_cu_cp_ue_e1ap_id_t get_next_cu_cp_ue_id();

  /// \brief Find the CU-CP UE ID by a given UE index.
  /// \param[in] ue_index The UE index used to find the CU-CP UE ID.
  /// \return The CU-CP UE ID.
  gnb_cu_cp_ue_e1ap_id_t find_cu_cp_ue_id(ue_index_t ue_index);

  srslog::basic_logger& logger;

  std::array<e1ap_ue_context, MAX_NOF_UES> cu_cp_ue_id_to_e1ap_ue_context;

  // nofifiers and handles

  timer_manager&               timers;
  e1_message_notifier&         pdu_notifier;
  e1_cu_up_processor_notifier& cu_up_notifier;

  std::unique_ptr<e1_event_manager> events;

  unsigned current_transaction_id = 0; // store current E1AP transaction id
};

} // namespace srs_cu_cp
} // namespace srsgnb
