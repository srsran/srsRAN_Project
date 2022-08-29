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

#include "srsgnb/asn1/ngap.h"
#include "srsgnb/ngap/ngap.h"
#include "srsgnb/support/timers.h"
#include <memory>

namespace srsgnb {

namespace srs_cu_cp {

class ngap_event_manager;

class ngap_impl final : public ngap_interface
{
public:
  ngap_impl(timer_manager& timers_, ng_message_notifier& event_notifier_);
  ~ngap_impl();

  // ngap connection manager functions
  async_task<ng_setup_response_message> handle_ngap_setup_request(const ng_setup_request_message& request) override;

  // ng message handler functions
  void handle_message(const ngap_message& msg) override;
  void handle_connection_loss() override {}

private:
  /// \brief Notify about the reception of an initiating message.
  /// \param[in] msg The received initiating message.
  void handle_initiating_message(const asn1::ngap::init_msg_s& msg);

  /// \brief Notify about the reception of a successful outcome message.
  /// \param[in] outcome The successful outcome message.
  void handle_successful_outcome(const asn1::ngap::successful_outcome_s& outcome);

  /// \brief Notify about the reception of an unsuccessful outcome message.
  /// \param[in] outcome The unsuccessful outcome message.
  void handle_unsuccessful_outcome(const asn1::ngap::unsuccessful_outcome_s& outcome);

  srslog::basic_logger& logger;
  timer_manager&        timers;
  ng_message_notifier&  ng_notifier;

  unique_timer ng_setup_timer;

  std::unique_ptr<ngap_event_manager> events;
};

} // namespace srs_cu_cp

} // namespace srsgnb