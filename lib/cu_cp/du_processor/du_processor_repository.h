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

#include "../adapters/du_processor_adapters.h"
#include "../adapters/f1ap_adapters.h"
#include "../adapters/ngap_adapters.h"
#include "../cu_cp_controller/cu_cp_controller.h"
#include "../cu_cp_impl_interface.h"
#include "../task_schedulers/du_task_scheduler.h"
#include "du_metrics_handler.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/ue_manager.h"
#include "srsran/support/async/async_task.h"
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

struct cu_cp_configuration;
class cu_cp_controller;

struct du_repository_config {
  const cu_cp_configuration&             cu_cp;
  cu_cp_du_event_handler&                cu_cp_du_handler;
  cu_cp_ue_removal_handler&              ue_removal_handler;
  cu_cp_ue_context_manipulation_handler& ue_context_handler;
  du_processor_e1ap_control_notifier&    e1ap_ctrl_notifier;
  du_processor_ngap_control_notifier&    ngap_ctrl_notifier;
  f1ap_ue_removal_notifier&              f1ap_cu_cp_notifier;
  rrc_ue_nas_notifier&                   ue_nas_pdu_notifier;
  rrc_ue_control_notifier&               ue_ngap_ctrl_notifier;
  du_processor_ue_task_scheduler&        ue_task_sched;
  du_processor_ue_manager&               ue_manager;
  rrc_du_measurement_config_notifier&    meas_config_notifier;
  du_connection_notifier&                du_conn_notif;
  srslog::basic_logger&                  logger;
};

class du_processor_repository : public cu_cp_f1c_handler,
                                public du_repository_ngap_handler,
                                public du_repository_metrics_handler
{
public:
  explicit du_processor_repository(du_repository_config cfg_);

  void stop();

  // DU interface
  std::unique_ptr<f1ap_message_notifier>
       handle_new_du_connection(std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier) override;
  void handle_du_remove_request(du_index_t du_index) override;

  du_handler& get_du(du_index_t du_index) override;
  du_index_t  find_du(pci_t pci) override;

  void handle_paging_message(cu_cp_paging_message& msg) override;

  ue_index_t handle_ue_index_allocation_request(const nr_cell_global_id_t& nci) override;
  async_task<ngap_handover_resource_allocation_response>
  handle_ngap_handover_request(const ngap_handover_request& request) override;

  void handle_inactivity_notification(du_index_t du_index, const cu_cp_inactivity_notification& msg);

  std::vector<metrics_report::du_info> handle_du_metrics_report_request() const override;

private:
  struct du_context final : public du_handler {
    // CU-CP handler of DU processor events.
    du_processor_cu_cp_adapter du_to_cu_cp_notifier;

    // NGAP to DU processor notifier;
    ngap_du_processor_adapter ngap_du_processor_notifier;

    std::unique_ptr<du_processor_impl_interface> du_processor;

    /// Notifier used by the CU-CP to push F1AP Tx messages to the respective DU.
    std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier;

    f1ap_message_handler&                  get_f1ap_message_handler() override;
    du_processor_mobility_handler&         get_mobility_handler() override;
    du_processor_ue_task_handler&          get_du_processor_ue_task_handler() override;
    du_processor_f1ap_ue_context_notifier& get_f1ap_ue_context_notifier() override;
    du_processor_ue_context_notifier&      get_du_processor_ue_context_notifier() override;
  };

  /// \brief Find a DU object.
  /// \param[in] du_index The index of the DU processor object.
  /// \return The DU processor object.
  du_processor_impl_interface& find_du(du_index_t du_index);

  /// \brief Adds a DU processor object to the CU-CP.
  /// \return The DU index of the added DU processor object.
  du_index_t add_du(std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier);

  /// \brief Removes the specified DU processor object from the CU-CP.
  ///
  /// Note: This function assumes that the caller is in the CU-CP execution context.
  /// \param[in] du_index The index of the DU processor to delete.
  void remove_du_impl(du_index_t du_index);

  /// \brief Get the next available index from the DU processor database.
  /// \return The DU index.
  du_index_t get_next_du_index();

  du_repository_config  cfg;
  srslog::basic_logger& logger;

  // F1AP to DU repository adapter.
  f1ap_du_repository_adapter f1ap_ev_notifier;

  std::map<du_index_t, du_context> du_db;

  // TODO: DU removal not yet fully supported. Instead we just move the DU context to a separate map.
  std::map<du_index_t, du_context> removed_du_db;

  std::atomic<bool> running{true};
};

} // namespace srs_cu_cp
} // namespace srsran
