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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ngap/ngap_context.h"
#include "srsran/ngap/ngap_handover.h"
#include "srsran/ngap/ngap_init_context_setup.h"
#include "srsran/ngap/ngap_metrics.h"
#include "srsran/ngap/ngap_reset.h"
#include "srsran/ngap/ngap_setup.h"
#include "srsran/ngap/ngap_ue_radio_capability_management.h"
#include "srsran/ran/plmn_identity.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

struct ngap_message;
struct up_pdu_session_context;

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

/// This interface notifies the transmission of new NGAP messages over the NGAP interface.
class ngap_message_notifier
{
public:
  virtual ~ngap_message_notifier() = default;

  /// \brief This callback is invoked on each transmitted NGAP message.
  /// \param[in] msg The NGAP message to transmit.
  /// \return True if the message was successfully transmitted, false otherwise.
  [[nodiscard]] virtual bool on_new_message(const ngap_message& msg) = 0;
};

/// Handle NGAP interface management procedures as defined in TS 38.413 section 8.7.
class ngap_connection_manager
{
public:
  virtual ~ngap_connection_manager() = default;

  /// \brief Request a new TNL association to the AMF.
  virtual bool handle_amf_tnl_connection_request() = 0;

  /// \brief Request the NGAP handler to disconnect from the AMF.
  virtual async_task<void> handle_amf_disconnection_request() = 0;

  /// \brief Initiates the NG Setup procedure.
  /// \param[in] max_setup_retries The maximum number of setup retries.
  /// \return Returns a ngap_ng_setup_result struct.
  /// \remark The CU transmits the NGSetupRequest as per TS 38.413 section 8.7.1
  /// and awaits the response. If a NGSetupFailure is received the NGAP will handle the failure.
  virtual async_task<ngap_ng_setup_result> handle_ng_setup_request(unsigned max_setup_retries) = 0;

  /// \brief Initiates NG Reset procedure as per TS 38.413 section 8.7.4.2.2.
  /// \param[in] msg The ng reset message to transmit.
  virtual async_task<void> handle_ng_reset_message(const cu_cp_ng_reset& msg) = 0;
};

/// Handle ue context removal.
class ngap_ue_context_removal_handler
{
public:
  virtual ~ngap_ue_context_removal_handler() = default;

  /// \brief Remove the context of an UE.
  /// \param[in] ue_index The index of the UE to remove.
  virtual void remove_ue_context(ue_index_t ue_index) = 0;
};

/// Notifier to the RRC UE.
class ngap_rrc_ue_notifier
{
public:
  virtual ~ngap_rrc_ue_notifier() = default;

  /// \brief Notify about the a new nas pdu.
  /// \param[in] nas_pdu The nas pdu.
  virtual void on_new_pdu(byte_buffer nas_pdu) = 0;

  /// \brief Get packed packed UE radio access capability info for UE radio capability info indication.
  virtual byte_buffer on_ue_radio_access_cap_info_required() = 0;

  /// \brief Get packed handover preparation message for inter-gNB handover.
  virtual byte_buffer on_handover_preparation_message_required() = 0;
};

/// NGAP notifier to the CU-CP UE.
class ngap_cu_cp_ue_notifier
{
public:
  virtual ~ngap_cu_cp_ue_notifier() = default;

  /// \brief Get the UE index of the UE.
  virtual ue_index_t get_ue_index() = 0;

  /// \brief Schedule an async task for the UE.
  virtual bool schedule_async_task(async_task<void> task) = 0;

  /// \brief Get the RRC UE notifier of the UE.
  virtual ngap_rrc_ue_notifier& get_ngap_rrc_ue_notifier() = 0;

  /// \brief Notify the CU-CP about a security context
  /// \param[in] sec_ctxt The received security context
  /// \return True if the security context was successfully initialized, false otherwise
  virtual bool init_security_context(const security::security_context& sec_ctxt) = 0;

  /// \brief Check if security is enabled
  [[nodiscard]] virtual bool is_security_enabled() const = 0;
};

/// NGAP notifier to the CU-CP.
class ngap_cu_cp_notifier
{
public:
  virtual ~ngap_cu_cp_notifier() = default;

  /// \brief Notifies the CU-CP about a new NGAP UE.
  /// \param[in] ue_index The index of the new NGAP UE.
  /// \returns Pointer to the NGAP UE notifier.
  virtual ngap_cu_cp_ue_notifier* on_new_ngap_ue(ue_index_t ue_index) = 0;

  /// \brief Request scheduling a task for a UE.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] task The task to schedule.
  /// \returns True if the task was successfully scheduled, false otherwise.
  virtual bool schedule_async_task(ue_index_t ue_index, async_task<void> task) = 0;

  /// \brief Notify the CU-CP about a handover request received.
  /// \param[in] ue_index Index of the UE.
  /// \param[in] selected_plmn The selected PLMN identity of the UE.
  /// \param[in] sec_ctxt The received security context.
  /// \return True if the handover request handling is successful, false otherwise.
  virtual bool on_handover_request_received(ue_index_t                        ue_index,
                                            const plmn_identity&              selected_plmn,
                                            const security::security_context& sec_ctxt) = 0;

  /// \brief Notify about the reception of a new Initial Context Setup Request.
  /// \param[in] request The received Initial Context Setup Request.
  /// \returns The Initial Context Setup Response or the Initial Context Setup Failure.
  virtual async_task<expected<ngap_init_context_setup_response, ngap_init_context_setup_failure>>
  on_new_initial_context_setup_request(ngap_init_context_setup_request& request) = 0;

  /// \brief Notify about the reception of a new PDU Session Resource Setup Request.
  /// \param[in] request The received PDU Session Resource Setup Request.
  /// \returns The PDU Session Resource Setup Response.
  virtual async_task<cu_cp_pdu_session_resource_setup_response>
  on_new_pdu_session_resource_setup_request(cu_cp_pdu_session_resource_setup_request& request) = 0;

  /// \brief Notify about the reception of a new PDU Session Resource Modify Request.
  /// \param[in] request The received PDU Session Resource Modify Request.
  /// \returns The PDU Session Resource Modify Response.
  virtual async_task<cu_cp_pdu_session_resource_modify_response>
  on_new_pdu_session_resource_modify_request(cu_cp_pdu_session_resource_modify_request& request) = 0;

  /// \brief Notify about the reception of a new PDU Session Resource Release Command.
  /// \param[in] command The received PDU Session Resource Release Command.
  /// \returns The PDU Session Resource Release Response.
  virtual async_task<cu_cp_pdu_session_resource_release_response>
  on_new_pdu_session_resource_release_command(cu_cp_pdu_session_resource_release_command& command) = 0;

  /// \brief Notify about the reception of a new UE Context Release Command.
  /// \param[in] command the UE Context Release Command.
  /// \returns The UE Context Release Complete.
  virtual async_task<cu_cp_ue_context_release_complete>
  on_new_ue_context_release_command(const cu_cp_ue_context_release_command& command) = 0;

  /// \brief Notifiy about the transmission of a handover required message to the AMF.
  virtual void on_transmission_of_handover_required() = 0;

  /// \brief Notify about the reception of a new Handover Command.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] command The Handover Command.
  /// \returns True if the Handover command is valid and was successfully handled by the DU.
  virtual async_task<bool> on_new_handover_command(ue_index_t ue_index, byte_buffer command) = 0;

  /// \brief Notify that the TNL connection to the AMF was lost.
  /// \param[in] amf_index The index of the lost AMF.
  virtual void on_n2_disconnection(amf_index_t amf_index) = 0;

  /// \brief Notifies the CU-CP about a Paging message.
  virtual void on_paging_message(cu_cp_paging_message& msg) = 0;

  /// \brief Request UE index allocation on the CU-CP on N2 handover request.
  virtual ue_index_t request_new_ue_index_allocation(nr_cell_global_id_t cgi, const plmn_identity& plmn) = 0;

  /// \brief Notifies the CU-CP about a Handover Request.
  virtual async_task<ngap_handover_resource_allocation_response>
  on_ngap_handover_request(const ngap_handover_request& request) = 0;

  /// \brief Notifies the CU-CP about a DL UE associated NRPPa transport.
  virtual void on_dl_ue_associated_nrppa_transport_pdu(ue_index_t ue_index, const byte_buffer& nrppa_pdu) = 0;

  /// \brief Notifies the CU-CP about a DL non UE associated NRPPa transport.
  /// \param[in] amf_index The index of the AMF that received the NRPPa transport.
  /// \param[in] nrppa_pdu The NRPPa transport PDU.
  virtual void on_dl_non_ue_associated_nrppa_transport_pdu(amf_index_t amf_index, const byte_buffer& nrppa_pdu) = 0;
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

/// Handle NGAP UE Radio Capability Management Messages as per TS 38.413 section 8.14.
class ngap_ue_radio_capability_management_handler
{
public:
  virtual ~ngap_ue_radio_capability_management_handler() = default;

  /// \brief Initiates the UE Radio Capability Info Indication procedure as per TS 38.413 section 8.14.1.
  /// \param[in] msg The ue radio capability info indication to transmit.
  virtual void
  handle_tx_ue_radio_capability_info_indication_required(const ngap_ue_radio_capability_info_indication& msg) = 0;
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

  virtual void handle_ul_ran_status_transfer(const ngap_ul_ran_status_transfer& ul_status_transfer) = 0;

  /// \brief Handle the reception of an inter CU handove related RRC Reconfiguration Complete.
  virtual void
  handle_inter_cu_ho_rrc_recfg_complete(const ue_index_t ue_index, const nr_cell_global_id_t& cgi, const tac_t tac) = 0;

  /// \brief Get the supported PLMNs.
  virtual const ngap_context_t& get_ngap_context() const = 0;

  /// \brief Handle the reception of a UL UE associated NRPPa message.
  virtual void handle_ul_ue_associated_nrppa_transport(ue_index_t ue_index, const byte_buffer& nrppa_pdu) = 0;

  /// \brief Handle the reception of a UL non UE associated NRPPa message.
  virtual async_task<void> handle_ul_non_ue_associated_nrppa_transport(const byte_buffer& nrppa_pdu) = 0;
};

/// Interface to control the NGAP.
class ngap_ue_control_manager
{
public:
  virtual ~ngap_ue_control_manager() = default;

  /// \brief Updates the NGAP UE context with a new UE index.
  /// \param[in] new_ue_index The new index of the UE.
  /// \param[in] old_ue_index The old index of the UE.
  /// \param[in] new_ue_notifier The notifier to the new UE.
  /// \returns True if the update was successful, false otherwise.
  virtual bool
  update_ue_index(ue_index_t new_ue_index, ue_index_t old_ue_index, ngap_cu_cp_ue_notifier& new_ue_notifier) = 0;
};

/// Interface to map between ue_index and amf_ue_id.
class ngap_ue_id_translator
{
public:
  virtual ~ngap_ue_id_translator() = default;

  /// \brief Map amf_ue_id to ue_index.
  /// \param[in] amf_ue_id of a given UE.
  /// \param[out] ue_index of the given UE.
  virtual ue_index_t get_ue_index(const amf_ue_id_t& amf_ue_id) = 0;

  /// \brief Map ue_index to amf_ue_id.
  /// \param[in] ue_index of a given UE.
  /// \param[out] amf_ue_id of the given UE.
  virtual amf_ue_id_t get_amf_ue_id(const ue_index_t& ue_index) = 0;
};

/// Interface used to capture the NGAP metrics from a single CU-CP NGAP.
class ngap_metrics_handler
{
public:
  virtual ~ngap_metrics_handler() = default;

  /// \brief Handle new request for metrics relative to a connected AMF.
  virtual ngap_info handle_ngap_metrics_report_request() const = 0;
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
                       public ngap_ue_radio_capability_management_handler,
                       public ngap_control_message_handler,
                       public ngap_ue_control_manager,
                       public ngap_statistics_handler,
                       public ngap_ue_context_removal_handler,
                       public ngap_metrics_handler,
                       public ngap_ue_id_translator
{
public:
  virtual ~ngap_interface() = default;

  virtual ngap_message_handler&                        get_ngap_message_handler()                 = 0;
  virtual ngap_event_handler&                          get_ngap_event_handler()                   = 0;
  virtual ngap_connection_manager&                     get_ngap_connection_manager()              = 0;
  virtual ngap_nas_message_handler&                    get_ngap_nas_message_handler()             = 0;
  virtual ngap_ue_radio_capability_management_handler& get_ngap_ue_radio_cap_management_handler() = 0;
  virtual ngap_control_message_handler&                get_ngap_control_message_handler()         = 0;
  virtual ngap_ue_control_manager&                     get_ngap_ue_control_manager()              = 0;
  virtual ngap_metrics_handler&                        get_metrics_handler()                      = 0;
  virtual ngap_statistics_handler&                     get_ngap_statistics_handler()              = 0;
  virtual ngap_ue_context_removal_handler&             get_ngap_ue_context_removal_handler()      = 0;
  virtual ngap_ue_id_translator&                       get_ngap_ue_id_translator()                = 0;
};

} // namespace srs_cu_cp

} // namespace srsran
