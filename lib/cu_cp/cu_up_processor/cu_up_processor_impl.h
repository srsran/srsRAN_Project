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

#include "cu_up_processor_config.h"
#include "cu_up_processor_impl_interface.h"
#include "srsran/cu_cp/common_task_scheduler.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include <string>

namespace srsran {
namespace srs_cu_cp {

class cu_up_processor_impl : public cu_up_processor_impl_interface
{
public:
  cu_up_processor_impl(const cu_up_processor_config_t cu_up_processor_config_,
                       e1ap_message_notifier&         e1ap_notifier_,
                       e1ap_cu_cp_notifier&           cu_cp_notifier_,
                       common_task_scheduler&         common_task_sched_);

  void stop(ue_index_t ue_index) override;

  // message handlers
  void handle_cu_up_e1_setup_request(const cu_up_e1_setup_request& msg) override;

  // getter functions
  cu_up_index_t                        get_cu_up_index() override { return context.cu_up_index; }
  cu_up_processor_context&             get_context() override { return context; }
  e1ap_message_handler&                get_e1ap_message_handler() override { return *e1ap; }
  e1ap_bearer_context_manager&         get_e1ap_bearer_context_manager() override { return *e1ap; }
  e1ap_bearer_context_removal_handler& get_e1ap_bearer_context_removal_handler() override { return *e1ap; }
  e1ap_statistics_handler&             get_e1ap_statistics_handler() override { return *e1ap; }

  void update_ue_index(ue_index_t ue_index, ue_index_t old_ue_index) override;

private:
  class e1ap_cu_up_processor_adapter;

  // E1AP senders

  /// \brief Create and transmit the GNB-CU-UP E1 Setup response message.
  /// \param[in] du_ctxt The context of the DU that should receive the message.
  void send_cu_up_e1_setup_response();

  /// \brief Create and transmit the GNB-CU-UP E1 Setup failure message.
  /// \param[in] cause The cause of the failure.
  void send_cu_up_e1_setup_failure(e1ap_cause_t cause);

  srslog::basic_logger&    logger = srslog::fetch_basic_logger("CU-CP");
  cu_up_processor_config_t cfg;

  e1ap_message_notifier& e1ap_notifier;
  e1ap_cu_cp_notifier&   cu_cp_notifier;

  cu_up_processor_context context;

  // E1AP to CU-UP processor adapter
  std::unique_ptr<e1ap_cu_up_processor_notifier> e1ap_ev_notifier;

  // Components
  std::unique_ptr<e1ap_interface> e1ap;
};

} // namespace srs_cu_cp
} // namespace srsran
