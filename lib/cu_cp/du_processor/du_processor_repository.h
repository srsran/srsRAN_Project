/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../adapters/du_processor_adapters.h"
#include "../adapters/f1ap_adapters.h"
#include "../adapters/ngap_adapters.h"
#include "../cu_cp_controller/common_task_scheduler.h"
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

struct du_repository_config {
  const cu_cp_configuration&             cu_cp;
  cu_cp_du_event_handler&                cu_cp_du_handler;
  cu_cp_ue_removal_handler&              ue_removal_handler;
  cu_cp_ue_context_manipulation_handler& ue_context_handler;
  rrc_ue_nas_notifier&                   ue_nas_pdu_notifier;
  rrc_ue_control_notifier&               ue_ngap_ctrl_notifier;
  common_task_scheduler&                 common_task_sched;
  du_processor_ue_task_scheduler&        ue_task_sched;
  du_processor_ue_manager&               ue_manager;
  rrc_du_measurement_config_notifier&    meas_config_notifier;
  du_connection_notifier&                du_conn_notif;
  srslog::basic_logger&                  logger;
};

class du_processor_repository : public du_repository_ngap_handler, public du_repository_metrics_handler
{
public:
  explicit du_processor_repository(du_repository_config cfg_);

  void stop();

  /// \brief Checks whether a cell with the specified PCI is served by any of the connected DUs.
  /// \param[out] The index of the DU serving the given PCI.
  du_index_t find_du(pci_t pci);

  du_processor& get_du_processor(du_index_t du_index);

  void handle_paging_message(cu_cp_paging_message& msg) override;

  ue_index_t handle_ue_index_allocation_request(const nr_cell_global_id_t& nci) override;

  std::vector<metrics_report::du_info> handle_du_metrics_report_request() const override;

  size_t get_nof_f1ap_ues();

  /// \brief Adds a DU processor object to the CU-CP.
  /// \return The DU index of the added DU processor object.
  du_index_t add_du(std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier);

  void remove_du(du_index_t du_idx);

private:
  struct du_context {
    // CU-CP handler of DU processor events.
    du_processor_cu_cp_adapter du_to_cu_cp_notifier;

    std::unique_ptr<du_processor> processor;

    /// Notifier used by the CU-CP to push F1AP Tx messages to the respective DU.
    std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier;
  };

  /// \brief Find a DU object.
  /// \param[in] du_index The index of the DU processor object.
  /// \return The DU processor object.
  du_processor& find_du(du_index_t du_index);

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

  std::map<du_index_t, du_context> du_db;

  // TODO: DU removal not yet fully supported. Instead we just move the DU context to a separate map.
  std::map<du_index_t, du_context> removed_du_db;

  std::atomic<bool> running{true};
};

} // namespace srs_cu_cp
} // namespace srsran
