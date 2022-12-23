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

#include "../common/e1_types.h"
#include "e1ap_cu_cp_bearer_context_update.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/expected.h"
#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/e1/common/e1_common.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Handle E1 interface management procedures as defined in TS 38.463 section 8.2.
class e1_connection_manager
{
public:
  virtual ~e1_connection_manager() = default;

  /// \brief Creates and transmits the E1 Setup outcome to the CU-UP.
  /// \param[in] msg The cu_up_e1_setup_response to transmit.
  /// \remark The CU-CP transmits the E1SetupResponse/E1SetupFailure as per TS 38.463 section 8.2.3.
  virtual void handle_cu_up_e1_setup_response(const cu_up_e1_setup_response& msg) = 0;

  /// \brief Initiates the CU-CP E1 Setup procedure as per TS 38.463, Section 8.2.4.
  /// \param[in] request The E1SetupRequest message to transmit.
  /// \return Returns a cu_up_e1_setup_response struct with the success member set to 'true' in case of a
  /// successful outcome, 'false' otherwise.
  /// \remark The CU-CP transmits the E1SetupRequest as per TS 38.463 section 8.2.4
  /// and awaits the response. If a E1SetupFailure is received the E1 will handle the failure.
  virtual async_task<cu_cp_e1_setup_response> handle_cu_cp_e1_setup_request(const cu_cp_e1_setup_request& request) = 0;
};

/// Handle E1 bearer context management procedures as defined in TS 38.463 section 8.3.
class e1_bearer_context_manager
{
public:
  virtual ~e1_bearer_context_manager() = default;

  /// \brief Initiates the Bearer Context Setup procedure as per TS 38.463 section 8.3.1.
  /// \param[in] request The Bearer Context Setup Request message to transmit.
  /// \return Returns a e1ap_bearer_context_setup_response struct with the success member set to
  /// 'true' in case of a successful outcome, 'false' otherwise.
  virtual async_task<e1ap_bearer_context_setup_response>
  handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request& request) = 0;

  /// \brief Initiates the Bearer Context Modification procedure as per TS 38.463 section 8.3.2.
  /// \param[in] request The Bearer Context Modification Request message to transmit.
  /// \return Returns a f1ap_ue_context_modification_response struct with the success member set to
  /// 'true' in case of a successful outcome, 'false' otherwise.
  virtual async_task<e1ap_bearer_context_modification_response>
  handle_bearer_context_modification_request(const e1ap_bearer_context_modification_request& request) = 0;

  /// \brief Initiates the Bearer Context Release procedure as per TS 38.463 section 8.3.4.
  /// \param[in] msg The Bearer Context Release Command message to transmit.
  /// \return Returns the e1ap_bearer_context_release_complete struct.
  virtual async_task<e1ap_bearer_context_release_complete>
  handle_bearer_context_release_command(const e1ap_bearer_context_release_command& msg) = 0;
};

struct e1ap_ue_context {
  gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_t::invalid;
  ue_index_t             ue_index         = INVALID_UE_INDEX;
};

struct bearer_creation_complete_message {
  ue_index_t ue_index;
};

/// Methods used by E1 to notify the CU-UP processor.
class e1_cu_up_processor_notifier
{
public:
  virtual ~e1_cu_up_processor_notifier() = default;

  /// \brief Notifies about the reception of a GNB-CU-UP E1 Setup Request message.
  /// \param[in] msg The received GNB-CU-UP E1 Setup Request message.
  virtual void on_cu_up_e1_setup_request_received(const cu_up_e1_setup_request& msg) = 0;
};

/// Methods used by E1 to notify the NGAP.
class e1_ngap_notifier
{
public:
  virtual ~e1_ngap_notifier() = default;

  /// \brief Notifies about the reception of a E1 Setup Request message.
  /// \param[in] msg The received E1 Setup Request message.
  virtual void on_e1_setup_request_received(const cu_up_e1_setup_request& msg) = 0;
};

/// Combined entry point for E1 handling.
class e1_interface : public e1_message_handler,
                     public e1_event_handler,
                     public e1_connection_manager,
                     public e1_bearer_context_manager
{
public:
  virtual ~e1_interface() = default;
};

} // namespace srs_cu_cp
} // namespace srsgnb
