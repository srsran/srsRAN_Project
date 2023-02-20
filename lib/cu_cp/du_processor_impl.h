/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "adapters/du_processor_adapters.h"
#include "adapters/f1ap_adapters.h"
#include "adapters/rrc_ue_adapters.h"
#include "routine_managers/du_processor_routine_manager.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/asn1/rrc_nr/rrc_nr.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/du_processor_config.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/pdcp/pdcp_entity.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/rrc/rrc_du_factory.h"
#include "srsran/support/executors/task_executor.h"
#include "ue_manager_impl.h"
#include <string>

namespace srsran {
namespace srs_cu_cp {

class du_processor_impl : public du_processor_interface
{
public:
  du_processor_impl(const du_processor_config_t         du_processor_config_,
                    du_processor_cu_cp_notifier&        cu_cp_notifier_,
                    f1ap_du_management_notifier&        f1ap_du_mgmt_notifier_,
                    f1ap_message_notifier&              f1ap_notifier_,
                    du_processor_e1ap_control_notifier& e1ap_ctrl_notifier_,
                    rrc_ue_nas_notifier&                rrc_ue_nas_pdu_notifier_,
                    rrc_ue_control_notifier&            rrc_ue_ngap_ctrl_notifier_,
                    du_processor_ue_task_scheduler&     task_sched_,
                    du_processor_ue_manager&            ue_manager_,
                    task_executor&                      ctrl_exec_);
  ~du_processor_impl() = default;

  // message handlers
  void handle_f1_setup_request(const f1_setup_request_message& msg) override;

  // getter functions

  du_index_t               get_du_index() override { return context.du_index; }
  du_processor_context&    get_context() override { return context; }
  f1ap_message_handler&    get_f1ap_message_handler() override { return *f1ap; }
  f1ap_ue_context_manager& get_f1ap_ue_context_manager() override { return *f1ap; }
  f1ap_statistics_handler& get_f1ap_statistics_handler() override { return *f1ap; }

  size_t get_nof_ues() override { return ue_manager.get_nof_du_ues(); };

  // du_processor_rrc_message_handler
  ue_creation_complete_message handle_ue_creation_request(const ue_creation_message& msg) override;

  rrc_amf_connection_handler&
  get_rrc_amf_connection_handler() override; /// Pass handle to AMF connection handler within RRC

  // du_processor_rrc_ue_interface
  /// \brief Create SRB entry in bearer list and add adapter handle.
  void create_srb(const srb_creation_message& msg) override;
  void handle_ue_context_release_command(const cu_cp_ue_context_release_command& cmd) override;

  // du_processor_ngap_interface
  async_task<cu_cp_pdu_session_resource_setup_response>
       handle_new_pdu_session_resource_setup_request(const cu_cp_pdu_session_resource_setup_request& msg) override;
  void handle_new_ue_context_release_command(const cu_cp_ue_context_release_command& cmd) override;

  void handle_ue_async_task(ue_index_t ue_index, async_task<void>&& task) override
  {
    task_sched.schedule_async_task(ue_index, std::move(task));
  }

  unique_timer   make_unique_timer() override { return task_sched.make_unique_timer(); }
  timer_manager& get_timer_manager() override { return task_sched.get_timer_manager(); }

private:
  /// \brief Lookup the cell based on a given NR cell ID.
  /// \param[in] packed_nr_cell_id The packed NR cell ID received over F1AP.
  du_cell_index_t find_cell(uint64_t packed_nr_cell_id);

  /// \brief Get the next available DU cell index.
  /// \return The next DU cell index.
  du_cell_index_t get_next_du_cell_index();

  /// \brief Get the DU processor cell database.
  /// \return The DU processor cell database.
  slotted_id_vector<du_cell_index_t, du_cell_context>& get_cell_db() { return cell_db; };

  // F1AP senders

  /// \brief Create and transmit the F1 Setup response message.
  /// \param[in] du_ctxt The context of the DU that should receive the message.
  void send_f1_setup_response(const du_processor_context& du_ctxt);

  /// \brief Create and transmit the F1 Setup failure message.
  /// \param[in] cause The cause of the failure.
  void send_f1_setup_failure(asn1::f1ap::cause_c::types::options cause);

  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-CP");
  du_processor_config_t cfg;

  du_processor_cu_cp_notifier&        cu_cp_notifier;
  f1ap_du_management_notifier&        f1ap_du_mgmt_notifier;
  f1ap_message_notifier&              f1ap_notifier;
  du_processor_e1ap_control_notifier& e1ap_ctrl_notifier;
  rrc_ue_nas_notifier&                rrc_ue_nas_pdu_notifier;
  rrc_ue_control_notifier&            rrc_ue_ngap_ctrl_notifier;
  du_processor_ue_task_scheduler&     task_sched;
  du_processor_ue_manager&            ue_manager;
  du_processor_f1ap_adapter           f1ap_ue_context_notifier;

  du_processor_context context;
  slotted_id_vector<du_cell_index_t, du_cell_context>
                        cell_db; /// flattened version of served cells list provided by DU/F1AP
  std::atomic<uint16_t> next_du_cell_index{0};

  // timers associated with a given DU.
  timer_manager timer_db;

  // Components
  std::unique_ptr<f1ap_cu>                                     f1ap;
  std::unique_ptr<rrc_du_interface>                            rrc;
  std::unordered_map<ue_index_t, std::unique_ptr<pdcp_entity>> pdcp_bearers;

  // F1AP to DU processor adapter
  f1ap_du_processor_adapter f1ap_ev_notifier;

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
