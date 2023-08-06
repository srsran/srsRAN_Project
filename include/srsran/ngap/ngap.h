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

#include "srsran/adt/optional.h"
#include "srsran/asn1/ngap/ngap.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ngap/ngap_handover.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/timers.h"

namespace srsran {

namespace srs_cu_cp {

struct ngap_message {
  asn1::ngap::ngap_pdu_c pdu;
};

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

struct ng_setup_request {
  asn1::ngap::ng_setup_request_s msg;
  unsigned                       max_setup_retries = 5;
};

struct ng_setup_response {
  asn1::ngap::ng_setup_resp_s msg;
  bool                        success = false;
};

/// Handle NGAP interface management procedures as defined in TS 38.413 section 8.7
class ngap_connection_manager
{
public:
  virtual ~ngap_connection_manager() = default;

  /// \brief Initiates the NG Setup procedure.
  /// \param[in] request The NGSetupRequest message to transmit.
  /// \return Returns a ng_setup_response struct with the success member set to 'true' in case of a
  /// successful outcome, 'false' otherwise.
  /// \remark The CU transmits the NGSetupRequest as per TS 38.413 section 8.7.1
  /// and awaits the response. If a NGSetupFailure is received the NGAP will handle the failure.
  virtual async_task<ng_setup_response> handle_ng_setup_request(const ng_setup_request& request) = 0;
};

/// Interface to notify about NGAP connections to the CU-CP
class ngap_cu_cp_connection_notifier
{
public:
  virtual ~ngap_cu_cp_connection_notifier() = default;

  /// \brief Notifies the CU-CP about a successful AMF connection.
  virtual void on_amf_connection() = 0;

  /// \brief Notifies the CU-CP about a dropped AMF connection.
  virtual void on_amf_connection_drop() = 0;
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

struct ngap_initial_context_failure_message {
  asn1::ngap::cause_c                                                          cause;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_failed_item> pdu_session_res_failed_to_setup_items;
  optional<asn1::ngap::crit_diagnostics_s>                                     crit_diagnostics;
};

struct ngap_initial_context_response_message {
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_response_item> pdu_session_res_setup_response_items;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_failed_item>   pdu_session_res_failed_to_setup_items;
  optional<asn1::ngap::crit_diagnostics_s>                                       crit_diagnostics;
};

struct ngap_initial_ue_message {
  ue_index_t                               ue_index = ue_index_t::invalid;
  byte_buffer                              nas_pdu;
  asn1::ngap::rrc_establishment_cause_opts establishment_cause;
  asn1::ngap::nr_cgi_s                     nr_cgi;
  uint32_t                                 tac;
  optional<cu_cp_five_g_s_tmsi>            five_g_s_tmsi;
};

struct ngap_ul_nas_transport_message {
  ue_index_t           ue_index = ue_index_t::invalid;
  byte_buffer          nas_pdu;
  asn1::ngap::nr_cgi_s nr_cgi;
  uint32_t             tac;
};

/// Handle NGAP NAS Message procedures as defined in TS 38.413 section 8.6.
class ngap_nas_message_handler
{
public:
  virtual ~ngap_nas_message_handler() = default;

  /// \brief Initiates Initial UE message procedure as per TS 38.413 section 8.6.1.
  /// \param[in] msg The initial UE message to transmit.
  virtual void handle_initial_ue_message(const ngap_initial_ue_message& msg) = 0;

  /// \brief Initiates Uplink NAS transport procedure as per TS 38.413 section 8.6.3.
  /// \param[in] msg The ul nas transfer message to transmit.
  virtual void handle_ul_nas_transport_message(const ngap_ul_nas_transport_message& msg) = 0;
};

class ngap_control_message_handler
{
public:
  virtual ~ngap_control_message_handler() = default;

  /// \brief Initiates a UE Context Release Request procedure TS 38.413 section 8.3.2.
  /// \param[in] msg The ue context release request to transmit.
  virtual void handle_ue_context_release_request(const cu_cp_ue_context_release_request& msg) = 0;

  /// \brief Initiates a Handover Preparation procedure TS 38.413 section 8.4.1.
  virtual async_task<ngap_handover_preparation_response>
  handle_handover_preparation_request(const ngap_handover_preparation_request& msg) = 0;
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

  /// \brief Notify about the reception of new security capabilities and key.
  virtual async_task<bool> on_new_security_context(const asn1::ngap::ue_security_cap_s&           caps,
                                                   const asn1::fixed_bitstring<256, false, true>& key) = 0;

  /// \brief Get required context for inter-gNB handover.
  virtual ngap_ue_source_handover_context on_ue_source_handover_context_required() = 0;
};

/// Interface to notify the DU Processor about control messages.
class ngap_du_processor_control_notifier
{
public:
  virtual ~ngap_du_processor_control_notifier() = default;

  /// \brief Request allocation of a new UE index.
  virtual ue_index_t on_new_ue_index_required() = 0;

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
  virtual cu_cp_ue_context_release_complete
  on_new_ue_context_release_command(const cu_cp_ngap_ue_context_release_command& command) = 0;
};

/// Interface to control the NGAP.
class ngap_ue_control_manager
{
public:
  virtual ~ngap_ue_control_manager() = default;

  /// \brief Creates a NGAP UE.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] rrc_ue_pdu_notifier The pdu notifier to the RRC UE.
  /// \param[in] rrc_ue_ctrl_notifier The control notifier to the RRC UE.
  /// \param[in] du_processor_ctrl_notifier The pdu notifier to the DU processor.
  virtual void create_ngap_ue(ue_index_t                          ue_index,
                              ngap_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier,
                              ngap_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier,
                              ngap_du_processor_control_notifier& du_processor_ctrl_notifier) = 0;
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
class ngap_statistic_interface
{
public:
  virtual ~ngap_statistic_interface() = default;

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
                       public ngap_statistic_interface
{
public:
  virtual ~ngap_interface() = default;

  virtual ngap_message_handler&         get_ngap_message_handler()         = 0;
  virtual ngap_event_handler&           get_ngap_event_handler()           = 0;
  virtual ngap_connection_manager&      get_ngap_connection_manager()      = 0;
  virtual ngap_nas_message_handler&     get_ngap_nas_message_handler()     = 0;
  virtual ngap_control_message_handler& get_ngap_control_message_handler() = 0;
  virtual ngap_ue_control_manager&      get_ngap_ue_control_manager()      = 0;
  virtual ngap_statistic_interface&     get_ngap_statistic_interface()     = 0;
};

} // namespace srs_cu_cp

} // namespace srsran
