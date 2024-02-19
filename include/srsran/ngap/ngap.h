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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ngap/ngap_handover.h"
#include "srsran/ngap/ngap_setup.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/timers.h"

namespace srsran {
namespace srs_cu_cp {

struct ngap_message;

/// This interface is used to push NGAP messages to the NGAP interface.
class ngap_message_handler
{
public:
  virtual ~ngap_message_handler() = default;

  /// Handle the incoming NGAP message.
  virtual void handle_message(const ngap_message& msg) = 0;
};

/// Interface used by NGAP to inform about events.
class ngap_event_handler
{
public:
  virtual ~ngap_event_handler()         = default;
  virtual void handle_connection_loss() = 0;
};

/// This interface notifies the reception of new NGAP messages over the NGAP interface.
class ngap_message_notifier
{
public:
  virtual ~ngap_message_notifier() = default;

  /// This callback is invoked on each received NGAP message.
  virtual void on_new_message(const ngap_message& msg) = 0;
};

/// Handle NGAP interface management procedures as defined in TS 38.413 section 8.7
class ngap_connection_manager
{
public:
  virtual ~ngap_connection_manager() = default;

  /// \brief Initiates the NG Setup procedure.
  /// \param[in] request The NGSetupRequest message to transmit.
  /// \return Returns a ngap_ng_setup_result struct.
  /// \remark The CU transmits the NGSetupRequest as per TS 38.413 section 8.7.1
  /// and awaits the response. If a NGSetupFailure is received the NGAP will handle the failure.
  virtual async_task<ngap_ng_setup_result> handle_ng_setup_request(const ngap_ng_setup_request& request) = 0;
};

/// Handle ue context removal
class ngap_ue_context_removal_handler
{
public:
  virtual ~ngap_ue_context_removal_handler() = default;

  /// \brief Remove the context of an UE.
  /// \param[in] ue_index The index of the UE to remove.
  virtual void remove_ue_context(ue_index_t ue_index) = 0;
};

/// Interface to notify the CU-CP about an NGAP UE creation.
class ngap_cu_cp_ue_creation_notifier
{
public:
  virtual ~ngap_cu_cp_ue_creation_notifier() = default;

  /// \brief Notifies the CU-CP about a new NGAP UE.
  /// \param[in] ue_index The index of the new NGAP UE.
  /// \returns True if the UE was successfully created, false otherwise.
  virtual bool on_new_ngap_ue(ue_index_t ue_index) = 0;
};

/// Interface to communication with the DU repository
/// Useful when the NGAP does not know the DU for an UE, e.g. paging and handover.
class ngap_cu_cp_du_repository_notifier
{
public:
  virtual ~ngap_cu_cp_du_repository_notifier() = default;

  /// \brief Notifies the CU-CP about a Paging message.
  virtual void on_paging_message(cu_cp_paging_message& msg) = 0;

  /// \brief Request UE index allocation on the CU-CP on N2 handover request.
  virtual ue_index_t request_new_ue_index_allocation(nr_cell_global_id_t cgi) = 0;

  /// \brief Notifies the CU-CP about a Handover Request.
  virtual async_task<ngap_handover_resource_allocation_response>
  on_ngap_handover_request(const ngap_handover_request& request) = 0;
};

/// Handle NGAP NAS Message procedures as defined in TS 38.413 section 8.6.
class ngap_nas_message_handler
{
public:
  virtual ~ngap_nas_message_handler() = default;

  /// \brief Initiates Initial UE message procedure as per TS 38.413 section 8.6.1.
  /// \param[in] msg The initial UE message to transmit.
  virtual void handle_initial_ue_message(const cu_cp_initial_ue_message& msg) = 0;

  /// \brief Initiates Uplink NAS transport procedure as per TS 38.413 section 8.6.3.
  /// \param[in] msg The ul nas transfer message to transmit.
  virtual void handle_ul_nas_transport_message(const cu_cp_ul_nas_transport& msg) = 0;
};

class ngap_control_message_handler
{
public:
  virtual ~ngap_control_message_handler() = default;

  /// \brief Initiates a UE Context Release Request procedure TS 38.413 section 8.3.2.
  /// \param[in] msg The ue context release request to transmit.
  /// \returns True if if a UeContextReleaseRequest was sent to the AMF, false if the UeContextReleaseRequest could not
  /// be sent e.g. because the UE didn't exist in the NGAP.
  virtual async_task<bool> handle_ue_context_release_request(const cu_cp_ue_context_release_request& msg) = 0;

  /// \brief Initiates a Handover Preparation procedure TS 38.413 section 8.4.1.
  virtual async_task<ngap_handover_preparation_response>
  handle_handover_preparation_request(const ngap_handover_preparation_request& msg) = 0;

  /// \brief Handle the reception of an inter CU handove related RRC Reconfiguration Complete.
  virtual void handle_inter_cu_ho_rrc_recfg_complete(const ue_index_t           ue_index,
                                                     const nr_cell_global_id_t& cgi,
                                                     const unsigned             tac) = 0;
};

/// Interface to notify about NAS PDUs and messages.
class ngap_rrc_ue_pdu_notifier
{
public:
  virtual ~ngap_rrc_ue_pdu_notifier() = default;

  /// \brief Notify about the a new nas pdu.
  /// \param [in] nas_pdu The nas pdu.
  virtual void on_new_pdu(byte_buffer nas_pdu) = 0;
};

/// Interface to notify the RRC UE about control messages.
class ngap_rrc_ue_control_notifier
{
public:
  virtual ~ngap_rrc_ue_control_notifier() = default;

  /// \brief Notify about the reception of new security context.
  virtual async_task<bool> on_new_security_context(const security::security_context& sec_context) = 0;

  /// \brief Get packed handover preparation message for inter-gNB handover.
  virtual byte_buffer on_handover_preparation_message_required() = 0;

  /// \brief Get the status of the security context.
  virtual bool on_security_enabled() = 0;

  /// \brief Notify about the reception of a new Handover Command pdu.
  /// \param[in] cmd The handover command RRC PDU.
  /// \returns true if the rrc reconfig was successfully forwarded to the DU, false otherwise.
  virtual bool on_new_rrc_handover_command(byte_buffer cmd) = 0;
};

/// Interface to notify the DU Processor about control messages.
class ngap_du_processor_control_notifier
{
public:
  virtual ~ngap_du_processor_control_notifier() = default;

  /// \brief Notify about the reception of a new PDU Session Resource Setup Request.
  virtual async_task<cu_cp_pdu_session_resource_setup_response>
  on_new_pdu_session_resource_setup_request(cu_cp_pdu_session_resource_setup_request& request) = 0;

  /// \brief Notify about the reception of a new PDU Session Resource Modify Request.
  virtual async_task<cu_cp_pdu_session_resource_modify_response>
  on_new_pdu_session_resource_modify_request(cu_cp_pdu_session_resource_modify_request& request) = 0;

  /// \brief Notify about the reception of a new PDU Session Resource Release Command.
  virtual async_task<cu_cp_pdu_session_resource_release_response>
  on_new_pdu_session_resource_release_command(cu_cp_pdu_session_resource_release_command& command) = 0;

  /// \brief Notify about the reception of a new UE Context Release Command.
  /// \param[in] command the UE Context Release Command.
  /// \returns The UE Context Release Complete.
  virtual async_task<cu_cp_ue_context_release_complete>
  on_new_ue_context_release_command(const cu_cp_ue_context_release_command& command) = 0;
};

/// Interface to control the NGAP.
class ngap_ue_control_manager
{
public:
  virtual ~ngap_ue_control_manager() = default;

  /// \brief Updates the NGAP UE context with a new UE index.
  /// \param[in] new_ue_index The new index of the UE.
  /// \param[in] old_ue_index The old index of the UE.
  /// \returns True if the update was successful, false otherwise.
  virtual bool update_ue_index(ue_index_t new_ue_index, ue_index_t old_ue_index) = 0;
};

/// \brief Schedules asynchronous tasks associated with an UE.
class ngap_ue_task_scheduler
{
public:
  virtual ~ngap_ue_task_scheduler()                                                        = default;
  virtual void           schedule_async_task(ue_index_t ue_index, async_task<void>&& task) = 0;
  virtual unique_timer   make_unique_timer()                                               = 0;
  virtual timer_manager& get_timer_manager()                                               = 0;
};

/// \brief Interface to query statistics from the NGAP interface.
class ngap_statistics_handler
{
public:
  virtual ~ngap_statistics_handler() = default;

  /// \brief Get the number of UEs registered at the NGAP.
  /// \return The number of UEs.
  virtual size_t get_nof_ues() const = 0;
};

/// Combined entry point for the NGAP object.
class ngap_interface : public ngap_message_handler,
                       public ngap_event_handler,
                       public ngap_connection_manager,
                       public ngap_nas_message_handler,
                       public ngap_control_message_handler,
                       public ngap_ue_control_manager,
                       public ngap_statistics_handler,
                       public ngap_ue_context_removal_handler
{
public:
  virtual ~ngap_interface() = default;

  virtual ngap_message_handler&            get_ngap_message_handler()            = 0;
  virtual ngap_event_handler&              get_ngap_event_handler()              = 0;
  virtual ngap_connection_manager&         get_ngap_connection_manager()         = 0;
  virtual ngap_nas_message_handler&        get_ngap_nas_message_handler()        = 0;
  virtual ngap_control_message_handler&    get_ngap_control_message_handler()    = 0;
  virtual ngap_ue_control_manager&         get_ngap_ue_control_manager()         = 0;
  virtual ngap_statistics_handler&         get_ngap_statistics_handler()         = 0;
  virtual ngap_ue_context_removal_handler& get_ngap_ue_context_removal_handler() = 0;
};

} // namespace srs_cu_cp

} // namespace srsran
