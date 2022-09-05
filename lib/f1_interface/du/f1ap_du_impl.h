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

#include "f1ap_du_context.h"
#include "f1ap_du_ue.h"
#include "handlers/f1c_du_packet_handler.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/du_high/du_high_ue_executor_mapper.h"
#include "srsgnb/f1_interface/du/f1ap_du.h"

#include "srsgnb/support/timers.h"
#include <memory>

namespace srsgnb {
namespace srs_du {

class f1ap_event_manager;

class f1ap_du_impl final : public f1_interface
{
public:
  f1ap_du_impl(f1c_message_notifier&       event_notifier_,
               f1c_task_scheduler&         task_sched_,
               task_executor&              ctrl_exec,
               du_high_ue_executor_mapper& ue_exec_mapper);
  ~f1ap_du_impl();

  // f1ap connection manager functions
  async_task<f1_setup_response_message> handle_f1ap_setup_request(const f1_setup_request_message& request) override;

  // f1ap rrc message transfer procedure functions
  void handle_rrc_delivery_report(const f1ap_rrc_delivery_report_msg& report) override {}
  void handle_pdu(f1_rx_pdu pdu) override;
  void handle_init_ul_rrc_message_transfer(const f1ap_initial_ul_rrc_msg& msg) override {}
  void handle_ul_rrc_message_transfer(const f1ap_ul_rrc_msg& msg) override {}

  // f1c message handler functions
  void handle_message(const f1c_message& msg) override;
  void handle_connection_loss() override {}

  // f1ap ue context manager functions
  async_task<f1ap_ue_create_response> handle_ue_creation_request(const f1ap_ue_create_request& msg) override;
  void handle_ue_context_release_request(const f1ap_ue_context_release_request_message& request) override {}
  async_task<f1ap_ue_context_modification_response_message>
       handle_ue_context_modification(const f1ap_ue_context_modification_required_message& msg) override;
  void handle_ue_inactivity_notification(const f1ap_ue_inactivity_notification_message& msg) override {}
  void handle_notify(const f1ap_notify_message& msg) override {}

private:
  /// \brief Notify the DU about the reception of an initiating message.
  /// \param[in] msg The received initiating message.
  void handle_initiating_message(const asn1::f1ap::init_msg_s& msg);

  /// \brief Notify the DU about the reception of a successful outcome message.
  /// \param[in] outcome The successful outcome message.
  void handle_successful_outcome(const asn1::f1ap::successful_outcome_s& outcome);

  /// \brief Notify the DU about the reception of an unsuccessful outcome message.
  /// \param[in] outcome The unsuccessful outcome message.
  void handle_unsuccessful_outcome(const asn1::f1ap::unsuccessful_outcome_s& outcome);

  /// \brief Notify the DU about the reception of a DL RRC message transfer message.
  /// \param[in] msg The DL RRC message transfer message.
  void handle_dl_rrc_message_transfer(const asn1::f1ap::dlrrc_msg_transfer_s& msg);

  srslog::basic_logger&       logger;
  f1c_message_notifier&       f1c_notifier;
  task_executor&              ctrl_exec;
  du_high_ue_executor_mapper& ue_exec_mapper;

  std::unique_ptr<f1c_task_scheduler> task_sched;

  f1ap_du_ue_manager ues;

  f1ap_du_context ctxt;

  std::unique_ptr<f1ap_event_manager> events;
};

} // namespace srs_du
} // namespace srsgnb
