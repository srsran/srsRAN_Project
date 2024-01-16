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

#include "cu_cp_types.h"
#include "cu_up_processor_context.h"
#include "srsran/adt/optional.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/support/timers.h"
#include <string>

namespace srsran {
namespace srs_cu_cp {

/// Interface for an E1AP notifier to communicate with the CU-UP processor.
class cu_up_processor_e1ap_interface
{
public:
  virtual ~cu_up_processor_e1ap_interface() = default;

  /// \brief Get the CU-UP index.
  /// \return The CU-UP index.
  virtual cu_up_index_t get_cu_up_index() = 0;

  /// \brief Get the CU-UP processor context.
  /// \return The CU-UP processor context.
  virtual cu_up_processor_context& get_context() = 0;

  /// \brief Handle the reception of an GNB-CU-UP E1 Setup Request message and transmit the GNB-CU-UP E1 Setup Response
  /// or GNB-CU-UP E1 Setup Failure.
  /// \param[in] msg The received GNB-CU-UP E1 Setup Request message.
  virtual void handle_cu_up_e1_setup_request(const cu_up_e1_setup_request& msg) = 0;

  /// \brief Get the E1AP message handler interface of the CU-UP processor object.
  /// \return The E1AP message handler interface of the CU-UP processor object.
  virtual e1ap_message_handler& get_e1ap_message_handler() = 0;

  /// \brief Get the E1AP bearer context manager interface.
  /// \return The E1AP bearer context manager interface of the CU-UP processor object.
  virtual e1ap_bearer_context_manager& get_e1ap_bearer_context_manager() = 0;

  /// \brief Get the E1AP bearer context removal interface.
  /// \return The E1AP bearer context removal interface of the CU-UP processor object.
  virtual e1ap_bearer_context_removal_handler& get_e1ap_bearer_context_removal_handler() = 0;

  /// \brief Get the E1AP statistic interface.
  /// \return The E1AP statistic interface of the CU-UP processor object.
  virtual e1ap_statistics_handler& get_e1ap_statistics_handler() = 0;
};

/// Methods used by CU-UP processor to notify about CU-UP specific events.
class cu_up_processor_cu_up_management_notifier
{
public:
  virtual ~cu_up_processor_cu_up_management_notifier() = default;

  /// \brief Notifies the CU-CP about a new CU-UP connection.
  virtual void on_new_cu_up_connection() = 0;

  /// \brief Notifies about a failed CU-CP E1 Setup procedure.
  /// The corresponding CU-UP processor will be removed now.
  /// \param[in] cu_up_index The index of the CU-UP processor to delete.
  virtual void on_cu_up_remove_request_received(const cu_up_index_t cu_up_index) = 0;
};

class cu_up_ue_handler
{
public:
  virtual ~cu_up_ue_handler() = default;

  /// \brief Update the index of an UE.
  virtual void update_ue_index(ue_index_t ue_index, ue_index_t old_ue_index) = 0;
};

class cu_up_processor_interface : public cu_up_processor_e1ap_interface, public cu_up_ue_handler
{
public:
  virtual ~cu_up_processor_interface() = default;
};

} // namespace srs_cu_cp
} // namespace srsran
