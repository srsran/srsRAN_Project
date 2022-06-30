/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_F1AP_CU_H
#define SRSGNB_F1AP_CU_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/expected.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/f1_interface/common/f1c_common.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

struct initial_ul_rrc_message_transfer_message {
  asn1::f1ap::init_ulrrc_msg_transfer_s msg;
};

struct ul_rrc_message_transfer_message {
  asn1::f1ap::ulrrc_msg_transfer_s msg;
};

struct dl_rrc_message_transfer_message {
  asn1::f1ap::dlrrc_msg_transfer_s msg;
};

class f1ap_rrc_message_transfer_procedure_handler
{
public:
  virtual ~f1ap_rrc_message_transfer_procedure_handler()                                  = default;
  virtual void handle_dl_rrc_message_transfer(const dl_rrc_message_transfer_message& msg) = 0;
};

struct f1_setup_response_message {
  asn1::f1ap::f1_setup_resp_s response;
  asn1::f1ap::f1_setup_fail_s failure;
  bool                        success;
};

struct f1_setup_request_message {
  asn1::f1ap::f1_setup_request_s request;
};

/// Handle F1AP interface management procedures as defined in TS 38.473 section 8.2.
class f1ap_connection_manager
{
public:
  virtual ~f1ap_connection_manager() = default;

  /// \brief Creates and transmits the F1 Setup outcome to the DU.
  /// \param[in] msg The f1_setup_response_message to transmit.
  /// \remark The CU transmits the F1SetupResponse/F1SetupFailure as per TS 38.473 section 8.2.3.
  virtual void handle_f1ap_setup_response(const f1_setup_response_message& msg) = 0;
};

struct f1ap_ue_context_setup_request_message {
  // TODO: replace asn1 by necessary containers
  asn1::f1ap::ue_context_setup_request_s msg;
};

struct f1ap_ue_context_setup_response_message {
  asn1::f1ap::ue_context_setup_resp_s msg;
  bool                                success;
};

struct f1ap_ue_context_release_command_message {
  asn1::f1ap::ue_context_release_complete_s msg;
};

struct f1ap_ue_context_release_complete_message {
  asn1::f1ap::ue_context_release_complete_s msg;
};

struct f1ap_ue_context_modification_request_message {
  asn1::f1ap::ue_context_mod_request_s msg;
};

struct f1ap_ue_context_modification_response_message {
  asn1::f1ap::ue_context_mod_resp_s response;
  asn1::f1ap::ue_context_mod_fail_s failure;
  bool                              success;
};

/// Handle F1AP UE context management procedures as defined in TS 38.473 section 8.3.
class f1ap_ue_context_manager
{
public:
  virtual ~f1ap_ue_context_manager() = default;

  /// Establish the UE context in F1.
  virtual async_task<f1ap_ue_context_setup_response_message>
  handle_ue_context_setup_request(const f1ap_ue_context_setup_request_message& request) = 0;

  /// \brief Initiates the UE Context Release procedure as per TS 38.473 section 8.3.3.
  /// \param[in] msg The UE Context Release message to transmit.
  /// \return Returns a UE Context Release Complete message.
  virtual async_task<f1ap_ue_context_release_complete_message>
  handle_ue_context_release(const f1ap_ue_context_release_command_message& msg) = 0;

  /// \brief Initiates the UE Context Modification procedure as per TS 38.473 section 8.3.4.
  /// \param[in] request The UE Context Modification message to transmit.
  /// \return Returns a f1ap_ue_context_modification_response_message struct struct with the success member set to
  /// 'true' in case of a successful outcome, 'false' otherwise.
  virtual async_task<f1ap_ue_context_modification_response_message>
  handle_ue_context_modification(const f1ap_ue_context_modification_request_message& request) = 0;
};

/// Methods used by F1AP to notify reception of initiating messages.
class f1c_initiating_message_notifier
{
public:
  virtual ~f1c_initiating_message_notifier()                                     = default;
  virtual void on_f1_setup_request_received(const f1_setup_request_message& msg) = 0;
  virtual void on_initial_ul_rrc_message_transfer_received(const initial_ul_rrc_message_transfer_message& msg) = 0;
  virtual void on_ul_rrc_message_transfer_received(const ul_rrc_message_transfer_message& msg)                 = 0;
};

/// Combined entry point for F1C/U handling.
class f1_interface : public f1c_message_handler,
                     public f1c_event_handler,
                     public f1ap_rrc_message_transfer_procedure_handler,
                     public f1ap_connection_manager,
                     public f1ap_ue_context_manager
{
public:
  virtual ~f1_interface() = default;
};

} // namespace srs_cu_cp
} // namespace srsgnb

#endif // SRSGNB_F1AP_CU_H
