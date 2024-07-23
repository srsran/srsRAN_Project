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

#include "du_configuration_handler.h"
#include "du_metrics_handler.h"
#include "srsran/adt/static_vector.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/rrc/rrc_du.h"
#include <string>

namespace srsran {
namespace srs_cu_cp {

/// Forward declared messages.
struct rrc_ue_creation_message;

/// Interface to notify UE context management procedures.
class du_processor_f1ap_ue_context_notifier
{
public:
  virtual ~du_processor_f1ap_ue_context_notifier() = default;

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

  /// \brief Notify the F1AP that the given UE corresponds to a reestablishment session of the old UE.
  /// \param[in] ue_index The index of the UE that is performing a reestablishment.
  /// \param[in] old_ue_index The index of the UE that
  virtual bool on_intra_du_reestablishment(ue_index_t ue_index, ue_index_t old_ue_index) = 0;
};

/// Interface to notify Paging procedures.
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

  /// \brief Get DU configuration context.
  virtual const du_configuration_context* get_context() const = 0;
};

/// Interface to notify RRC DU about UE management procedures.
class du_processor_rrc_du_ue_notifier
{
public:
  virtual ~du_processor_rrc_du_ue_notifier() = default;

  /// \brief Notify RRC DU about served cells.
  /// \param[in] served_cell_list The list of served cells.
  /// \return Returns true on success, false otherwise.
  virtual bool on_new_served_cell_list(const std::vector<cu_cp_du_served_cells_item>& served_cell_list) = 0;

  /// \brief Notify RRC DU about a required RRCReject.
  /// \return Returns a RRC Container containing the RRCReject.
  virtual byte_buffer on_rrc_reject_required() = 0;

  /// \brief Notify RRC DU to create a UE.
  /// \param[in] msg The UE creation message.
  /// \return Returns a handle to the created UE.
  virtual rrc_ue_interface* on_ue_creation_request(const rrc_ue_creation_message& msg) = 0;

  /// Send RRC Release to all UEs connected to this DU.
  virtual void on_release_ues() = 0;
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

  /// \brief Get the packed UE Capability RAT Container List.
  /// \returns The packed UE Capability RAT Container List.
  virtual byte_buffer get_packed_ue_capability_rat_container_list() = 0;

  /// \brief Request the RRC Handover Reconfiguration Context.
  /// \returns The RRC Handover Reconfiguration Context.
  virtual rrc_ue_handover_reconfiguration_context
  get_rrc_ue_handover_reconfiguration_context(const rrc_reconfiguration_procedure_request& request) = 0;

  /// \brief Notify the target RRC UE to await a RRC Reconfiguration Complete for a handover.
  /// \param[in] transaction_id The transaction ID of the RRC Reconfiguration Complete.
  /// \returns True if the RRC Reconfiguration Complete was received, false otherwise.
  virtual async_task<bool> on_handover_reconfiguration_complete_expected(uint8_t transaction_id) = 0;

  /// \brief Get the RRC UE release context.
  /// \returns The release context of the UE.
  virtual rrc_ue_release_context get_rrc_ue_release_context(bool requires_rrc_message) = 0;

  /// \brief Get all mobility related information of an UE required for reestablishment, handover, etc.
  /// \returns The mobility context of the UE.
  virtual rrc_ue_transfer_context get_transfer_context() = 0;

  /// \brief (Re-)generate the RRC measurement config for the current serving cell of the UE.
  /// \params[in] current_meas_config The current meas config of the UE (if applicable).
  /// \return The measurement config, if present.
  virtual std::optional<rrc_meas_cfg> generate_meas_config(std::optional<rrc_meas_cfg> current_meas_config = {}) = 0;

  /// \brief Request the packed Handover Preparation Message.
  virtual byte_buffer get_packed_handover_preparation_message() = 0;

  /// \brief Notify about the reception of a new Handover Command PDU.
  /// \param[in] cmd The handover command RRC PDU.
  /// \returns The RRC Handover Reconfiguration PDU. If the Handover Command PDU is invalid, an empty buffer is
  /// returned.
  virtual byte_buffer on_new_rrc_handover_command(byte_buffer cmd) = 0;

  /// \brief Request the RRC Handover Command PDU.
  /// \returns The RRC Handover Command PDU.
  virtual byte_buffer on_rrc_handover_command_required(const rrc_reconfiguration_procedure_request& request,
                                                       unsigned                                     transaction_id) = 0;
};

/// Interface to notify an RRC UE about SRB control queries (e.g. SRB creation).
class du_processor_rrc_ue_srb_control_notifier
{
public:
  virtual ~du_processor_rrc_ue_srb_control_notifier() = default;

  /// \brief Create an SRB at the target RRC UE.
  virtual void create_srb(const srb_creation_message& msg) = 0;

  /// \brief Get all SRBs of the UE.
  virtual static_vector<srb_id_t, MAX_NOF_SRBS> get_srbs() = 0;
};

/// Interface used by mobility manager to trigger handover routines.
class du_processor_mobility_handler
{
public:
  virtual ~du_processor_mobility_handler() = default;

  /// \brief Retrieve CGI for a given PCI of a DU.
  virtual std::optional<nr_cell_global_id_t> get_cgi(pci_t pci) = 0;

  /// \brief Retrieve the SIB1 for a given PCI of a DU.
  virtual byte_buffer get_packed_sib1(nr_cell_global_id_t cgi) = 0;
};

/// Methods used by DU processor to notify about DU specific events.
class du_processor_cu_cp_notifier
{
public:
  virtual ~du_processor_cu_cp_notifier() = default;

  /// \brief Notifies about a successful F1AP and RRC creation.
  /// \param[in] du_index The index of the DU the UE is connected to.
  /// \param[in] f1ap_handler Handler to the F1AP to initiate the UE context removal.
  /// \param[in] f1ap_statistic_handler Handler to the F1AP statistic interface.
  /// \param[in] rrc_handler Handler to the RRC DU to initiate the RRC UE removal.
  /// \param[in] rrc_statistic_handler Handler to the RRC DU statistic interface.
  virtual void on_du_processor_created(du_index_t                       du_index,
                                       f1ap_ue_context_removal_handler& f1ap_handler,
                                       f1ap_statistics_handler&         f1ap_statistic_handler,
                                       rrc_ue_handler&                  rrc_handler,
                                       rrc_du_statistics_handler&       rrc_statistic_handler) = 0;

  /// \brief Notifies about a successful RRC UE creation.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] rrc_ue_msg_handler The created RRC UE.
  virtual void on_rrc_ue_created(ue_index_t ue_index, rrc_ue_interface& rrc_ue) = 0;

  /// \brief Notify the CU-CP that the SIB1 for a given PCI of a DU is required.
  /// \param[in] du_index The index of the DU the cell is connected to.
  /// \param[in] cgi The cell global id of the target cell.
  /// \returns The packed SIB1 for the target cell, if available. An empty byte_buffer otherwise.
  virtual byte_buffer on_target_cell_sib1_required(du_index_t du_index, nr_cell_global_id_t cgi) = 0;

  /// \brief Notify the CU-CP to completly remove a UE from the CU-CP.
  /// \param[in] ue_index The index of the UE to remove.
  virtual async_task<void> on_ue_removal_required(ue_index_t ue_index) = 0;

  /// \brief Notify the CU-CP to release a UE.
  /// \param[in] request The release request.
  virtual async_task<void> on_ue_release_required(const cu_cp_ue_context_release_request& request) = 0;

  /// \brief Notify that the F1 transaction information was lost for some UEs.
  virtual async_task<void> on_transaction_info_loss(const f1_ue_transaction_info_loss_event& ev) = 0;
};

class du_processor : public du_processor_cell_info_interface
{
public:
  virtual ~du_processor() = default;

  /// \brief Retrieve F1AP handler for the respective DU.
  virtual f1ap_cu& get_f1ap_handler() = 0;

  virtual du_processor_mobility_handler& get_mobility_handler() = 0;

  /// \brief Get the F1AP message handler interface of the DU processor object.
  /// \return The F1AP message handler interface of the DU processor object.
  virtual du_processor_f1ap_ue_context_notifier& get_f1ap_ue_context_notifier() = 0;

  /// \brief Retrieve the DU-specific metrics handler.
  virtual du_metrics_handler& get_metrics_handler() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
