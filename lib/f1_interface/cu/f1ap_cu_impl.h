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

#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/ran/nr_cgi.h"
#include "srsgnb/support/async/async_queue.h"
#include <memory>

namespace srsgnb {
namespace srs_cu_cp {

class f1ap_cu_impl final : public f1_interface
{
public:
  f1ap_cu_impl(f1c_message_notifier&              f1c_pdu_notifier_,
               f1c_du_processor_message_notifier& du_processor_notifier_,
               f1c_rrc_message_notifier&          rrc_message_notifier_,
               f1c_du_management_notifier&        f1c_du_management_notifier_);
  ~f1ap_cu_impl();

  // f1ap connection manager functions

  void handle_f1ap_setup_response(const f1_setup_response_message& msg) override;

  // f1ap rrc message transfer procedure functions

  void handle_dl_rrc_message_transfer(const f1ap_dl_rrc_msg& msg) override;

  // f1ap ue context manager functions

  async_task<f1ap_ue_context_setup_response_message>
  handle_ue_context_setup_request(const f1ap_ue_context_setup_request_message& request) override;

  async_task<f1ap_ue_context_release_complete_message>
  handle_ue_context_release(const f1ap_ue_context_release_command_message& msg) override;

  async_task<f1ap_ue_context_modification_response_message>
  handle_ue_context_modification(const f1ap_ue_context_modification_request_message& request) override;

  // f1c message handler functions

  void handle_message(const f1c_msg& msg) override;

  void handle_connection_loss() override {}

private:
  class f1ap_event_manager;

  /// \brief Notify the CU about the reception of an initiating message.
  /// \param[in] msg The received initiating message.
  void handle_initiating_message(const asn1::f1ap::init_msg_s& msg);

  /// \brief Notify the UE manager about the reception of an Initial UL RRC Message Transfer message.
  /// \param[in] msg The F1AP initial UL RRC message.
  void handle_initial_ul_rrc_message(const f1ap_initial_ul_rrc_msg& msg);

  /// \brief Notify the UE manager about the reception of an UL RRC Message Transfer message.
  /// \param[in] msg The F1AP UL RRC message.
  void handle_ul_rrc_message(const f1ap_ul_rrc_msg& msg);

  /// \brief Notify the CU-CP manager about the reception of an F1 Removal Request.
  /// \param[in] msg The F1 Removal Request message.
  void handle_f1_removal_resquest(const f1_removal_request_message& msg);

  /// \brief Get the next available CU UE ID.
  /// \return The CU UE ID.
  f1ap_ue_id_t get_next_cu_ue_id();

  /// \brief Find the CU UE ID by a given UE index.
  /// \param[in] ue_index The UE index used to find the CU UE ID.
  /// \return The CU UE ID.
  f1ap_ue_id_t find_cu_ue_id(ue_index_t ue_index);

  srslog::basic_logger& logger;

  std::array<f1ap_ue_context, MAX_NOF_UES> cu_ue_id_to_f1ap_ue_context;

  // nofifiers
  f1c_message_notifier&              pdu_notifier;
  f1c_du_processor_message_notifier& du_processor_notifier;
  f1c_rrc_message_notifier&          rrc_message_notifier;
  f1c_du_management_notifier&        du_management_notifier;

  std::unique_ptr<f1ap_event_manager> events;
};

} // namespace srs_cu_cp
} // namespace srsgnb
