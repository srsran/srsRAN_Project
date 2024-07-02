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

#include "../ue_manager/ue_manager_impl.h"
#include "srsran/cu_cp/cu_cp_command_handler.h"
#include "srsran/cu_cp/cu_cp_f1c_handler.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/mobility_manager_config.h"

namespace srsran {
namespace srs_cu_cp {

class du_processor_repository;

/// Handler for measurement related events.
class mobility_manager_measurement_handler
{
public:
  virtual ~mobility_manager_measurement_handler() = default;

  /// \brief Handle event where neighbor became better than serving cell.
  virtual void handle_neighbor_better_than_spcell(ue_index_t       ue_index,
                                                  gnb_id_t         neighbor_gnb_id,
                                                  nr_cell_identity neighbor_nci,
                                                  pci_t            neighbor_pci) = 0;
};

/// Methods used by mobility manager to signal handover events to the CU-CP.
class mobility_manager_cu_cp_notifier
{
public:
  virtual ~mobility_manager_cu_cp_notifier() = default;

  /// \brief Notify the CU-CP about an required inter-DU handover.
  virtual async_task<cu_cp_inter_du_handover_response>
  on_inter_du_handover_required(const cu_cp_inter_du_handover_request& request,
                                du_index_t                             source_du_index,
                                du_index_t                             target_du_index) = 0;
};

/// Basic cell manager implementation
class mobility_manager final : public mobility_manager_measurement_handler, public cu_cp_mobility_command_handler
{
public:
  mobility_manager(const mobility_manager_cfg&      cfg,
                   mobility_manager_cu_cp_notifier& cu_cp_notifier_,
                   ngap_control_message_handler&    ngap_handler_,
                   du_processor_repository&         du_db_,
                   ue_manager&                      ue_mng_);

  void trigger_handover(pci_t source_pci, rnti_t rnti, pci_t target_pci) override;

  void handle_neighbor_better_than_spcell(ue_index_t       ue_index,
                                          gnb_id_t         neighbor_gnb_id,
                                          nr_cell_identity neighbor_nci,
                                          pci_t            neighbor_pci) override;

private:
  void
  handle_handover(ue_index_t ue_index, gnb_id_t neighbor_gnb_id, nr_cell_identity neighbor_nci, pci_t neighbor_pci);
  void handle_inter_cu_handover(ue_index_t source_ue_index, gnb_id_t target_gnb_id, nr_cell_identity target_nci);
  void handle_inter_du_handover(ue_index_t source_ue_index,
                                pci_t      neighbor_pci,
                                du_index_t source_du_index,
                                du_index_t target_du_index);
  void handle_intra_du_handover(ue_index_t source_ue_index, pci_t neighbor_pci);

  mobility_manager_cfg             cfg;
  mobility_manager_cu_cp_notifier& cu_cp_notifier;
  ngap_control_message_handler&    ngap_handler;
  du_processor_repository&         du_db;
  ue_manager&                      ue_mng;

  srslog::basic_logger& logger;
};

} // namespace srs_cu_cp
} // namespace srsran
