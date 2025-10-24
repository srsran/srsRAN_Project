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

#include "srsran/cu_cp/cell_meas_manager_config.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ngap/ngap.h"
#include "srsran/nrppa/nrppa.h"
#include "srsran/ran/plmn_identity.h"
#include "srsran/rrc/rrc_du.h"
#include "srsran/rrc/rrc_ue.h"
#include <string>

namespace srsran {
namespace srs_cu_cp {

class cu_cp_ue_context_release_handler
{
public:
  virtual ~cu_cp_ue_context_release_handler() = default;

  /// \brief Handle the reception of a new UE Context Release Command.
  /// \param[in] command The UE Context Release Command.
  /// \returns The UE Context Release Complete.
  virtual async_task<cu_cp_ue_context_release_complete>
  handle_ue_context_release_command(const cu_cp_ue_context_release_command& command) = 0;
};

/// Interface for the CU-CP to schedule tasks for UEs.
class cu_cp_task_scheduler_handler
{
public:
  virtual ~cu_cp_task_scheduler_handler() = default;

  /// \brief Schedule a task for a UE.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] task The task to schedule.
  /// \returns True if the task was successfully scheduled, false otherwise.
  virtual bool schedule_ue_task(ue_index_t ue_index, async_task<void> task) = 0;
};

/// Interface for the NGAP notifier to communicate with the CU-CP.
class cu_cp_ngap_handler : public cu_cp_ue_context_release_handler, public cu_cp_task_scheduler_handler
{
public:
  virtual ~cu_cp_ngap_handler() = default;

  /// \brief Handle the creation of a new NGAP UE. This will add the NGAP adapters to the UE manager.
  /// \param[in] ue_index The index of the new NGAP UE.
  /// \returns Pointer to the NGAP UE notifier.
  virtual ngap_cu_cp_ue_notifier* handle_new_ngap_ue(ue_index_t ue_index) = 0;

  /// \brief Handle a reeceived handover request.
  /// \param[in] ue_index Index of the UE.
  /// \param[in] selected_plmn The selected PLMN identity of the UE.
  /// \param[in] sec_ctxt The received security context.
  /// \return True if the handover request was successfully handled, false otherwise.
  virtual bool handle_handover_request(ue_index_t                        ue_index,
                                       const plmn_identity&              selected_plmn,
                                       const security::security_context& sec_ctxt) = 0;

  /// \brief Handle the reception of a new Initial Context Setup Request.
  /// \param[in] request The received Initial Context Setup Request.
  /// \returns The Initial Context Setup Response or the Initial Context Setup Failure.
  virtual async_task<expected<ngap_init_context_setup_response, ngap_init_context_setup_failure>>
  handle_new_initial_context_setup_request(const ngap_init_context_setup_request& request) = 0;

  /// \brief Handle the reception of a new PDU Session Resource Setup Request.
  /// \param[in] request The received PDU Session Resource Setup Request.
  /// \returns The PDU Session Resource Setup Response.
  virtual async_task<cu_cp_pdu_session_resource_setup_response>
  handle_new_pdu_session_resource_setup_request(cu_cp_pdu_session_resource_setup_request& request) = 0;

  /// \brief Handle the reception of a new PDU Session Resource Modify Request.
  /// \param[in] request The received PDU Session Resource Modify Request.
  /// \returns The PDU Session Resource Modify Response.
  virtual async_task<cu_cp_pdu_session_resource_modify_response>
  handle_new_pdu_session_resource_modify_request(const cu_cp_pdu_session_resource_modify_request& request) = 0;

  /// \brief Handle the reception of a new PDU Session Resource Release Command.
  /// \param[in] command The received PDU Session Resource Release Command.
  /// \returns The PDU Session Resource Release Response.
  virtual async_task<cu_cp_pdu_session_resource_release_response>
  handle_new_pdu_session_resource_release_command(const cu_cp_pdu_session_resource_release_command& command) = 0;

  /// \brief Handle the handover request of the handover resource allocation procedure handover procedure.
  /// See TS 38.413 section 8.4.2.2.
  virtual async_task<ngap_handover_resource_allocation_response>
  handle_ngap_handover_request(const ngap_handover_request& request) = 0;

  /// \brief Handle the transmission of a handover required message to the AMF.
  virtual void handle_transmission_of_handover_required() = 0;

  /// \brief Handle the reception of a new Handover Command.
  /// \param[in] ue_index The index of the UE that received the Handover Command.
  /// \param[in] command The received Handover Command.
  /// \returns True if the Handover Command was successfully handled, false otherwise.
  virtual async_task<bool> handle_new_handover_command(ue_index_t ue_index, byte_buffer command) = 0;

  /// \brief Handle the handover execution phase of the handover at target gNB.
  /// \param[in] ue_index The index of the UE that is performing the handover.
  virtual void handle_n2_handover_execution(ue_index_t ue_index) = 0;

  /// \brief Handles UE index allocation request for N2 handover at target gNB.
  virtual ue_index_t handle_ue_index_allocation_request(const nr_cell_global_id_t& cgi, const plmn_identity& plmn) = 0;

  /// \brief Handles a DL UE associated NRPPa transport.
  virtual void handle_dl_ue_associated_nrppa_transport_pdu(ue_index_t ue_index, const byte_buffer& nrppa_pdu) = 0;

  /// \brief Handles a DL non UE associated NRPPa transport.
  /// \param[in] amf_index The index of the AMF that received the NRPPa transport.
  /// \param[in] nrppa_pdu The NRPPa transport PDU.
  virtual void handle_dl_non_ue_associated_nrppa_transport_pdu(amf_index_t amf_index, const byte_buffer& nrppa_pdu) = 0;

  /// \brief Handle N2 AMF connection drop.
  /// \param[in] amf_index The index of the dropped AMF.
  virtual void handle_n2_disconnection(amf_index_t amf_index) = 0;
};

/// Interface for the NRPPa notifier to communicate with the CU-CP.
class cu_cp_nrppa_handler
{
public:
  virtual ~cu_cp_nrppa_handler() = default;

  /// \brief Handle the creation of a new NRPPA UE. This will add the NRPPA adapters to the UE manager.
  /// \param[in] ue_index The index of the new NRPPA UE.
  /// \returns Pointer to the NRPPA UE notifier.
  virtual nrppa_cu_cp_ue_notifier* handle_new_nrppa_ue(ue_index_t ue_index) = 0;

  /// \brief Handle a UL NRPPa PDU.
  /// \param[in] msg The NRPPa PDU.
  /// \param[in] ue_or_amf_index The UE index for UE associated NRPPa messages or the AMF index for non UE associated
  virtual void handle_ul_nrppa_pdu(const byte_buffer&                    nrppa_pdu,
                                   std::variant<ue_index_t, amf_index_t> ue_or_amf_index) = 0;

  /// \brief Handle a TRP information request.
  /// \param[in] request The TRP information request.
  /// \returns The TRP information CU-CP response.
  virtual async_task<trp_information_cu_cp_response_t>
  handle_trp_information_request(const trp_information_request_t& request) = 0;
};

/// Handler of E1AP-CU-CP events.
class cu_cp_e1ap_event_handler : public cu_cp_task_scheduler_handler
{
public:
  virtual ~cu_cp_e1ap_event_handler() = default;

  /// \brief Handle the reception of an Bearer Context Release Request message.
  /// \param[in] msg The received Bearer Context Release Request message.
  virtual void handle_bearer_context_release_request(const cu_cp_bearer_context_release_request& msg) = 0;

  /// \brief Handle the reception of an Bearer Context Inactivity Notification message.
  /// \param[in] msg The received Bearer Context Inactivity Notification message.
  virtual void handle_bearer_context_inactivity_notification(const cu_cp_inactivity_notification& msg) = 0;

  /// \brief Handle a UE release request.
  /// \param[in] request The release request.
  virtual async_task<void> handle_ue_context_release(const cu_cp_ue_context_release_request& request) = 0;

  /// \brief Handles the reception of an E1 Release Request message.
  /// \param[in] cu_up_index The index of the CU-UP processor.
  virtual void handle_e1_release_request(cu_up_index_t cu_up_index) = 0;

  /// \brief Handle transaction information loss in the E1AP.
  virtual async_task<void> handle_transaction_info_loss(const ue_transaction_info_loss_event& ev) = 0;
};

/// Interface used to handle DU specific procedures.
class cu_cp_du_event_handler
{
public:
  virtual ~cu_cp_du_event_handler() = default;

  /// \brief Handle a RRC UE creation notification from the DU processor.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] rrc_ue The interface of the created RRC UE.
  virtual void handle_rrc_ue_creation(ue_index_t ue_index, rrc_ue_interface& rrc_ue) = 0;

  /// \brief Handle a SIB1 request for a given cell.
  /// \param[in] du_index The index of the DU the cell is connected to.
  /// \param[in] cgi The cell global id of the cell.
  /// \returns The packed SIB1 for the cell, if available. An empty byte_buffer otherwise.
  virtual byte_buffer handle_target_cell_sib1_required(du_index_t du_index, nr_cell_global_id_t cgi) = 0;

  /// \brief Handle transaction information loss in the F1AP.
  virtual async_task<void> handle_transaction_info_loss(const ue_transaction_info_loss_event& ev) = 0;
};

/// Interface for an RRC UE entity to communicate with the CU-CP.
class cu_cp_rrc_ue_interface
{
public:
  virtual ~cu_cp_rrc_ue_interface() = default;

  /// \brief Handles a new UE connection with a selected PLMN.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] plmn The selected PLMN of the UE.
  /// \return True if the UE connection is accepted, false otherwise.
  virtual bool handle_ue_plmn_selected(ue_index_t ue_index, const plmn_identity& plmn) = 0;

  /// \brief Handle the reception of an RRC Reestablishment Request by transfering UE Contexts at the RRC.
  /// \param[in] old_pci The old PCI contained in the RRC Reestablishment Request.
  /// \param[in] old_c_rnti The old C-RNTI contained in the RRC Reestablishment Request.
  /// \param[in] ue_index The new UE index of the UE that sent the Reestablishment Request.
  /// \returns The RRC Reestablishment UE context for the old UE.
  virtual rrc_ue_reestablishment_context_response
  handle_rrc_reestablishment_request(pci_t old_pci, rnti_t old_c_rnti, ue_index_t ue_index) = 0;

  /// \brief Handle a required reestablishment context modification.
  /// \param[in] ue_index The index of the UE that needs the context modification.
  virtual async_task<bool> handle_rrc_reestablishment_context_modification_required(ue_index_t ue_index) = 0;

  /// \brief Handle reestablishment failure by releasing the old UE.
  /// \param[in] request The release request.
  virtual void handle_rrc_reestablishment_failure(const cu_cp_ue_context_release_request& request) = 0;

  /// \brief Handle an successful reestablishment by removing the old UE.
  /// \param[in] ue_index The index of the old UE to remove.
  virtual void handle_rrc_reestablishment_complete(ue_index_t old_ue_index) = 0;

  /// \brief Handle a notification of the reception of the RRC Reconfiguration Complete, and notify the DU with the F1AP
  /// UE context modification procedure with the RRC Reconfiguration Complete Indicator IE present.
  /// \param[in] ue_index The index of the UE that received the reconfiguration complete.
  virtual void handle_rrc_reconf_complete_indicator(ue_index_t ue_index) = 0;

  /// \brief Transfer and remove UE contexts for an ongoing Reestablishment.
  /// \param[in] ue_index The new UE index of the UE that sent the Reestablishment Request.
  /// \param[in] old_ue_index The old UE index of the UE that sent the Reestablishment Request.
  virtual async_task<bool> handle_ue_context_transfer(ue_index_t ue_index, ue_index_t old_ue_index) = 0;

  /// \brief Handle a UE release request.
  /// \param[in] request The release request.
  virtual async_task<void> handle_ue_context_release(const cu_cp_ue_context_release_request& request) = 0;
};

// Request with information for the target handler of the intra cu handover.
struct cu_cp_intra_cu_handover_target_request {
  ue_index_t                               target_ue_index = ue_index_t::invalid;
  ue_index_t                               source_ue_index = ue_index_t::invalid;
  uint8_t                                  transaction_id;
  std::chrono::milliseconds                timeout;
  e1ap_bearer_context_modification_request bearer_context_modification_request;
};

/// Interface for entities (e.g. DU processor) that wish to manipulate the context of a UE.
class cu_cp_ue_context_manipulation_handler
{
public:
  virtual ~cu_cp_ue_context_manipulation_handler() = default;

  /// \brief Handle a UE release request.
  /// \param[in] request The release request.
  virtual async_task<void> handle_ue_context_release(const cu_cp_ue_context_release_request& request) = 0;

  /// \brief Transfer and remove UE contexts for an ongoing Reestablishment/Handover.
  /// \param[in] ue_index The new UE index of the UE that sent the Reestablishment Request or is the target UE.
  /// \param[in] old_ue_index The old UE index of the UE that sent the Reestablishment Request or is the source UE.
  virtual async_task<bool> handle_ue_context_transfer(ue_index_t ue_index, ue_index_t old_ue_index) = 0;

  /// \brief Handle the trasmission of the handover reconfiguration by notifying the target RRC UE to await a RRC
  /// Reconfiguration Complete.
  /// \param[in] request The intra CU handover target request.
  virtual void handle_handover_reconfiguration_sent(const cu_cp_intra_cu_handover_target_request& request) = 0;

  /// \brief Handle a UE context push during handover.
  /// \param[in] source_ue_index The index of the UE that is the source of the handover.
  /// \param[in] target_ue_index The index of the UE that is the target of the handover.
  virtual void handle_handover_ue_context_push(ue_index_t source_ue_index, ue_index_t target_ue_index) = 0;

  /// \brief Initialize a handover UE release timer. When the timeout is reached, a release request is sent to the AMF.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] handover_ue_release_timeout The timeout for the release.
  /// \param[in] ue_context_release_request The release request.
  virtual void
  initialize_handover_ue_release_timer(ue_index_t                              ue_index,
                                       std::chrono::milliseconds               handover_ue_release_timeout,
                                       const cu_cp_ue_context_release_request& ue_context_release_request) = 0;
};

/// Methods used by CU-CP to transfer the RRC UE context e.g. for RRC Reestablishments.
class cu_cp_rrc_ue_context_transfer_notifier
{
public:
  virtual ~cu_cp_rrc_ue_context_transfer_notifier() = default;

  /// \brief Notifies the RRC UE to return the RRC Reestablishment UE context.
  virtual rrc_ue_reestablishment_context_response on_rrc_ue_context_transfer() = 0;
};

/// Interface to handle measurement requests.
class cu_cp_measurement_handler
{
public:
  virtual ~cu_cp_measurement_handler() = default;

  /// \brief Handle a measurement config request (for any UE) connected to the given serving cell.
  /// \param[in] ue_index The index of the UE to update the measurement config for.
  /// \param[in] nci The cell id of the serving cell to update.
  /// \param[in] current_meas_config The current meas config of the UE (if applicable).
  virtual std::optional<rrc_meas_cfg>
  handle_measurement_config_request(ue_index_t                         ue_index,
                                    nr_cell_identity                   nci,
                                    const std::optional<rrc_meas_cfg>& current_meas_config = std::nullopt) = 0;

  /// \brief Handle a measurement report for given UE.
  virtual void handle_measurement_report(const ue_index_t ue_index, const rrc_meas_results& meas_results) = 0;
};

/// Interface to handle measurement config update requests.
class cu_cp_measurement_config_handler
{
public:
  virtual ~cu_cp_measurement_config_handler() = default;

  /// \brief Handle a request to update the measurement related parameters for the given cell id.
  /// \param[in] nci The cell id of the serving cell to update.
  /// \param[in] serv_cell_cfg_ The serving cell meas config to update.
  virtual bool handle_cell_config_update_request(nr_cell_identity                nci,
                                                 const serving_cell_meas_config& serv_cell_cfg) = 0;
};

/// Interface to request handover.
class cu_cp_mobility_manager_handler
{
public:
  virtual ~cu_cp_mobility_manager_handler() = default;

  /// \brief Handle an Intra CU handover.
  virtual async_task<cu_cp_intra_cu_handover_response>
  handle_intra_cu_handover_request(const cu_cp_intra_cu_handover_request& request,
                                   du_index_t&                            source_du_index,
                                   du_index_t&                            target_du_index) = 0;
};

/// Interface to handle ue removals.
class cu_cp_ue_removal_handler
{
public:
  virtual ~cu_cp_ue_removal_handler() = default;

  /// \brief Completly remove a UE from the CU-CP.
  /// \param[in] ue_index The index of the UE to remove.
  virtual async_task<void> handle_ue_removal_request(ue_index_t ue_index) = 0;

  /// \brief Cancel pending UE tasks.
  virtual void handle_pending_ue_task_cancellation(ue_index_t ue_index) = 0;
};

/// Interface to handle AMF reconnections.
class cu_cp_amf_reconnection_handler
{
public:
  virtual ~cu_cp_amf_reconnection_handler() = default;

  /// \brief Handle AMF reconnections.
  /// \param[in] amf_index The index of the AMF that reconnected.
  virtual void handle_amf_reconnection(amf_index_t amf_index) = 0;
};

class cu_cp_impl_interface : public cu_cp_e1ap_event_handler,
                             public cu_cp_du_event_handler,
                             public cu_cp_rrc_ue_interface,
                             public cu_cp_measurement_handler,
                             public cu_cp_measurement_config_handler,
                             public cu_cp_ngap_handler,
                             public cu_cp_nrppa_handler,
                             public cu_cp_ue_context_manipulation_handler,
                             public cu_cp_mobility_manager_handler,
                             public cu_cp_ue_removal_handler,
                             public cu_cp_amf_reconnection_handler
{
public:
  virtual ~cu_cp_impl_interface() = default;

  virtual cu_cp_e1ap_event_handler&              get_cu_cp_e1ap_handler()               = 0;
  virtual cu_cp_ngap_handler&                    get_cu_cp_ngap_handler()               = 0;
  virtual cu_cp_nrppa_handler&                   get_cu_cp_nrppa_handler()              = 0;
  virtual cu_cp_rrc_ue_interface&                get_cu_cp_rrc_ue_interface()           = 0;
  virtual cu_cp_ue_context_manipulation_handler& get_cu_cp_ue_context_handler()         = 0;
  virtual cu_cp_measurement_handler&             get_cu_cp_measurement_handler()        = 0;
  virtual cu_cp_measurement_config_handler&      get_cu_cp_measurement_config_handler() = 0;
  virtual cu_cp_mobility_manager_handler&        get_cu_cp_mobility_manager_handler()   = 0;
  virtual cu_cp_ue_removal_handler&              get_cu_cp_ue_removal_handler()         = 0;
  virtual cu_cp_amf_reconnection_handler&        get_cu_cp_amf_reconnection_handler()   = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
