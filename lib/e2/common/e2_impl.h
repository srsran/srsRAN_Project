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

#include "procedures/e2_setup_procedure.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/e2/e2.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/support/timers.h"
#include <memory>

namespace srsran {

class e2_event_manager;

class e2_impl final : public e2_interface
{
public:
  e2_impl(timer_manager& timers_, e2_message_notifier& e2_pdu_notifier_);

  /// E2 connection manager functions.
  async_task<e2_setup_response_message> handle_e2_setup_request(const e2_setup_request_message& request) override;

  void handle_e2_setup_response(const e2_setup_response_message& msg) override;

  /// E2_event_ handler functions.
  void handle_connection_loss() override {}

  /// E2 message handler functions.
  void handle_message(const e2_message& msg) override;

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

  srslog::basic_logger&             logger;
  timer_manager&                    timers;
  e2_message_notifier&              pdu_notifier;
  std::unique_ptr<e2_event_manager> events;

  unsigned current_transaction_id = 0; // store current E2AP transaction id
};

} // namespace srsran