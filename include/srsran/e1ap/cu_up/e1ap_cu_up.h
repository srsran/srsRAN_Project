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

#include "../common/e1ap_types.h"
#include "e1ap_cu_up_bearer_context_update.h"
#include "srsran/adt/expected.h"
#include "srsran/asn1/e1ap/e1ap.h"
#include "srsran/cu_up/cu_up_types.h"
#include "srsran/e1ap/common/e1_setup_messages.h"
#include "srsran/e1ap/common/e1ap_common.h"

namespace srsran {
namespace srs_cu_up {

/// \brief Request to modify a bearer context.
struct e1ap_bearer_context_modification_request {
  ue_index_t                                   ue_index = INVALID_UE_INDEX;
  asn1::e1ap::sys_bearer_context_mod_request_c request;
  // TODO: add optional fields
};

/// \brief Response to a bearer context modification request including UE index for E1AP map.
struct e1ap_bearer_context_modification_response {
  bool       success  = false;
  ue_index_t ue_index = INVALID_UE_INDEX; // Valid UE index if modification was successful.
  asn1::e1ap::sys_bearer_context_mod_resp_c sys_bearer_context_modification_resp;
  asn1::e1ap::cause_c                       cause; // Cause if modification was unsuccessful.
};

/// \brief Request to release a bearer context.
struct e1ap_bearer_context_release_command {
  ue_index_t          ue_index = INVALID_UE_INDEX;
  asn1::e1ap::cause_c cause; // Cause of the release.
};

/// Handle E1AP interface management procedures as defined in TS 38.463 section 8.2.
class e1ap_connection_manager
{
public:
  virtual ~e1ap_connection_manager() = default;

  /// \brief Creates and transmits the CU-CP initiated E1 Setup outcome to the CU-CP.
  virtual void handle_cu_cp_e1_setup_response(const cu_cp_e1_setup_response& msg) = 0;
};

/// Methods used by E1AP to notify the CU-UP.
class e1ap_cu_up_notifier
{
public:
  virtual ~e1ap_cu_up_notifier() = default;

  /// \brief Notifies about the reception of a E1 Setup Request message.
  /// \param[in] msg The received E1 Setup Request message.
  /// \return The CU-CP E1 Setup Response message.
  virtual cu_cp_e1_setup_response on_cu_cp_e1_setup_request_received(const cu_cp_e1_setup_request& msg) = 0;

  /// \brief Notifies the UE manager to create a UE context.
  /// \param[in] msg The received bearer context setup message.
  /// \return Returns a bearer context response message containing the index of the created UE context.
  virtual e1ap_bearer_context_setup_response
  on_bearer_context_setup_request_received(const e1ap_bearer_context_setup_request& msg) = 0;

  /// \brief Notifies the UE manager to create a UE context.
  /// \param[in] msg The received bearer context modification message.
  /// \return Returns a bearer context response message containing the index of the created UE context.
  virtual e1ap_bearer_context_modification_response
  on_bearer_context_modification_request_received(const e1ap_bearer_context_modification_request& msg) = 0;

  /// \brief Notifies the UE manager to release a UE context.
  /// \param[in] msg The received bearer context release command.
  virtual void on_bearer_context_release_command_received(const e1ap_bearer_context_release_command& msg) = 0;
};

/// Combined entry point for E1AP handling.
class e1ap_interface : public e1ap_message_handler, public e1ap_event_handler, public e1ap_connection_manager
{
public:
  virtual ~e1ap_interface() = default;
};

} // namespace srs_cu_up
} // namespace srsran
