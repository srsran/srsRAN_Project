/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "e1ap_cu_cp_bearer_context_update.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/e1ap/common/e1_setup_messages.h"
#include "srsran/e1ap/common/e1ap_common.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// Handle E1AP interface management procedures as defined in TS 38.463 section 8.2.
class e1ap_connection_manager
{
public:
  virtual ~e1ap_connection_manager() = default;

  /// \brief Creates and transmits the E1 Setup outcome to the CU-UP.
  /// \param[in] msg The cu_up_e1_setup_response to transmit.
  /// \remark The CU-CP transmits the E1SetupResponse/E1SetupFailure as per TS 38.463 section 8.2.3.
  virtual void handle_cu_up_e1_setup_response(const cu_up_e1_setup_response& msg) = 0;

  /// \brief Initiates the CU-CP E1 Setup procedure as per TS 38.463, Section 8.2.4.
  /// \param[in] request The E1SetupRequest message to transmit.
  /// \return Returns a cu_up_e1_setup_response struct with the success member set to 'true' in case of a
  /// successful outcome, 'false' otherwise.
  /// \remark The CU-CP transmits the E1SetupRequest as per TS 38.463 section 8.2.4
  /// and awaits the response. If a E1SetupFailure is received the E1AP will handle the failure.
  virtual async_task<cu_cp_e1_setup_response> handle_cu_cp_e1_setup_request(const cu_cp_e1_setup_request& request) = 0;
};

/// Handle E1AP bearer context management procedures as defined in TS 38.463 section 8.3.
class e1ap_bearer_context_manager
{
public:
  virtual ~e1ap_bearer_context_manager() = default;

  /// \brief Initiates the Bearer Context Setup procedure as per TS 38.463 section 8.3.1.
  /// \param[in] request The Bearer Context Setup Request message to transmit.
  /// \return Returns a e1ap_bearer_context_setup_response struct with the success member set to
  /// 'true' in case of a successful outcome, 'false' otherwise.
  virtual async_task<e1ap_bearer_context_setup_response>
  handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request& request) = 0;

  /// \brief Initiates the Bearer Context Modification procedure as per TS 38.463 section 8.3.2.
  /// \param[in] request The Bearer Context Modification Request message to transmit.
  /// \return Returns a e1ap_bearer_context_modification_response struct with the success member set to
  /// 'true' in case of a successful outcome, 'false' otherwise.
  virtual async_task<e1ap_bearer_context_modification_response>
  handle_bearer_context_modification_request(const e1ap_bearer_context_modification_request& request) = 0;

  /// \brief Initiates the Bearer Context Release procedure as per TS 38.463 section 8.3.4.
  /// \param[in] msg The Bearer Context Release Command message to transmit.
  virtual async_task<void> handle_bearer_context_release_command(const e1ap_bearer_context_release_command& msg) = 0;
};

struct bearer_creation_complete_message {
  ue_index_t ue_index;
};

/// Methods used by E1AP to notify the CU-UP processor.
class e1ap_cu_up_processor_notifier
{
public:
  virtual ~e1ap_cu_up_processor_notifier() = default;

  /// \brief Notifies about the reception of a GNB-CU-UP E1 Setup Request message.
  /// \param[in] msg The received GNB-CU-UP E1 Setup Request message.
  virtual void on_cu_up_e1_setup_request_received(const cu_up_e1_setup_request& msg) = 0;
};

/// Methods used by E1AP to notify the CU-CP.
class e1ap_cu_cp_notifier
{
public:
  virtual ~e1ap_cu_cp_notifier() = default;

  /// \brief Notifies about the reception of a Bearer Context Inactivity Notification message.
  /// \param[in] msg The received Bearer Context Inactivity Notification message.
  virtual void on_bearer_context_inactivity_notification_received(const cu_cp_inactivity_notification& msg) = 0;
};

/// Methods used by E1AP to notify the NGAP.
class e1ap_ngap_notifier
{
public:
  virtual ~e1ap_ngap_notifier() = default;

  /// \brief Notifies about the reception of a E1 Setup Request message.
  /// \param[in] msg The received E1 Setup Request message.
  virtual void on_e1_setup_request_received(const cu_up_e1_setup_request& msg) = 0;
};

class e1ap_ue_handler
{
public:
  virtual ~e1ap_ue_handler() = default;

  /// \brief Update the context of an UE.
  virtual void update_ue_context(ue_index_t ue_index, ue_index_t old_ue_index) = 0;
};

/// Combined entry point for E1AP handling.
class e1ap_interface : public e1ap_message_handler,
                       public e1ap_event_handler,
                       public e1ap_connection_manager,
                       public e1ap_bearer_context_manager,
                       public e1ap_ue_handler
{
public:
  virtual ~e1ap_interface() = default;
};

} // namespace srs_cu_cp
} // namespace srsran
