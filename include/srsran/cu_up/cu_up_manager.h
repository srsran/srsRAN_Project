/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
  virtual async_task<void> handle_bearer_context_release_command(const e1ap_bearer_context_release_command& msg) = 0;

  /// \brief Handle E1 reset message. It will release bearer contexts as indicated by the reset message.
  /// \param[in] msg The reset message.
  virtual async_task<void> handle_e1_reset(const e1ap_reset& msg) = 0;

  /// \brief Get the state of the E1AP connection.
  /// \return True if E1AP is connected, false otherwise.
  virtual bool e1ap_is_connected() = 0;

  virtual void handle_e1ap_connection_drop() = 0;

  virtual void schedule_cu_up_async_task(async_task<void> task) = 0;

  virtual void schedule_ue_async_task(srs_cu_up::ue_index_t ue_index, async_task<void> task) = 0;
};

class cu_up_manager : public cu_up_manager_e1ap_connection_notifier, public cu_up_manager_e1ap_interface
{
public:
  ~cu_up_manager() override = default;

  virtual async_task<void> stop()             = 0;
  virtual async_task<void> enable_test_mode() = 0;
  virtual size_t           get_nof_ues()      = 0;
};

} // namespace srsran::srs_cu_up
