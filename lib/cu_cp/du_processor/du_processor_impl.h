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
#include "../adapters/rrc_ue_adapters.h"
#include "../ue_manager/ue_manager_impl.h"
#include "du_configuration_handler.h"
#include "du_processor.h"
#include "du_processor_config.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/du_processor_context.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/support/executors/task_executor.h"
#include <string>

namespace srsran {
namespace srs_cu_cp {

class du_processor_impl : public du_processor,
                          public du_setup_handler,
                          public du_metrics_handler,
                          public du_processor_f1ap_interface,
                          public du_processor_paging_handler,
                          public du_processor_statistics_handler,
                          public du_processor_mobility_handler
{
public:
  du_processor_impl(du_processor_config_t               du_processor_config_,
                    du_processor_cu_cp_notifier&        cu_cp_notifier_,
                    f1ap_message_notifier&              f1ap_pdu_notifier_,
                    rrc_ue_nas_notifier&                rrc_ue_nas_pdu_notifier_,
                    rrc_ue_control_notifier&            rrc_ue_ngap_ctrl_notifier_,
                    rrc_du_measurement_config_notifier& rrc_du_cu_cp_notifier,
                    common_task_scheduler&              common_task_sched_,
                    ue_manager&                         ue_mng_,
                    timer_manager&                      timers_,
                    task_executor&                      ctrl_exec_);
  ~du_processor_impl() override = default;

  // getter functions

  du_index_t                       get_du_index() override { return cfg.du_index; }
  f1ap_cu&                         get_f1ap_handler() override { return *f1ap; };
  f1ap_ue_context_manager&         get_f1ap_ue_context_manager() override { return *f1ap; }
  f1ap_ue_context_removal_handler& get_f1ap_ue_context_removal_handler() override { return *f1ap; }
  f1ap_statistics_handler&         get_f1ap_statistics_handler() override { return *f1ap; }

  size_t get_nof_ues() const override { return ue_mng.get_nof_du_ues(cfg.du_index); };

  // du_processor_f1ap_interface
  du_setup_result handle_du_setup_request(const du_setup_request& req) override;
  ue_rrc_context_creation_outcome
       handle_ue_rrc_context_creation_request(const ue_rrc_context_creation_request& req) override;
  void handle_du_initiated_ue_context_release_request(const f1ap_ue_context_release_request& request) override;
  async_task<void> handle_ue_transaction_info_loss(const f1_ue_transaction_info_loss_event& request) override;

  // du_processor_mobility_manager_interface
  std::optional<nr_cell_global_id_t> get_cgi(pci_t pci) override;
  byte_buffer                        get_packed_sib1(nr_cell_global_id_t cgi) override;

  // du_processor paging handler
  void handle_paging_message(cu_cp_paging_message& msg) override;

  // du_processor_cell_info_interface
  bool has_cell(pci_t pci) override;
  bool has_cell(nr_cell_global_id_t cgi) override;

  metrics_report::du_info handle_du_metrics_report_request() const override;

  du_processor_f1ap_interface&           get_f1ap_interface() override { return *this; }
  du_processor_paging_handler&           get_paging_handler() override { return *this; }
  du_processor_statistics_handler&       get_statistics_handler() override { return *this; }
  du_processor_mobility_handler&         get_mobility_handler() override { return *this; }
  du_processor_f1ap_ue_context_notifier& get_f1ap_ue_context_notifier() override { return f1ap_ue_context_notifier; }
  du_metrics_handler&                    get_metrics_handler() override { return *this; }

private:
  /// \brief Create RRC UE object for given UE.
  /// \return True on success, falso otherwise.
  bool create_rrc_ue(cu_cp_ue&                              ue,
                     rnti_t                                 c_rnti,
                     const nr_cell_global_id_t&             cgi,
                     byte_buffer                            du_to_cu_rrc_container,
                     std::optional<rrc_ue_transfer_context> rrc_context);

  // NGAP senders
  /// \brief Request UE context release over NGAP.
  /// \param[in] ue_index The UE.
  /// \param[in] cause The cause of the failure.
  void send_ngap_ue_context_release_request(ue_index_t ue_index, ngap_cause_t cause);

  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-CP");
  du_processor_config_t cfg;

  du_processor_cu_cp_notifier&         cu_cp_notifier;
  f1ap_message_notifier&               f1ap_pdu_notifier;
  rrc_ue_nas_notifier&                 rrc_ue_nas_pdu_notifier;
  rrc_ue_control_notifier&             rrc_ue_ngap_ctrl_notifier;
  ue_manager&                          ue_mng;
  du_processor_f1ap_ue_context_adapter f1ap_ue_context_notifier;
  du_processor_f1ap_paging_adapter     f1ap_paging_notifier;

  // F1AP to DU processor adapter
  f1ap_du_processor_adapter f1ap_ev_notifier;

  std::map<uint32_t, std::vector<nr_cell_global_id_t>> tac_to_nr_cgi;

  // F1AP to RRC UE adapters
  std::unordered_map<ue_index_t, f1ap_rrc_ue_adapter> f1ap_rrc_ue_adapters;

  // RRC UE to F1AP adapters
  std::unordered_map<ue_index_t, rrc_ue_f1ap_pdu_adapter> rrc_ue_f1ap_adapters;

  // DU processor to RRC DU adapter
  du_processor_rrc_du_adapter rrc_du_adapter;

  // DU processor to RRC UE adapters
  std::unordered_map<ue_index_t, du_processor_rrc_ue_adapter> rrc_ue_adapters;

  // Components
  std::unique_ptr<f1ap_cu>          f1ap;
  std::unique_ptr<rrc_du_interface> rrc;
};

} // namespace srs_cu_cp
} // namespace srsran
