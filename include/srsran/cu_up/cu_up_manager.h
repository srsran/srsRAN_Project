/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/e1ap/cu_up/e1ap_cu_up_bearer_context_update.h"
#include "srsran/support/async/async_task.h"

namespace srsran::srs_cu_up {

/// Interface to notify about E1AP connections (from the CU-CP) to the CU-UP
class cu_up_manager_e1ap_connection_notifier
{
public:
  virtual ~cu_up_manager_e1ap_connection_notifier() = default;

  /// \brief Notifies the CU-UP about a successful E1AP connection.
  virtual void on_e1ap_connection_establish() = 0;

  /// \brief Notifies the CU-CP about a dropped E1AP connection.
  virtual void on_e1ap_connection_drop() = 0;
};

class cu_up_manager_e1ap_interface
{
public:
  virtual ~cu_up_manager_e1ap_interface() = default;

  /// \brief Create a new UE context and handle bearer setup request.
  /// \param[in] msg The original bearer setup request.
  /// \return Returns message containing the index of the created UE and all response/failure message.
  virtual e1ap_bearer_context_setup_response
  handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg) = 0;

  /// \brief Create a new UE context and handle bearer modification request.
  /// \param[in] msg The original bearer modification request.
  /// \return Returns message containing the index of the created UE and all response/failure message.
  virtual async_task<e1ap_bearer_context_modification_response>
  handle_bearer_context_modification_request(const e1ap_bearer_context_modification_request& msg) = 0;

  /// \brief Handle bearer release command and remove the associated UE context.
  /// \param[in] msg The original bearer release command.
  virtual void handle_bearer_context_release_command(const e1ap_bearer_context_release_command& msg) = 0;

  /// \brief Get the state of the E1AP connection.
  /// \return True if E1AP is connected, false otherwise.
  virtual bool e1ap_is_connected() = 0;

  virtual void schedule_ue_async_task(srs_cu_up::ue_index_t ue_index, async_task<void> task) = 0;
};

class cu_up_manager : public cu_up_manager_e1ap_connection_notifier, public cu_up_manager_e1ap_interface
{
public:
  ~cu_up_manager() override = default;

  virtual async_task<e1ap_bearer_context_modification_response> enable_test_mode() = 0;
  virtual size_t                                                get_nof_ues()      = 0;
};

} // namespace srsran::srs_cu_up
