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

#include "du/ue_context/f1ap_du_ue_manager.h"
#include "f1ap_du_context.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/du_high/du_high_ue_executor_mapper.h"
#include "srsran/f1ap/du/f1ap_du.h"

#include "srsran/support/timers.h"
#include <memory>

namespace srsran {
namespace srs_du {

class f1ap_event_manager;

class f1ap_du_impl final : public f1ap_interface
{
public:
  f1ap_du_impl(f1ap_message_notifier&      event_notifier_,
               f1ap_du_configurator&       task_sched_,
               task_executor&              ctrl_exec,
               du_high_ue_executor_mapper& ue_exec_mapper);
  ~f1ap_du_impl();

  // F1AP interface management procedures functions as per TS38.473, Section 8.2.
  async_task<f1_setup_response_message> handle_f1_setup_request(const f1_setup_request_message& request) override;

  // F1AP RRC Message Transfer Procedure functions as per TS38.473, Section 8.4.
  void handle_rrc_delivery_report(const f1ap_rrc_delivery_report_msg& report) override {}

  // F1AP message handler functions
  void handle_message(const f1ap_message& msg) override;
  void handle_connection_loss() override {}

  // F1AP UE configuration functions
  f1ap_ue_creation_response      handle_ue_creation_request(const f1ap_ue_creation_request& msg) override;
  f1ap_ue_configuration_response handle_ue_configuration_request(const f1ap_ue_configuration_request& msg) override;

  // F1AP UE context manager functions
  async_task<f1ap_ue_context_modification_response_message>
       handle_ue_context_modification_required(const f1ap_ue_context_modification_required_message& msg) override;
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

  /// \brief Handle GNB-CU CONFIGURATION UPDATE as per TS38.473, Section 8.2.5.2.
  void handle_gnb_cu_configuration_update(const asn1::f1ap::gnb_cu_cfg_upd_s& msg);

  /// \brief Handle UE CONTEXT SETUP REQUEST as per TS38.473, Section 8.3.1.
  void handle_ue_context_setup_request(const asn1::f1ap::ue_context_setup_request_s& msg);

  /// \brief Handle UE CONTEXT MODIFICATION REQUEST as per TS38.473, Section 8.3.3.
  void handle_ue_context_release_command(const asn1::f1ap::ue_context_release_cmd_s& msg);

  /// \brief Handle UE CONTEXT MODIFICATION REQUEST as per TS38.473, Section 8.3.4.
  void handle_ue_context_modification_request(const asn1::f1ap::ue_context_mod_request_s& msg);

  /// \brief Handle DL RRC Message Transfer as per TS38.473, Section 8.4.2.2.
  void handle_dl_rrc_message_transfer(const asn1::f1ap::dl_rrc_msg_transfer_s& msg);

  bool handle_rx_message_gnb_cu_ue_f1ap_id(f1ap_du_ue& ue, gnb_cu_ue_f1ap_id_t cu_ue_id);

  void send_error_indication(const asn1::f1ap::cause_c& cause);

  srslog::basic_logger&       logger;
  f1ap_message_notifier&      f1ap_notifier;
  task_executor&              ctrl_exec;
  du_high_ue_executor_mapper& ue_exec_mapper;

  f1ap_du_configurator& du_mng;

  f1ap_du_ue_manager ues;

  f1ap_du_context ctxt;

  std::unique_ptr<f1ap_event_manager> events;
};

} // namespace srs_du
} // namespace srsran
