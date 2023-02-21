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

#include "../common/e1ap_types.h"
#include "e1ap_cu_up_bearer_context_update.h"
#include "srsran/adt/expected.h"
#include "srsran/cu_up/cu_up_types.h"
#include "srsran/e1ap/common/e1_setup_messages.h"
#include "srsran/e1ap/common/e1ap_common.h"

namespace srsran {
namespace srs_cu_up {

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
