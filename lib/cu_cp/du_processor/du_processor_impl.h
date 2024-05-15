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
#include "../adapters/rrc_ue_adapters.h"
#include "du_processor.h"
#include "du_processor_config.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/du_processor_context.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/support/executors/task_executor.h"
#include <string>
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

class du_processor_impl : public du_processor,
                          public du_setup_handler,
                          public du_metrics_handler,
                          public du_processor_f1ap_interface,
                          public du_processor_ngap_interface,
                          public du_processor_ue_task_handler,
                          public du_processor_paging_handler,
                          public du_processor_statistics_handler,
                          public du_processor_mobility_handler
{
public:
  du_processor_impl(const du_processor_config_t&        du_processor_config_,
                    du_processor_cu_cp_notifier&        cu_cp_notifier_,
                    f1ap_du_management_notifier&        f1ap_du_mgmt_notifier_,
                    f1ap_message_notifier&              f1ap_notifier_,
                    rrc_ue_nas_notifier&                rrc_ue_nas_pdu_notifier_,
                    rrc_ue_control_notifier&            rrc_ue_ngap_ctrl_notifier_,
                    rrc_du_measurement_config_notifier& rrc_du_cu_cp_notifier,
                    common_task_scheduler&              common_task_sched_,
                    du_processor_ue_task_scheduler&     task_sched_,
                    du_processor_ue_manager&            ue_manager_,
                    task_executor&                      ctrl_exec_);
  ~du_processor_impl() = default;

  // getter functions

  du_index_t                       get_du_index() override { return context.du_index; }
  f1ap_message_handler&            get_message_handler() override { return *f1ap; }
  f1ap_ue_context_manager&         get_f1ap_ue_context_manager() override { return *f1ap; }
  f1ap_ue_context_removal_handler& get_f1ap_ue_context_removal_handler() override { return *f1ap; }
  f1ap_statistics_handler&         get_f1ap_statistics_handler() override { return *f1ap; }

  size_t get_nof_ues() const override { return ue_manager.get_nof_du_ues(context.du_index); };

  // du_processor_f1ap_interface
  du_setup_result handle_du_setup_request(const du_setup_request& req) override;
  ue_index_t      allocate_new_ue_index() override;
  ue_rrc_context_creation_response
       handle_ue_rrc_context_creation_request(const ue_rrc_context_creation_request& req) override;
  void handle_du_initiated_ue_context_release_request(const f1ap_ue_context_release_request& request) override;

  // du_processor_mobility_manager_interface
  optional<nr_cell_global_id_t> get_cgi(pci_t pci) override;
  byte_buffer                   get_packed_sib1(nr_cell_global_id_t cgi) override;

  // du_processor paging handler
  void handle_paging_message(cu_cp_paging_message& msg) override;

  // du_processor_cell_info_interface
  bool has_cell(pci_t pci) override;
  bool has_cell(nr_cell_global_id_t cgi) override;

  void handle_ue_async_task(ue_index_t ue_index, async_task<void>&& task) override
  {
    task_sched.schedule_async_task(ue_index, std::move(task));
  }

  metrics_report::du_info handle_du_metrics_report_request() const override;

  unique_timer   make_unique_timer() override { return task_sched.make_unique_timer(); }
  timer_manager& get_timer_manager() override { return task_sched.get_timer_manager(); }

  du_processor_f1ap_interface&           get_f1ap_interface() override { return *this; }
  du_processor_ngap_interface&           get_ngap_interface() override { return *this; }
  du_processor_ue_task_handler&          get_ue_task_handler() override { return *this; }
  du_processor_paging_handler&           get_paging_handler() override { return *this; }
  du_processor_statistics_handler&       get_statistics_handler() override { return *this; }
  du_processor_mobility_handler&         get_mobility_handler() override { return *this; }
  du_processor_f1ap_ue_context_notifier& get_f1ap_ue_context_notifier() override { return f1ap_ue_context_notifier; }
  du_metrics_handler&                    get_metrics_handler() override { return *this; }

private:
  /// \brief Create RRC UE object for given UE.
  /// \return True on success, falso otherwise.
  bool create_rrc_ue(du_ue&                            ue,
                     rnti_t                            c_rnti,
                     const nr_cell_global_id_t&        cgi,
                     byte_buffer                       du_to_cu_rrc_container,
                     optional<rrc_ue_transfer_context> rrc_context);

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

  // NGAP senders
  /// \brief Request UE context release over NGAP.
  /// \param[in] ue_index The UE.
  /// \param[in] cause The cause of the failure.
  void send_ngap_ue_context_release_request(ue_index_t ue_index, ngap_cause_t cause);

  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-CP");
  du_processor_config_t cfg;

  du_processor_cu_cp_notifier&         cu_cp_notifier;
  f1ap_du_management_notifier&         f1ap_du_mgmt_notifier;
  f1ap_message_notifier&               f1ap_pdu_notifier;
  rrc_ue_nas_notifier&                 rrc_ue_nas_pdu_notifier;
  rrc_ue_control_notifier&             rrc_ue_ngap_ctrl_notifier;
  du_processor_ue_task_scheduler&      task_sched;
  du_processor_ue_manager&             ue_manager;
  du_processor_f1ap_ue_context_adapter f1ap_ue_context_notifier;
  du_processor_f1ap_paging_adapter     f1ap_paging_notifier;

  // F1AP to DU processor adapter
  f1ap_du_processor_adapter f1ap_ev_notifier;

  du_processor_context                       context;
  std::map<du_cell_index_t, du_cell_context> cell_db; /// flattened version of served cells list provided by DU/F1AP

  std::map<uint32_t, std::vector<nr_cell_global_id_t>> tac_to_nr_cgi;

  // timers associated with a given DU.
  timer_manager timer_db;

  // Components
  std::unique_ptr<f1ap_cu>          f1ap;
  std::unique_ptr<rrc_du_interface> rrc;

  // F1AP to RRC UE adapters
  std::unordered_map<ue_index_t, f1ap_rrc_ue_adapter> f1ap_rrc_ue_adapters;

  // RRC UE to F1AP adapters
  std::unordered_map<ue_index_t, rrc_ue_f1ap_pdu_adapter> rrc_ue_f1ap_adapters;

  // DU processor to RRC DU adapter
  du_processor_rrc_du_adapter rrc_du_adapter;

  // DU processor to RRC UE adapters
  std::unordered_map<ue_index_t, du_processor_rrc_ue_adapter> rrc_ue_adapters;
};

} // namespace srs_cu_cp
} // namespace srsran
