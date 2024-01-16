/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "du/ue_context/f1ap_du_ue_manager.h"
#include "f1ap_du_connection_handler.h"
#include "f1ap_du_context.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/du_high/du_high_executor_mapper.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include <memory>

namespace srsran {
namespace srs_du {

class f1c_connection_client;
class f1ap_event_manager;

class f1ap_du_impl final : public f1ap_du
{
public:
  f1ap_du_impl(f1c_connection_client&      f1c_client_handler_,
               f1ap_du_configurator&       task_sched_,
               task_executor&              ctrl_exec,
               du_high_ue_executor_mapper& ue_exec_mapper,
               f1ap_du_paging_notifier&    paging_notifier_);
  ~f1ap_du_impl() override;

  bool connect_to_cu_cp() override;

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
  void                           handle_ue_deletion_request(du_ue_index_t ue_index) override;

  // F1AP UE context manager functions
  void handle_ue_context_release_request(const f1ap_ue_context_release_request& request) override;
  async_task<f1ap_ue_context_modification_confirm>
       handle_ue_context_modification_required(const f1ap_ue_context_modification_required& msg) override;
  void handle_ue_inactivity_notification(const f1ap_ue_inactivity_notification_message& msg) override {}
  void handle_notify(const f1ap_notify_message& msg) override {}

  // F1AP UE ID translator functions.
  gnb_cu_ue_f1ap_id_t get_gnb_cu_ue_f1ap_id(const du_ue_index_t& ue_index) override;
  gnb_cu_ue_f1ap_id_t get_gnb_cu_ue_f1ap_id(const gnb_du_ue_f1ap_id_t& gnb_du_ue_f1ap_id) override;
  gnb_du_ue_f1ap_id_t get_gnb_du_ue_f1ap_id(const du_ue_index_t& ue_index) override;
  gnb_du_ue_f1ap_id_t get_gnb_du_ue_f1ap_id(const gnb_cu_ue_f1ap_id_t& gnb_cu_ue_f1ap_id) override;
  du_ue_index_t       get_ue_index(const gnb_du_ue_f1ap_id_t& gnb_du_ue_f1ap_id) override;
  du_ue_index_t       get_ue_index(const gnb_cu_ue_f1ap_id_t& gnb_cu_ue_f1ap_id) override;

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

  /// \brief Handle Paging as per TS38.473, Section 8.7.
  void handle_paging_request(const asn1::f1ap::paging_s& msg);

  srslog::basic_logger& logger;
  task_executor&        ctrl_exec;

  f1ap_du_connection_handler connection_handler;

  f1ap_du_configurator& du_mng;

  f1ap_du_ue_manager ues;

  f1ap_du_context ctxt;

  std::unique_ptr<f1ap_event_manager> events;

  f1ap_du_paging_notifier& paging_notifier;
};

} // namespace srs_du
} // namespace srsran
