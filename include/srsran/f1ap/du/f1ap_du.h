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

#include "f1ap_du_ue_config.h"
#include "f1c_bearer.h"
#include "srsran/adt/expected.h"
#include "srsran/f1ap/common/f1ap_common.h"
#include "srsran/f1ap/du/f1ap_du_connection_manager.h"
#include "srsran/f1ap/du/f1ap_du_ue_context_update.h"
#include "srsran/f1u/du/f1u_bearer.h"
#include "srsran/mac/mac_paging_information_handler.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/rnti.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/timers.h"

namespace srsran {
namespace srs_du {

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

struct f1ap_ue_context_modification_confirm {
  bool success = false;
};

struct f1ap_ue_inactivity_notification_message {};

struct f1ap_notify_message {};

struct f1ap_ue_delete_request {
  du_ue_index_t ue_index = INVALID_DU_UE_INDEX;
};

/// Handle F1AP UE context management procedures as defined in TS 38.473 section 8.3.
class f1ap_ue_context_manager
{
public:
  virtual ~f1ap_ue_context_manager() = default;

  /// \brief Requests the creation of a new UE context in the F1AP.
  virtual f1ap_ue_creation_response handle_ue_creation_request(const f1ap_ue_creation_request& msg) = 0;

  /// \brief Updates the configuration of an existing UE context in the F1AP.
  virtual f1ap_ue_configuration_response handle_ue_configuration_request(const f1ap_ue_configuration_request& msg) = 0;

  /// \brief Removes UE Context from F1AP.
  virtual void handle_ue_deletion_request(du_ue_index_t ue_index) = 0;

  /// \brief Initiates the UE Context Release Request (gNB-DU initiated) procedure as per TS 38.473 section 8.3.2.
  virtual void handle_ue_context_release_request(const f1ap_ue_context_release_request& request) = 0;

  /// \brief Initiates the UE Context Modification Required procedure as per TS 38.473 section 8.3.5.
  /// \param[in] msg The UE Context Modification Required message to transmit.
  /// \return Returns a f1ap_ue_context_modification_confirm struct with the success member set to 'true' in
  /// case of a successful outcome, 'false' otherwise.
  virtual async_task<f1ap_ue_context_modification_confirm>
  handle_ue_context_modification_required(const f1ap_ue_context_modification_required& msg) = 0;

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

  virtual timer_factory& get_timer_factory() = 0;

  /// \brief Schedule Async Task respective to the whole DU.
  virtual void schedule_async_task(async_task<void>&& task) = 0;
};

/// The F1AP uses this interface to notify the DU of new required updates (e.g. UE config modification, etc.) and to
/// request services such as timers, scheduling of async tasks, etc.
class f1ap_du_configurator : public f1ap_task_scheduler
{
public:
  virtual ~f1ap_du_configurator() = default;

  /// \brief Search for an unused DU UE index.
  virtual du_ue_index_t find_free_ue_index() = 0;

  /// \brief Request to create a new UE context in the DU.
  virtual async_task<f1ap_ue_context_creation_response>
  request_ue_creation(const f1ap_ue_context_creation_request& request) = 0;

  /// \brief Request the update of an existing UE configuration in the DU.
  virtual async_task<f1ap_ue_context_update_response>
  request_ue_context_update(const f1ap_ue_context_update_request& request) = 0;

  /// \brief Request the update of the UE configuration in the DU.
  virtual async_task<void> request_ue_removal(const f1ap_ue_delete_request& request) = 0;

  /// \brief Notify DU that a given UE is performing RRC Reestablishment.
  virtual void notify_reestablishment_of_old_ue(du_ue_index_t new_ue_index, du_ue_index_t old_ue_index) = 0;

  /// \brief Retrieve task scheduler specific to a given UE.
  virtual f1ap_ue_task_scheduler& get_ue_handler(du_ue_index_t ue_index) = 0;
};

/// \brief The F1AP uses this interface to notify the DU of paging.
class f1ap_du_paging_notifier
{
public:
  virtual ~f1ap_du_paging_notifier() = default;

  /// \brief Forward the F1AP Paging to DU.
  virtual void on_paging_received(const paging_information& msg) = 0;
};

/// Combined entry point for F1AP handling.
class f1ap_du : public f1ap_message_handler,
                public f1ap_event_handler,
                public f1ap_rrc_message_transfer_procedure_handler,
                public f1ap_connection_manager,
                public f1ap_ue_context_manager
{
public:
  virtual ~f1ap_du() = default;
};

} // namespace srs_du
} // namespace srsran
