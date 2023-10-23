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

#include "adapters/cell_meas_manager_adapters.h"
#include "adapters/cu_cp_adapters.h"
#include "adapters/du_processor_adapters.h"
#include "adapters/e1ap_adapters.h"
#include "adapters/f1ap_adapters.h"
#include "adapters/ngap_adapters.h"
#include "adapters/rrc_ue_adapters.h"
#include "cu_cp_impl_interface.h"
#include "cu_up_processor/cu_up_processor_repository.h"
#include "du_processor/du_processor_repository.h"
#include "routine_managers/cu_cp_routine_manager.h"
#include "task_schedulers/cu_up_task_scheduler.h"
#include "task_schedulers/du_task_scheduler.h"
#include "task_schedulers/ue_task_scheduler.h"
#include "ue_manager_impl.h"
#include "srsran/cu_cp/cell_meas_manager.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include <memory>
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

class cu_cp_impl final : public cu_cp_interface, public cu_cp_impl_interface
{
public:
  explicit cu_cp_impl(const cu_cp_configuration& config_);
  ~cu_cp_impl() override;

  void start() override;
  void stop() override;

  // CU-CP CU-UP interface
  size_t                get_nof_cu_ups() const override;
  e1ap_message_handler& get_e1ap_message_handler(cu_up_index_t cu_up_index) override;

  // NGAP interface
  ngap_message_handler& get_ngap_message_handler() override;
  ngap_event_handler&   get_ngap_event_handler() override;

  bool amf_is_connected() override { return amf_connected; };

  // CU-UP handler
  void handle_e1ap_created(e1ap_bearer_context_manager&         bearer_context_manager,
                           e1ap_bearer_context_removal_handler& bearer_removal_handler,
                           e1ap_statistics_handler&             e1ap_statistic_handler) override;
  void handle_bearer_context_inactivity_notification(const cu_cp_inactivity_notification& msg) override;

  // NGAP connection handler
  void handle_amf_connection() override;
  void handle_amf_connection_drop() override;

  // RRC UE handler
  rrc_reestablishment_ue_context_t
                   handle_rrc_reestablishment_request(pci_t old_pci, rnti_t old_c_rnti, ue_index_t ue_index) override;
  async_task<bool> handle_ue_context_transfer(ue_index_t ue_index, ue_index_t old_ue_index) override;

  // cu_cp_ue_removal_interface
  void handle_ue_removal_request(ue_index_t ue_index) override;

  // cu_cp interface
  du_repository&                    get_connected_dus() override { return du_db; }
  cu_up_repository&                 get_connected_cu_ups() override { return cu_up_db; }
  cu_cp_cu_up_connection_interface& get_cu_cp_cu_up_connection_interface() override { return *this; }
  cu_cp_e1ap_handler&               get_cu_cp_e1ap_handler() override { return *this; }
  cu_cp_ngap_connection_interface&  get_cu_cp_ngap_connection_interface() override { return *this; }
  cu_cp_ngap_handler&               get_cu_cp_ngap_handler() override { return *this; }
  cu_cp_rrc_ue_interface&           get_cu_cp_rrc_ue_interface() override { return *this; }
  cu_cp_ue_removal_handler&         get_cu_cp_ue_removal_handler() override { return *this; }

private:
  // Handling of DU events.
  void handle_du_processor_creation(du_index_t                       du_index,
                                    f1ap_ue_context_removal_handler& f1ap_handler,
                                    f1ap_statistics_handler&         f1ap_statistic_handler,
                                    rrc_ue_removal_handler&          rrc_handler,
                                    rrc_du_statistics_handler&       rrc_statistic_handler) override;

  void handle_rrc_ue_creation(ue_index_t                          ue_index,
                              rrc_ue_interface&                   rrc_ue,
                              ngap_du_processor_control_notifier& ngap_to_du_notifier) override;

  void on_statistics_report_timer_expired();

  cu_cp_configuration cfg;

  // logger
  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-CP");

  // Components
  std::unique_ptr<ngap_interface> ngap_entity;

  ue_manager ue_mng;

  std::unique_ptr<mobility_manager> mobility_mng;

  std::unique_ptr<cell_meas_manager> cell_meas_mng; // cell measurement manager

  // CU-CP to NGAP adapter
  cu_cp_ngap_adapter ngap_adapter;

  // CU-CP to E1AP adapters
  std::map<cu_up_index_t, cu_cp_e1ap_adapter> e1ap_adapters;

  // CU-CP to F1AP adapters
  std::map<du_index_t, cu_cp_f1ap_adapter> f1ap_adapters;

  // CU-CP to RRC DU adapters
  std::map<du_index_t, cu_cp_rrc_du_adapter> rrc_du_adapters;

  // UE Task scheduler used by DU processors.
  du_processor_to_cu_cp_task_scheduler du_processor_task_sched;

  // DU Processor to E1AP adapter
  du_processor_e1ap_adapter du_processor_e1ap_notifier;

  // DU Processor to NGAP adapter
  du_processor_ngap_adapter du_processor_ngap_notifier;

  // Cell Measurement Manager to mobility manager adapters
  cell_meas_mobility_manager_adapter cell_meas_ev_notifier;

  // E1AP to CU-CP adapter
  e1ap_cu_cp_adapter e1ap_ev_notifier;

  // NGAP to CU-CP adapters
  ngap_to_cu_cp_task_scheduler ngap_task_sched;
  ngap_cu_cp_adapter           ngap_cu_cp_ev_notifier;

  // F1AP to CU-CP adapter
  f1ap_cu_cp_adapter f1ap_cu_cp_notifier;

  // RRC UE to CU-CP adapter
  rrc_ue_cu_cp_adapter rrc_ue_cu_cp_notifier;

  // RRC UE to NGAP adapter
  rrc_ue_ngap_adapter rrc_ue_ngap_notifier;

  // NGAP to RRC UE adapter array
  slotted_array<ngap_rrc_ue_adapter, MAX_NOF_CU_UES> ngap_rrc_ue_ev_notifiers;

  // CU-CP to RRC UE adapter array
  std::map<ue_index_t, cu_cp_rrc_ue_adapter> cu_cp_rrc_ue_ev_notifiers; // indexed by UE index

  // DU connections being managed by the CU-CP.
  du_processor_repository du_db;

  // CU-UP connections being managed by the CU-CP.
  cu_up_processor_repository cu_up_db;

  // UE task scheduler
  ue_task_scheduler ue_task_sched;

  std::unique_ptr<cu_cp_routine_manager> routine_mng;

  std::atomic<bool> amf_connected = {false};

  unique_timer statistics_report_timer;

  std::atomic<bool> stopped{false};
};

} // namespace srs_cu_cp
} // namespace srsran
