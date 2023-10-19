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

#include "../adapters/du_processor_adapters.h"
#include "../adapters/f1ap_adapters.h"
#include "../adapters/rrc_ue_adapters.h"
#include "../routine_managers/du_processor_routine_manager.h"
#include "srsran/cu_cp/cell_meas_manager.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/du_processor_config.h"
#include "srsran/cu_cp/du_processor_context.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/support/executors/task_executor.h"
#include <string>

namespace srsran {
namespace srs_cu_cp {

class du_processor_impl : public du_processor_interface
{
public:
  du_processor_impl(const du_processor_config_t&        du_processor_config_,
                    du_processor_cu_cp_notifier&        cu_cp_notifier_,
                    f1ap_du_management_notifier&        f1ap_du_mgmt_notifier_,
                    f1ap_message_notifier&              f1ap_notifier_,
                    du_processor_e1ap_control_notifier& e1ap_ctrl_notifier_,
                    du_processor_ngap_control_notifier& ngap_ctrl_notifier_,
                    f1ap_ue_removal_notifier&           f1ap_cu_cp_notifier_,
                    rrc_ue_nas_notifier&                rrc_ue_nas_pdu_notifier_,
                    rrc_ue_control_notifier&            rrc_ue_ngap_ctrl_notifier_,
                    rrc_ue_reestablishment_notifier&    rrc_ue_cu_cp_notifier_,
                    du_processor_ue_task_scheduler&     task_sched_,
                    du_processor_ue_manager&            ue_manager_,
                    cell_meas_manager&                  cell_meas_mng_,
                    task_executor&                      ctrl_exec_);
  ~du_processor_impl() = default;

  // getter functions

  du_index_t                  get_du_index() override { return context.du_index; }
  f1ap_message_handler&       get_f1ap_message_handler() override { return *f1ap; }
  f1ap_ue_context_manager&    get_f1ap_ue_context_manager() override { return *f1ap; }
  f1ap_statistics_handler&    get_f1ap_statistics_handler() override { return *f1ap; }
  rrc_amf_connection_handler& get_rrc_amf_connection_handler() override { return *rrc; };

  size_t get_nof_ues() const override { return ue_manager.get_nof_du_ues(context.du_index); };

  // du_processor_f1ap_interface
  void                         handle_f1_setup_request(const f1ap_f1_setup_request& request) override;
  ue_index_t                   get_new_ue_index() override;
  ue_creation_complete_message handle_ue_creation_request(const cu_cp_ue_creation_message& msg) override;
  void handle_du_initiated_ue_context_release_request(const f1ap_ue_context_release_request& request) override;
  ue_update_complete_message handle_ue_update_request(const ue_update_message& msg) override;

  // du_processor_rrc_ue_interface
  async_task<cu_cp_ue_context_release_complete>
                   handle_ue_context_release_command(const cu_cp_ue_context_release_command& cmd) override;
  async_task<bool> handle_rrc_reestablishment_context_modification_required(ue_index_t ue_index) override;

  // du_processor_ngap_interface
  async_task<cu_cp_pdu_session_resource_setup_response>
  handle_new_pdu_session_resource_setup_request(const cu_cp_pdu_session_resource_setup_request& msg) override;
  async_task<cu_cp_pdu_session_resource_modify_response>
  handle_new_pdu_session_resource_modify_request(const cu_cp_pdu_session_resource_modify_request& msg) override;
  async_task<cu_cp_pdu_session_resource_release_response>
  handle_new_pdu_session_resource_release_command(const cu_cp_pdu_session_resource_release_command& msg) override;
  async_task<cu_cp_ue_context_release_complete>
  handle_ue_context_release_command(const cu_cp_ngap_ue_context_release_command& cmd) override;

  // du_processor_mobility_manager_interface
  optional<nr_cell_global_id_t> get_cgi(pci_t pci) override;
  async_task<cu_cp_inter_du_handover_response>
  handle_inter_du_handover_request(const cu_cp_inter_du_handover_request& request,
                                   du_processor_f1ap_ue_context_notifier& target_du_f1ap_ue_ctxt_notif_) override;
  async_task<cu_cp_inter_ngran_node_n2_handover_response>
  handle_inter_ngran_node_n2_handover_request(const cu_cp_inter_ngran_node_n2_handover_request& request) override;
  async_task<ngap_handover_resource_allocation_response>
  handle_ngap_handover_request(const ngap_handover_request& request) override;

  // du_processor paging handler
  void handle_paging_message(cu_cp_paging_message& msg) override;

  // du_processor inactivity handler
  void handle_inactivity_notification(const cu_cp_inactivity_notification& msg) override;

  // du_processor_cell_info_interface
  bool has_cell(pci_t pci) override;
  bool has_cell(nr_cell_global_id_t cgi) override;

  void handle_ue_async_task(ue_index_t ue_index, async_task<void>&& task) override
  {
    task_sched.schedule_async_task(ue_index, std::move(task));
  }

  unique_timer   make_unique_timer() override { return task_sched.make_unique_timer(); }
  timer_manager& get_timer_manager() override { return task_sched.get_timer_manager(); }

  du_processor_f1ap_interface&           get_du_processor_f1ap_interface() override { return *this; }
  du_processor_rrc_interface&            get_du_processor_rrc_interface() override { return *this; }
  du_processor_rrc_ue_interface&         get_du_processor_rrc_ue_interface() override { return *this; }
  du_processor_ngap_interface&           get_du_processor_ngap_interface() override { return *this; }
  du_processor_ue_task_handler&          get_du_processor_ue_task_handler() override { return *this; }
  du_processor_paging_handler&           get_du_processor_paging_handler() override { return *this; }
  du_processor_inactivity_handler&       get_du_processor_inactivity_handler() override { return *this; }
  du_processor_statistics_handler&       get_du_processor_statistics_handler() override { return *this; }
  du_processor_mobility_handler&         get_du_processor_mobility_handler() override { return *this; }
  du_processor_f1ap_ue_context_notifier& get_du_processor_f1ap_ue_context_notifier() override
  {
    return f1ap_ue_context_notifier;
  }

private:
  /// \brief Create RRC UE object for given UE.
  /// \return True on success, falso otherwise.
  bool create_rrc_ue(du_ue&                     ue,
                     rnti_t                     c_rnti,
                     const nr_cell_global_id_t& cgi,
                     byte_buffer                du_to_cu_rrc_container,
                     bool                       is_inter_cu_handover = false);

  /// \brief Lookup the cell based on a given NR cell ID.
  /// \param[in] packed_nr_cell_id The packed NR cell ID received over F1AP.
  du_cell_index_t find_cell(uint64_t packed_nr_cell_id);

  /// \brief Get the next available DU cell index.
  /// \return The next DU cell index.
  du_cell_index_t get_next_du_cell_index();

  /// \brief Get the DU processor cell database.
  /// \return The DU processor cell database.
  std::map<du_cell_index_t, du_cell_context>& get_cell_db() { return cell_db; };

  // F1AP senders

  /// \brief Create and transmit the F1 Setup response message.
  /// \param[in] du_ctxt The context of the DU that should receive the message.
  void send_f1_setup_response(const du_processor_context& du_ctxt);

  /// \brief Create and transmit the F1 Setup failure message.
  /// \param[in] cause The cause of the failure.
  void send_f1_setup_failure(cause_t cause);

  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-CP");
  du_processor_config_t cfg;

  du_processor_cu_cp_notifier&         cu_cp_notifier;
  f1ap_du_management_notifier&         f1ap_du_mgmt_notifier;
  f1ap_message_notifier&               f1ap_notifier;
  du_processor_e1ap_control_notifier&  e1ap_ctrl_notifier;
  du_processor_ngap_control_notifier&  ngap_ctrl_notifier;
  f1ap_ue_removal_notifier&            f1ap_cu_cp_notifier;
  rrc_ue_nas_notifier&                 rrc_ue_nas_pdu_notifier;
  rrc_ue_control_notifier&             rrc_ue_ngap_ctrl_notifier;
  rrc_ue_reestablishment_notifier&     rrc_ue_cu_cp_notifier;
  du_processor_ue_task_scheduler&      task_sched;
  du_processor_ue_manager&             ue_manager;
  du_processor_f1ap_ue_context_adapter f1ap_ue_context_notifier;
  du_processor_f1ap_paging_adapter     f1ap_paging_notifier;
  task_executor&                       ctrl_exec;

  du_processor_context                       context;
  std::map<du_cell_index_t, du_cell_context> cell_db; /// flattened version of served cells list provided by DU/F1AP
  std::atomic<uint16_t>                      next_du_cell_index{0};

  std::map<uint32_t, nr_cell_global_id_t> tac_to_nr_cgi;

  // timers associated with a given DU.
  timer_manager timer_db;

  // Components
  std::unique_ptr<f1ap_cu>          f1ap;
  std::unique_ptr<rrc_du_interface> rrc;

  // F1AP to DU processor adapter
  f1ap_du_processor_adapter f1ap_ev_notifier;

  // F1AP to RRC UE adapters
  std::unordered_map<ue_index_t, f1ap_rrc_ue_adapter> f1ap_rrc_ue_adapters;

  // RRC UE to F1AP adapters
  std::unordered_map<ue_index_t, rrc_ue_f1ap_pdu_adapter> rrc_ue_f1ap_adapters;

  // RRC UE to DU processor adapter
  rrc_ue_du_processor_adapter rrc_ue_ev_notifier;

  // DU processor to RRC DU adapter
  du_processor_rrc_du_adapter rrc_du_adapter;

  // RRC UE to DU processor task schedulers
  std::unordered_map<ue_index_t, rrc_to_du_ue_task_scheduler> rrc_ue_task_scheds;

  // DU processor to RRC UE adapters
  std::unordered_map<ue_index_t, du_processor_rrc_ue_adapter> rrc_ue_adapters;

  // DU processor routine manager
  std::unique_ptr<du_processor_routine_manager> routine_mng;
};

} // namespace srs_cu_cp
} // namespace srsran
