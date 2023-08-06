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

#include "cu_cp_types.h"
#include "srsran/adt/optional.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/f1ap/cu_cp/f1ap_interface_management_types.h"
#include "srsran/ngap/ngap_handover.h"
#include "srsran/pdcp/pdcp_entity.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/rnti.h"
#include "srsran/rrc/rrc.h"
#include "srsran/rrc/rrc_du.h"
#include <string>

namespace srsran {
namespace srs_cu_cp {

/// Forward declared messages.
struct rrc_ue_creation_message;

/// Additional context of a SRB containing notifiers to PDCP, i.e. SRB1 and SRB2.
struct cu_srb_pdcp_context {
  std::unique_ptr<pdcp_entity>                    entity;
  std::unique_ptr<pdcp_tx_lower_notifier>         pdcp_tx_notifier;
  std::unique_ptr<pdcp_tx_upper_control_notifier> rrc_tx_control_notifier;
  std::unique_ptr<pdcp_rx_upper_data_notifier>    rrc_rx_data_notifier;
  std::unique_ptr<pdcp_rx_upper_control_notifier> rrc_rx_control_notifier;
  std::unique_ptr<rrc_tx_security_notifier>       rrc_tx_sec_notifier;
  std::unique_ptr<rrc_rx_security_notifier>       rrc_rx_sec_notifier;
};

/// Context for a SRB with adapters between DU processor, F1AP, RRC and optionally PDCP.
struct cu_srb_context {
  std::unique_ptr<f1ap_rrc_message_notifier> rx_notifier     = std::make_unique<f1ap_rrc_null_notifier>();
  std::unique_ptr<rrc_pdu_notifier>          rrc_tx_notifier = std::make_unique<rrc_pdu_null_notifier>();
  optional<cu_srb_pdcp_context>              pdcp_context;
};

/// Interface to request SRB creations at the DU processor.
class du_processor_srb_interface
{
public:
  virtual ~du_processor_srb_interface() = default;

  /// \brief Instruct the DU processor to create a new SRB for a given UE. Depending on the config it creates all
  /// required intermediate objects (e.g. PDCP) and connects them with one another.
  /// \param[in] msg The UE index, SRB ID and config.
  virtual void create_srb(const srb_creation_message& msg) = 0;
};

/// Interface for an F1AP notifier to communicate with the DU processor.
class du_processor_f1ap_interface : public du_processor_srb_interface
{
public:
  virtual ~du_processor_f1ap_interface() = default;

  /// \brief Get the DU index.
  /// \return The DU index.
  virtual du_index_t get_du_index() = 0;

  /// \brief Handle the reception of a F1 Setup Request message and transmit the F1 Setup Response or F1 Setup Failure.
  /// \param[in] msg The received F1 Setup Request message.
  virtual void handle_f1_setup_request(const f1ap_f1_setup_request& request) = 0;

  /// \brief Allocate a new UE index.
  virtual ue_index_t get_new_ue_index() = 0;

  /// \brief Create a new UE context.
  /// \param[in] msg The UE creation message.
  /// \return Returns a UE creation complete message containing the index of the created UE and its SRB notifiers.
  virtual ue_creation_complete_message handle_ue_creation_request(const cu_cp_ue_creation_message& msg) = 0;

  /// \brief Remove UE object from DU processor.
  virtual void remove_ue(ue_index_t ue_index) = 0;

  /// \brief Update existing UE object.
  virtual ue_update_complete_message handle_ue_update_request(const ue_update_message& msg) = 0;

  /// \brief Handle the reception of a F1AP UE Context Release Request and notify NGAP.
  /// \param[in] req The F1AP UE Context Release Request.
  virtual void handle_du_initiated_ue_context_release_request(const f1ap_ue_context_release_request& request) = 0;

  /// \brief Get the F1AP message handler interface of the DU processor object.
  /// \return The F1AP message handler interface of the DU processor object.
  virtual f1ap_message_handler& get_f1ap_message_handler() = 0;

  /// \brief Get the F1AP UE context management handler interface of the DU processor object.
  /// \return The F1AP UE context management handler interface of the DU processor object.
  virtual f1ap_ue_context_manager& get_f1ap_ue_context_manager() = 0;

  /// \brief Get the F1AP statistics handler interface of the DU processor object.
  /// \return The F1AP statistics handler interface of the DU processor object.
  virtual f1ap_statistics_handler& get_f1ap_statistics_handler() = 0;
};

/// Interface to notifiy UE context management procedures.
class du_processor_f1ap_ue_context_notifier
{
public:
  virtual ~du_processor_f1ap_ue_context_notifier() = default;

  /// Notify F1AP to establish the UE context.
  virtual async_task<f1ap_ue_context_setup_response>
  on_ue_context_setup_request(const f1ap_ue_context_setup_request& request) = 0;

  /// \brief Notify the F1AP to initiate the UE Context Release procedure.
  /// \param[in] msg The UE Context Release message to transmit.
  /// \return Returns the index of the released UE.
  virtual async_task<ue_index_t> on_ue_context_release_command(const f1ap_ue_context_release_command& msg) = 0;

  /// \brief Notify the F1AP to initiate the UE Context Modification procedure.
  /// \param[in] request The UE Context Modification message to transmit.
  /// \return Returns a f1ap_ue_context_modification_response_message struct with the success member set to
  /// 'true' in case of a successful outcome, 'false' otherwise.
  virtual async_task<f1ap_ue_context_modification_response>
  on_ue_context_modification_request(const f1ap_ue_context_modification_request& request) = 0;
};

/// Interface to notifiy Paging procedures.
class du_processor_f1ap_paging_notifier
{
public:
  virtual ~du_processor_f1ap_paging_notifier() = default;

  /// Notify F1AP to send paging message.
  virtual void on_paging_message(const cu_cp_paging_message& msg) = 0;
};

/// \brief Helpers to query information about DU.
class du_processor_cell_info_interface
{
public:
  virtual ~du_processor_cell_info_interface() = default;

  /// \brief Checks whether a cell with the specified PCI is served by the DU.
  virtual bool has_cell(pci_t pci) = 0;

  /// \brief Checks whether a cell with the specified NR cell global id is served by the DU.
  virtual bool has_cell(nr_cell_global_id_t cgi) = 0;
};

/// Interface for an RRC entity to communicate with the DU processor.
class du_processor_rrc_interface
{
public:
  virtual ~du_processor_rrc_interface() = default;

  /// \brief Get the RRC AMF connection handler interface of the DU processor object.
  /// \return The RRC AMF connection handler interface of the DU processor object.
  virtual rrc_amf_connection_handler& get_rrc_amf_connection_handler() = 0;
};

/// Interface to notifiy RRC DU about UE management procedures.
class du_processor_rrc_du_ue_notifier
{
public:
  virtual ~du_processor_rrc_du_ue_notifier() = default;

  /// \brief Notify RRC DU about served cells.
  /// \param[in] served_cell_list The list of served cells.
  /// \return Returns true on success, false otherwise.
  virtual bool on_new_served_cell_list(const std::vector<cu_cp_du_served_cells_item>& served_cell_list) = 0;

  /// \brief Notify RRC DU to create a UE.
  /// \param[in] resource_mng The UP resource manager of this UE.
  /// \param[in] msg The UE creation message.
  /// \return Returns a handle to the created UE.
  virtual rrc_ue_interface* on_ue_creation_request(up_resource_manager&           resource_mng,
                                                   const rrc_ue_creation_message& msg) = 0;

  /// \brief Notify the RRC DU to release a UE.
  /// \param[in] ue_index The index of the UE object to remove.
  virtual void on_ue_context_release_command(ue_index_t ue_index) = 0;

  /// Send RRC Release to all UEs connected to this DU.
  virtual void on_release_ues() = 0;
};

/// Interface for an RRC UE entity to communicate with the DU processor.
class du_processor_rrc_ue_interface : public du_processor_srb_interface
{
public:
  virtual ~du_processor_rrc_ue_interface() = default;

  /// \brief Handle a UE Context Release Command
  /// \param[in] cmd The UE Context Release Command.
  virtual void handle_ue_context_release_command(const rrc_ue_context_release_command& cmd) = 0;

  /// \brief Handle a required reestablishment context modification.
  /// \param[in] ue_index The index of the UE that needs the context modification.
  virtual async_task<bool> handle_rrc_reestablishment_context_modification_required(ue_index_t ue_index) = 0;
};

/// Interface to notify an RRC UE about control messages.
class du_processor_rrc_ue_control_message_notifier
{
public:
  virtual ~du_processor_rrc_ue_control_message_notifier() = default;

  /// \brief Notify the RRC UE to trigger a UE capability transfer procedure.
  /// \param[in] msg The new request msg containing the RAT type, etc.
  virtual async_task<bool> on_ue_capability_transfer_request(const rrc_ue_capability_transfer_request& msg) = 0;

  /// \brief Notify the RRC UE about an RRC Reconfiguration Request.
  /// \param[in] msg The new RRC Reconfiguration Request.
  /// \returns The result of the rrc reconfiguration.
  virtual async_task<bool> on_rrc_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg) = 0;

  /// \brief Notify the source RRC UE about an RRC Reconfiguration Request for a handover.
  /// \param[in] msg The new RRC Reconfiguration Request.
  /// \returns The transaction ID of the RRC Reconfiguration request.
  virtual uint8_t on_handover_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg) = 0;

  /// \brief Notify the target RRC UE to await a RRC Reconfiguration Complete for a handover.
  /// \param[in] transaction_id The transaction ID of the RRC Reconfiguration Complete.
  /// \returns True if the RRC Reconfiguration Complete was received, false otherwise.
  virtual async_task<bool> on_handover_reconfiguration_complete_expected(uint8_t transaction_id) = 0;

  /// \brief Get the RRC UE release context.
  /// \returns The release context of the UE.
  virtual rrc_ue_release_context get_rrc_ue_release_context() = 0;

  /// \brief Get the RRC measurement config for the current serving cell of the UE.
  /// \return The measurement config, if present.
  virtual optional<rrc_meas_cfg> get_rrc_ue_meas_config() = 0;
};

/// Interface used by mobility manager to trigger handover routines.
class du_processor_mobility_handler
{
public:
  virtual ~du_processor_mobility_handler() = default;

  /// \brief Retrieve CGI for a given PCI of a DU.
  virtual optional<nr_cell_global_id_t> get_cgi(pci_t pci) = 0;

  /// \brief Handle an Inter DU handover.
  virtual async_task<cu_cp_inter_du_handover_response>
  handle_inter_du_handover_request(const cu_cp_inter_du_handover_request& request,
                                   du_processor_f1ap_ue_context_notifier& target_du_f1ap_ue_ctxt_notifier) = 0;

  /// \brief Start the inter NG-RAN node N2 handover procedure at the source gNB.
  /// See TS 23.502 section 4.9.1.3.
  virtual async_task<cu_cp_inter_ngran_node_n2_handover_response>
  handle_inter_ngran_node_n2_handover_request(const cu_cp_inter_ngran_node_n2_handover_request& request) = 0;

  /// \brief Handle the handover request of the handover resource allocation procedure handover procedure.
  /// See TS 38.413 section 8.4.2.2.
  virtual async_task<ngap_handover_resource_allocation_response>
  handle_ngap_handover_request(const ngap_handover_request& request) = 0;
};

/// Handler for an NGAP entity to communicate with the DU processor
class du_processor_ngap_interface
{
public:
  virtual ~du_processor_ngap_interface() = default;

  /// \brief Allocate a new UE index.
  virtual ue_index_t get_new_ue_index() = 0;

  /// \brief Handle the reception of a new PDU Session Resource Setup Request.
  virtual async_task<cu_cp_pdu_session_resource_setup_response>
  handle_new_pdu_session_resource_setup_request(const cu_cp_pdu_session_resource_setup_request& msg) = 0;

  /// \brief Handle the reception of a new PDU Session Resource Modify Request.
  virtual async_task<cu_cp_pdu_session_resource_modify_response>
  handle_new_pdu_session_resource_modify_request(const cu_cp_pdu_session_resource_modify_request& msg) = 0;

  /// \brief Handle the reception of a new PDU Session Resource Release Command.
  virtual async_task<cu_cp_pdu_session_resource_release_response>
  handle_new_pdu_session_resource_release_command(const cu_cp_pdu_session_resource_release_command& msg) = 0;

  /// \brief Handle a UE Context Release Command.
  /// \param[in] cmd The UE Context Release Command.
  virtual cu_cp_ue_context_release_complete
  handle_new_ue_context_release_command(const cu_cp_ngap_ue_context_release_command& cmd) = 0;
};

/// Interface to notify the NGAP about control messages.
class du_processor_ngap_control_notifier
{
public:
  virtual ~du_processor_ngap_control_notifier() = default;

  /// \brief Notify the NGAP to request a UE release e.g. due to inactivity.
  /// \param[in] msg The UE Context Release Request.
  virtual void on_ue_context_release_request(const cu_cp_ue_context_release_request& msg) = 0;

  virtual async_task<ngap_handover_preparation_response>
  on_ngap_handover_preparation_request(const ngap_handover_preparation_request& req) = 0;
};

/// Interface to notify the E1AP about control messages.
class du_processor_e1ap_control_notifier
{
public:
  virtual ~du_processor_e1ap_control_notifier() = default;

  /// \brief Notify about the reception of a new Bearer Context Setup Request.
  virtual async_task<e1ap_bearer_context_setup_response>
  on_bearer_context_setup_request(const e1ap_bearer_context_setup_request& request) = 0;

  /// \brief Notify about the reception of a new Bearer Context Modification Request.
  virtual async_task<e1ap_bearer_context_modification_response>
  on_bearer_context_modification_request(const e1ap_bearer_context_modification_request& request) = 0;

  /// \brief Notify about the reception of a new Bearer Context Release Command.
  virtual async_task<void> on_bearer_context_release_command(const e1ap_bearer_context_release_command& cmd) = 0;
};

/// Interface to notify the F1AP about control messages.
class du_processor_f1ap_control_notifier
{
public:
  virtual ~du_processor_f1ap_control_notifier() = default;

  /// \brief Notify about the reception of a new PDU Session Resource Setup List.
  virtual async_task<f1ap_ue_context_modification_response>
  on_new_pdu_session_resource_setup_request(f1ap_ue_context_modification_request& msg) = 0;
};

/// \brief Schedules asynchronous tasks associated with an UE.
class du_processor_ue_task_scheduler
{
public:
  virtual ~du_processor_ue_task_scheduler()                                                = default;
  virtual void           schedule_async_task(ue_index_t ue_index, async_task<void>&& task) = 0;
  virtual unique_timer   make_unique_timer()                                               = 0;
  virtual timer_manager& get_timer_manager()                                               = 0;
};

/// \brief Handles incoming task scheduling requests associated with an UE.
class du_processor_ue_task_handler
{
public:
  virtual ~du_processor_ue_task_handler()                                                   = default;
  virtual void           handle_ue_async_task(ue_index_t ue_index, async_task<void>&& task) = 0;
  virtual unique_timer   make_unique_timer()                                                = 0;
  virtual timer_manager& get_timer_manager()                                                = 0;
};

/// Methods used by DU processor to notify about DU specific events.
class du_processor_cu_cp_notifier
{
public:
  virtual ~du_processor_cu_cp_notifier() = default;

  /// \brief Notifies about a successful RRC UE creation.
  /// \param[in] du_index The index of the DU the UE is connected to.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] rrc_ue_msg_handler The created RRC UE.
  virtual void on_rrc_ue_created(du_index_t du_index, ue_index_t ue_index, rrc_ue_interface& rrc_ue) = 0;
};

/// DU processor Paging handler.
class du_processor_paging_handler
{
public:
  virtual ~du_processor_paging_handler() = default;

  /// \brief Handles a Paging message.
  virtual void handle_paging_message(cu_cp_paging_message& msg) = 0;
};

/// DU processor inactivity handler.
class du_processor_inactivity_handler
{
public:
  virtual ~du_processor_inactivity_handler() = default;

  /// \brief Handles an Inactivity notification message.
  virtual void handle_inactivity_notification(const cu_cp_inactivity_notification& msg) = 0;
};

class du_processor_ue_handler
{
public:
  virtual ~du_processor_ue_handler() = default;

  /// \brief Removes a UE from the RRC and DU Processor.
  virtual void remove_ue(ue_index_t ue_index) = 0;
};

/// Methods to get statistics of the DU processor.
class du_processor_statistics_handler
{
public:
  virtual ~du_processor_statistics_handler() = default;

  /// \brief Returns the number of connected UEs at the DU processor
  /// \return The number of connected UEs.
  virtual size_t get_nof_ues() = 0;
};

class du_processor_interface : public du_processor_f1ap_interface,
                               public du_processor_rrc_interface,
                               public du_processor_rrc_ue_interface,
                               public du_processor_cell_info_interface,
                               public du_processor_ngap_interface,
                               public du_processor_ue_task_handler,
                               public du_processor_paging_handler,
                               public du_processor_inactivity_handler,
                               public du_processor_statistics_handler,
                               public du_processor_ue_handler,
                               public du_processor_mobility_handler

{
public:
  virtual ~du_processor_interface() = default;

  virtual du_processor_f1ap_interface&           get_du_processor_f1ap_interface()           = 0;
  virtual du_processor_rrc_interface&            get_du_processor_rrc_interface()            = 0;
  virtual du_processor_rrc_ue_interface&         get_du_processor_rrc_ue_interface()         = 0;
  virtual du_processor_ngap_interface&           get_du_processor_ngap_interface()           = 0;
  virtual du_processor_ue_task_handler&          get_du_processor_ue_task_handler()          = 0;
  virtual du_processor_paging_handler&           get_du_processor_paging_handler()           = 0;
  virtual du_processor_inactivity_handler&       get_du_processor_inactivity_handler()       = 0;
  virtual du_processor_statistics_handler&       get_du_processor_statistics_handler()       = 0;
  virtual du_processor_ue_handler&               get_du_processor_ue_handler()               = 0;
  virtual du_processor_mobility_handler&         get_du_processor_mobility_handler()         = 0;
  virtual du_processor_f1ap_ue_context_notifier& get_du_processor_f1ap_ue_context_notifier() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
