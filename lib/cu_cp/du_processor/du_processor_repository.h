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

#include "../adapters/du_processor_adapters.h"
#include "../adapters/f1ap_adapters.h"
#include "../adapters/ngap_adapters.h"
#include "../cu_cp_impl_interface.h"
#include "../ue_manager/ue_manager_impl.h"
#include "du_configuration_manager.h"
#include "du_metrics_handler.h"
#include "srsran/cu_cp/common_task_scheduler.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/async/async_task.h"
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

struct cu_cp_configuration;

struct du_repository_config {
  const cu_cp_configuration&             cu_cp;
  cu_cp_du_event_handler&                cu_cp_du_handler;
  cu_cp_measurement_config_handler&      meas_config_handler;
  cu_cp_ue_removal_handler&              ue_removal_handler;
  cu_cp_ue_context_manipulation_handler& ue_context_handler;
  common_task_scheduler&                 common_task_sched;
  ue_manager&                            ue_mng;
  du_connection_notifier&                du_conn_notif;
  srslog::basic_logger&                  logger;
};

class du_processor_repository : public du_repository_metrics_handler
{
public:
  explicit du_processor_repository(du_repository_config cfg_);

  /// \brief Checks whether a cell with the specified PCI is served by any of the connected DUs.
  /// \param[in] pci The serving cell PCI.
  /// \return The index of the DU serving the given PCI.
  du_index_t find_du(pci_t pci);

  /// \brief Checks whether a cell with the specified CGI is served by any of the connected DUs.
  /// \param[in] cgi The serving cell CGI.
  /// \return The index of the DU serving the given CGI.
  du_index_t find_du(const nr_cell_global_id_t& cgi);

  /// \brief Find a DU object.
  /// \param[in] du_index The index of the DU processor object.
  /// \return A pointer to the DU processor object, nullptr if the DU processor object is not found.
  du_processor* find_du_processor(du_index_t du_index);

  /// \brief Find a DU object.
  /// \param[in] du_index The index of the DU processor object.
  /// \return The DU processor object.
  du_processor& get_du_processor(du_index_t du_index);

  std::vector<du_index_t> get_du_processor_indexes() const;

  std::vector<cu_cp_metrics_report::du_info> handle_du_metrics_report_request() const override;

  size_t get_nof_f1ap_ues();

  size_t get_nof_rrc_ues();

  /// \brief Adds a DU processor object to the CU-CP.
  /// \return The DU index of the added DU processor object.
  du_index_t add_du(std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier);

  /// \brief Launches task that removes the specified DU processor object from the CU-CP.
  /// \param[in] du_index The index of the DU processor to delete.
  /// \return asynchronous task for the DU processor removal.
  async_task<void> remove_du(du_index_t du_index);

  /// Number of DUs managed by the CU-CP.
  size_t get_nof_dus() const { return du_db.size(); }

private:
  struct du_context {
    // CU-CP handler of DU processor events.
    du_processor_cu_cp_adapter du_to_cu_cp_notifier;

    std::unique_ptr<du_processor> processor;

    /// Notifier used by the CU-CP to push F1AP Tx messages to the respective DU.
    std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier;
  };

  /// \brief Get the next available index from the DU processor database.
  /// \return The DU index.
  du_index_t get_next_du_index();

  du_repository_config  cfg;
  srslog::basic_logger& logger;

  std::map<du_index_t, du_context> du_db;

  du_configuration_manager du_cfg_mng;
};

} // namespace srs_cu_cp
} // namespace srsran
