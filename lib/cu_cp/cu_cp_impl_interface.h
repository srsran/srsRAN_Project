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

#include "srsran/cu_cp/cell_meas_manager_config.h"
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
  virtual async_task<ngap_ng_setup_result> on_ng_setup_request(const ngap_ng_setup_request& request) = 0;

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

/// Interface for the NGAP notifier to communicate with the CU-CP.
class cu_cp_ngap_ue_creation_handler
{
public:
  virtual ~cu_cp_ngap_ue_creation_handler() = default;

  /// \brief Handle the creation of a new NGAP UE. This will add the NGAP adapters to the UE manager.
  /// \param[in] ue_index The index of the new NGAP UE.
  /// \returns True if the UE was successfully created, false otherwise.
  virtual bool handle_new_ngap_ue(ue_index_t ue_index) = 0;
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

/// Interface for entities (e.g. DU processor) that wish to manipulate the context of a UE.
class cu_cp_ue_context_manipulation_handler
{
public:
  virtual ~cu_cp_ue_context_manipulation_handler() = default;

  /// \brief Transfer and remove UE contexts for an ongoing Reestablishment/Handover.
  /// \param[in] ue_index The new UE index of the UE that sent the Reestablishment Request or is the target UE.
  /// \param[in] old_ue_index The old UE index of the UE that sent the Reestablishment Request or is the source UE.
  virtual async_task<bool> handle_ue_context_transfer(ue_index_t ue_index, ue_index_t old_ue_index) = 0;

  /// \brief Handle a UE context push during handover.
  /// \param[in] source_ue_index The index of the UE that is the source of the handover.
  /// \param[in] target_ue_index The index of the UE that is the target of the handover.
  virtual void handle_handover_ue_context_push(ue_index_t source_ue_index, ue_index_t target_ue_index) = 0;
};

/// Methods used by CU-CP to transfer the RRC UE context e.g. for RRC Reestablishments
class cu_cp_rrc_ue_context_transfer_notifier
{
public:
  virtual ~cu_cp_rrc_ue_context_transfer_notifier() = default;

  /// \brief Notifies the RRC UE to return the RRC Reestablishment UE context.
  virtual rrc_reestablishment_ue_context_t on_rrc_ue_context_transfer() = 0;
};

/// Interface to handle measurement requests
class cu_cp_measurement_handler
{
public:
  virtual ~cu_cp_measurement_handler() = default;

  /// \brief Handle a measurement config request (for any UE) connected to the given serving cell.
  /// \param[in] ue_index The index of the UE to update the measurement config for.
  /// \param[in] nci The cell id of the serving cell to update.
  /// \param[in] current_meas_config The current meas config of the UE (if applicable).
  virtual optional<rrc_meas_cfg> handle_measurement_config_request(ue_index_t             ue_index,
                                                                   nr_cell_id_t           nci,
                                                                   optional<rrc_meas_cfg> current_meas_config = {}) = 0;

  /// \brief Handle a measurement report for given UE.
  virtual void handle_measurement_report(const ue_index_t ue_index, const rrc_meas_results& meas_results) = 0;
};

/// Interface to handle measurement config update requests
class cu_cp_measurement_config_handler
{
public:
  virtual ~cu_cp_measurement_config_handler() = default;

  /// \brief Handle a request to update the measurement related parameters for the given cell id.
  /// \param[in] nci The cell id of the serving cell to update.
  /// \param[in] serv_cell_cfg_ The serving cell meas config to update.
  /// \param[in] ncells_ Optional neigbor cells to replace the current neighbor cells with.
  virtual void handle_cell_config_update_request(nr_cell_id_t                           nci,
                                                 const serving_cell_meas_config&        serv_cell_cfg,
                                                 std::vector<neighbor_cell_meas_config> ncells = {}) = 0;
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
                             public cu_cp_measurement_handler,
                             public cu_cp_measurement_config_handler,
                             public cu_cp_ngap_ue_creation_handler,
                             public cu_cp_ue_context_manipulation_handler,
                             public cu_cp_ue_removal_handler
{
public:
  virtual ~cu_cp_impl_interface() = default;

  virtual cu_cp_e1ap_handler&                    get_cu_cp_e1ap_handler()               = 0;
  virtual cu_cp_rrc_ue_interface&                get_cu_cp_rrc_ue_interface()           = 0;
  virtual cu_cp_ue_context_manipulation_handler& get_cu_cp_ue_context_handler()         = 0;
  virtual cu_cp_measurement_handler&             get_cu_cp_measurement_handler()        = 0;
  virtual cu_cp_measurement_config_handler&      get_cu_cp_measurement_config_handler() = 0;
  virtual cu_cp_ue_removal_handler&              get_cu_cp_ue_removal_handler()         = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
