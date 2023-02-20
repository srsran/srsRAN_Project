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

#include "f1ap_du_ue_config.h"
#include "f1c_bearer.h"
#include "srsgnb/adt/byte_buffer_slice_chain.h"
#include "srsgnb/adt/expected.h"
#include "srsgnb/asn1/f1ap/f1ap.h"
#include "srsgnb/f1ap/common/f1ap_common.h"
#include "srsgnb/f1ap/du/f1ap_du_ue_context_update.h"
#include "srsgnb/f1u/du/f1u_bearer.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/timers.h"

namespace srsgnb {
namespace srs_du {

struct f1ap_rx_pdu {
  du_ue_index_t           ue_index = INVALID_DU_UE_INDEX;
  lcid_t                  lcid     = INVALID_LCID;
  byte_buffer_slice_chain pdu;
};

struct f1ap_initial_ul_rrc_msg {
  asn1::f1ap::init_ul_rrc_msg_transfer_s init_msg;
};

struct f1ap_ul_rrc_msg {
  asn1::f1ap::ul_rrc_msg_transfer_s ul_msg;
};

struct f1ap_rrc_delivery_report_msg {
  du_cell_index_t cell_index          = INVALID_DU_CELL_INDEX;
  du_ue_index_t   ue_index            = INVALID_DU_UE_INDEX;
  lcid_t          lcid                = INVALID_LCID;
  bool            rrc_delivery_status = false;
};

class f1ap_rrc_message_transfer_procedure_handler
{
public:
  virtual ~f1ap_rrc_message_transfer_procedure_handler() = default;

  /// \brief Packs and transmits the RRC delivery report as per TS 38.473 section 8.4.4.
  /// \param[in] report The RRC delivery report message to transmit.
  virtual void handle_rrc_delivery_report(const f1ap_rrc_delivery_report_msg& report) = 0;
};

struct du_setup_params {
  // mandatory
  uint64_t gnb_du_id;
  uint8_t  rrc_version;

  // optional
  std::string gnb_du_name;
};

struct f1_setup_request_message {
  asn1::f1ap::f1_setup_request_s msg;
  unsigned                       max_setup_retries = 5;
};

struct f1_setup_response_message {
  asn1::f1ap::f1_setup_resp_s msg;
  bool                        success = false;
};

/// Handle F1AP interface management procedures as defined in TS 38.473 section 8.2.
class f1ap_connection_manager
{
public:
  virtual ~f1ap_connection_manager() = default;

  /// \brief Initiates the F1 Setup procedure as per TS 38.473, Section 8.2.3.
  /// \param[in] request The F1SetupRequest message to transmit.
  /// \return Returns a f1_setup_response_message struct with the success member set to 'true' in case of a
  /// successful outcome, 'false' otherwise. \remark The DU transmits the F1SetupRequest as per TS 38.473 section 8.2.3
  /// and awaits the response. If a F1SetupFailure is received the F1AP will handle the failure.
  virtual async_task<f1_setup_response_message> handle_f1_setup_request(const f1_setup_request_message& request) = 0;
};

struct f1ap_ue_context_release_request_message {
  asn1::f1ap::ue_context_release_request_s msg;
};

struct f1ap_ue_context_modification_required_message {
  asn1::f1ap::ue_context_mod_required_s msg;
};

struct f1ap_ue_context_modification_response_message {
  asn1::f1ap::ue_context_mod_confirm_s confirm;
  asn1::f1ap::ue_context_mod_refuse_s  refuse;
  bool                                 success = false;
};

struct f1ap_ue_inactivity_notification_message {
  asn1::f1ap::ue_inactivity_notif_s msg;
};

struct f1ap_notify_message {
  asn1::f1ap::notify_s msg;
};

struct f1ap_ue_delete_request {
  du_ue_index_t ue_index = INVALID_DU_UE_INDEX;
};

/// Handle F1AP UE context management procedures as defined in TS 38.473 section 8.3.
class f1ap_ue_context_manager
{
public:
  virtual ~f1ap_ue_context_manager() = default;

  /// \brief Initiates creation of UE context object in F1AP.
  virtual f1ap_ue_creation_response handle_ue_creation_request(const f1ap_ue_creation_request& msg) = 0;

  /// \brief Updates the configuration of an existing UE context object in F1AP.
  virtual f1ap_ue_configuration_response handle_ue_configuration_request(const f1ap_ue_configuration_request& msg) = 0;

  /// \brief Initiates UE context release request procedure as per TS 38.473 section 8.3.2.
  /// \param[in] request The UE Context Release Request message to transmit.
  virtual void handle_ue_context_release_request(const f1ap_ue_context_release_request_message& request) = 0;

  /// \brief Initiates the UE Context Modification Required procedure as per TS 38.473 section 8.3.5.
  /// \param[in] msg The UE Context Modification Required message to transmit.
  /// \return Returns a f1ap_ue_context_modification_response_message struct with the success member set to 'true' in
  /// case of a successful outcome, 'false' otherwise.
  virtual async_task<f1ap_ue_context_modification_response_message>
  handle_ue_context_modification_required(const f1ap_ue_context_modification_required_message& msg) = 0;

  /// \brief Indicate an UE activity event as per TS 38.473 section 8.3.6
  /// \param[in] msg The UE Inactivity Nofication message to transmit.
  virtual void handle_ue_inactivity_notification(const f1ap_ue_inactivity_notification_message& msg) = 0;

  /// \brief Initiate the Notify procedure as per TS 38.473 section 8.3.7
  /// \param[in] msg The Notify message to transmit.
  virtual void handle_notify(const f1ap_notify_message& msg) = 0;
};

/// The F1AP uses this interface to request services such as timers and scheduling of asynchronous tasks.
class f1ap_ue_task_scheduler
{
public:
  virtual ~f1ap_ue_task_scheduler() = default;

  /// \brief Create timer for a given UE.
  virtual unique_timer create_timer() = 0;

  /// \brief Schedule Async Task respective to a given UE.
  virtual void schedule_async_task(async_task<void>&& task) = 0;
};

/// Class to manage scheduling of asynchronous F1AP tasks and timers.
class f1ap_task_scheduler
{
public:
  virtual ~f1ap_task_scheduler() = default;

  virtual timer_manager& get_timer_manager() = 0;

  /// \brief Schedule Async Task respective to the whole DU.
  virtual void schedule_async_task(async_task<void>&& task) = 0;
};

/// The F1AP uses this interface to notify the DU of new required updates (e.g. UE config modification, etc.) and to
/// request services such as timers, scheduling of async tasks, etc.
class f1ap_du_configurator : public f1ap_task_scheduler
{
public:
  virtual ~f1ap_du_configurator() = default;

  /// \brief Request the update of the UE configuration in the DU.
  virtual async_task<f1ap_ue_context_update_response>
  request_ue_context_update(const f1ap_ue_context_update_request& request) = 0;

  /// \brief Request the update of the UE configuration in the DU.
  virtual async_task<void> request_ue_removal(const f1ap_ue_delete_request& request) = 0;

  /// \brief Retrieve task scheduler specific to a given UE.
  virtual f1ap_ue_task_scheduler& get_ue_handler(du_ue_index_t ue_index) = 0;
};

/// Combined entry point for F1AP handling.
class f1ap_interface : public f1ap_message_handler,
                       public f1ap_event_handler,
                       public f1ap_rrc_message_transfer_procedure_handler,
                       public f1ap_connection_manager,
                       public f1ap_ue_context_manager
{
public:
  virtual ~f1ap_interface() = default;
};

} // namespace srs_du
} // namespace srsgnb
