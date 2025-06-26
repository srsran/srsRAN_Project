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

class du_processor_configuration_update_interface
{
public:
  virtual ~du_processor_configuration_update_interface() = default;

  /// \brief Apply neccesary configuration changes and forward the GNB CU Configuration Update to the F1AP.
  /// \param[in] request The F1AP GNB CU Configuration Update.
  /// \return The F1AP GNB CU Configuration Update response.
  virtual async_task<f1ap_gnb_cu_configuration_update_response>
  handle_configuration_update(const f1ap_gnb_cu_configuration_update& request) = 0;
};

class du_processor : public du_processor_cell_info_interface, public du_processor_configuration_update_interface
{
public:
  virtual ~du_processor() = default;

  /// \brief Retrieve F1AP handler for the respective DU.
  virtual f1ap_cu& get_f1ap_handler() = 0;

  /// \brief Retrieve RRC DU handler for the respective DU.
  virtual rrc_du& get_rrc_du_handler() = 0;

  virtual du_processor_mobility_handler& get_mobility_handler() = 0;

  /// \brief Retrieve the DU-specific metrics handler.
  virtual du_metrics_handler& get_metrics_handler() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
