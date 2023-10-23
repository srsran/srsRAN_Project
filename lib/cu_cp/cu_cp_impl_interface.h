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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ngap/ngap.h"
#include "srsran/rrc/rrc_du.h"
#include "srsran/rrc/rrc_ue.h"
#include <string>

namespace srsran {
namespace srs_cu_cp {

/// Methods used by CU-CP to initiate NGAP connection procedures.
class cu_cp_ngap_control_notifier
{
public:
  virtual ~cu_cp_ngap_control_notifier() = default;

  /// \brief Notifies the NGAP to initiate a NG Setup Procedure.
  /// \param[in] request The NG Setup Request.
  virtual async_task<ng_setup_response> on_ng_setup_request(const ng_setup_request& request) = 0;

  /// \brief Notify the NGAP to request a UE release e.g. due to inactivity.
  /// \param[in] msg The UE Context Release Request.
  virtual void on_ue_context_release_request(const cu_cp_ue_context_release_request& request) = 0;

  /// \brief Remove the context of a UE at the NGAP.
  /// \param[in] ue_index The index of the UE to remove.
  virtual void remove_ue(ue_index_t ue_index) = 0;
};

/// Methods used by CU-CP to request NGAP statistics
class cu_cp_ngap_statistics_notifier
{
public:
  virtual ~cu_cp_ngap_statistics_notifier() = default;

  /// \brief Get the number of UEs registered at the NGAP.
  /// \return The number of UEs.
  virtual size_t get_nof_ues() const = 0;
};

/// Interface for the NGAP to interface with the DU repository
/// Useful for paging and handover
class cu_cp_du_repository_ngap_handler
{
public:
  virtual ~cu_cp_du_repository_ngap_handler() = default;

  /// \brief Handles a Paging message notification.
  virtual void handle_paging_message(cu_cp_paging_message& msg) = 0;

  /// \brief Handles UE index allocation request for N2 handover at target gNB
  virtual ue_index_t handle_ue_index_allocation_request(const nr_cell_global_id_t& cgi) = 0;

  /// \brief Handles a handover request to start the ngap handover routine at the target CU
  virtual async_task<ngap_handover_resource_allocation_response>
  handle_ngap_handover_request(const ngap_handover_request& request) = 0;
};

/// Interface for an E1AP notifier to communicate with the CU-CP.
class cu_cp_e1ap_handler
{
public:
  virtual ~cu_cp_e1ap_handler() = default;

  /// \brief Handle the creation of an E1AP.
  /// \param[in] bearer_context_manager The E1AP Bearer Context Manager interface.
  /// \param[in] bearer_removal_handler The E1AP bearer context removal handler.
  /// \param[in] e1ap_statistic_handler The E1AP statistic interface.
  virtual void handle_e1ap_created(e1ap_bearer_context_manager&         bearer_context_manager,
                                   e1ap_bearer_context_removal_handler& bearer_removal_handler,
                                   e1ap_statistics_handler&             e1ap_statistic_handler) = 0;

  /// \brief Handle the reception of an Bearer Context Inactivity Notification message.
  /// \param[in] msg The received Bearer Context Inactivity Notification message.
  virtual void handle_bearer_context_inactivity_notification(const cu_cp_inactivity_notification& msg) = 0;
};

/// Methods used by CU-CP to request removal of the UE context at the E1AP
class cu_cp_e1ap_ue_removal_notifier
{
public:
  virtual ~cu_cp_e1ap_ue_removal_notifier() = default;

  /// \brief Remove the context of a UE at the E1AP.
  /// \param[in] ue_index The index of the UE to remove.
  virtual void remove_ue(ue_index_t ue_index) = 0;
};

/// Methods used by CU-CP to request E1AP statistics
class cu_cp_e1ap_statistics_notifier
{
public:
  virtual ~cu_cp_e1ap_statistics_notifier() = default;

  /// \brief Get the number of UEs registered at the E1AP.
  /// \return The number of UEs.
  virtual size_t get_nof_ues() const = 0;
};

/// Methods used by CU-CP to request removal of the UE context at the F1AP
class cu_cp_f1ap_ue_removal_notifier
{
public:
  virtual ~cu_cp_f1ap_ue_removal_notifier() = default;

  /// \brief Remove the context of a UE at the F1AP.
  /// \param[in] ue_index The index of the UE to remove.
  virtual void remove_ue(ue_index_t ue_index) = 0;
};

/// Methods used by CU-CP to request F1AP statistics
class cu_cp_f1ap_statistics_notifier
{
public:
  virtual ~cu_cp_f1ap_statistics_notifier() = default;

  /// \brief Get the number of UEs registered at the F1AP.
  /// \return The number of UEs.
  virtual size_t get_nof_ues() const = 0;
};

/// Methods used by CU-CP to request removal of the RRC UE context at the RRD DU
class cu_cp_rrc_ue_removal_notifier
{
public:
  virtual ~cu_cp_rrc_ue_removal_notifier() = default;

  /// \brief Remove the context of a UE at the RRC DU.
  /// \param[in] ue_index The index of the UE to remove.
  virtual void remove_ue(ue_index_t ue_index) = 0;
};

/// Methods used by CU-CP to request RRC DU statistics
class cu_cp_rrc_du_statistics_notifier
{
public:
  virtual ~cu_cp_rrc_du_statistics_notifier() = default;

  /// \brief Get the number of UEs registered at the RRC DU.
  /// \return The number of UEs.
  virtual size_t get_nof_ues() const = 0;
};

/// Interface used to handle DU specific procedures
class cu_cp_du_event_handler
{
public:
  virtual ~cu_cp_du_event_handler() = default;

  /// \brief Handle about a successful F1AP and RRC creation.
  /// \param[in] du_index The index of the DU the UE is connected to.
  /// \param[in] f1ap_handler Handler to the F1AP to initiate the UE context removal.
  /// \param[in] f1ap_statistic_handler Handler to the F1AP statistic interface.
  /// \param[in] rrc_handler Handler to the RRC DU to initiate the RRC UE removal.
  /// \param[in] rrc_statistic_handler Handler to the RRC DU statistic interface.
  virtual void handle_du_processor_creation(du_index_t                       du_index,
                                            f1ap_ue_context_removal_handler& f1ap_handler,
                                            f1ap_statistics_handler&         f1ap_statistic_handler,
                                            rrc_ue_removal_handler&          rrc_handler,
                                            rrc_du_statistics_handler&       rrc_statistic_handler) = 0;

  /// \brief Handle a RRC UE creation notification from the DU processor.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] rrc_ue The interface of the created RRC UE.
  /// \param[in] ngap_to_du_ev_notifier The notifier used by the NGAP to signal DU-specific events.
  virtual void handle_rrc_ue_creation(ue_index_t                          ue_index,
                                      rrc_ue_interface&                   rrc_ue,
                                      ngap_du_processor_control_notifier& ngap_to_du_ev_notifier) = 0;
};

/// Interface for an RRC UE entity to communicate with the CU-CP.
class cu_cp_rrc_ue_interface
{
public:
  virtual ~cu_cp_rrc_ue_interface() = default;

  /// \brief Handle the reception of an RRC Reestablishment Request by transfering UE Contexts at the RRC.
  /// \param[in] old_pci The old PCI contained in the RRC Reestablishment Request.
  /// \param[in] old_c_rnti The old C-RNTI contained in the RRC Reestablishment Request.
  /// \param[in] ue_index The new UE index of the UE that sent the Reestablishment Request.
  /// \returns The RRC Reestablishment UE context for the old UE.
  virtual rrc_reestablishment_ue_context_t
  handle_rrc_reestablishment_request(pci_t old_pci, rnti_t old_c_rnti, ue_index_t ue_index) = 0;

  /// \brief Transfer and remove UE contexts for an ongoing Reestablishment.
  /// \param[in] ue_index The new UE index of the UE that sent the Reestablishment Request.
  /// \param[in] old_ue_index The old UE index of the UE that sent the Reestablishment Request.
  virtual async_task<bool> handle_ue_context_transfer(ue_index_t ue_index, ue_index_t old_ue_index) = 0;
};

/// Methods used by CU-CP to transfer the RRC UE context e.g. for RRC Reestablishments
class cu_cp_rrc_ue_context_transfer_notifier
{
public:
  virtual ~cu_cp_rrc_ue_context_transfer_notifier() = default;

  /// \brief Notifies the RRC UE to return the RRC Reestablishment UE context.
  virtual rrc_reestablishment_ue_context_t on_rrc_ue_context_transfer() = 0;
};

/// Interface to request handover.
class cu_cp_mobility_manager_handler
{
public:
  virtual ~cu_cp_mobility_manager_handler() = default;

  /// \brief Handle an Inter DU handover request.
  /// \param[in] ue_index The UE index to be handed over to the new cell.
  /// \param[in] target_pci The PCI of the target cell.
  virtual void handle_inter_du_handover_request(ue_index_t ue_index, pci_t target_pci) = 0;
};

/// Interface to handle ue removals
class cu_cp_ue_removal_handler
{
public:
  virtual ~cu_cp_ue_removal_handler() = default;

  /// \brief Completly remove a UE from the CU-CP.
  /// \param[in] ue_index The index of the UE to remove.
  virtual void handle_ue_removal_request(ue_index_t ue_index) = 0;
};

class cu_cp_impl_interface : public cu_cp_e1ap_handler,
                             public cu_cp_du_event_handler,
                             public cu_cp_rrc_ue_interface,
                             public cu_cp_ue_removal_handler
{
public:
  virtual ~cu_cp_impl_interface() = default;

  virtual cu_cp_e1ap_handler&       get_cu_cp_e1ap_handler()       = 0;
  virtual cu_cp_rrc_ue_interface&   get_cu_cp_rrc_ue_interface()   = 0;
  virtual cu_cp_ue_removal_handler& get_cu_cp_ue_removal_handler() = 0;

  virtual void start() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
