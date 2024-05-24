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

#include "../adapters/e1ap_adapters.h"
#include "../adapters/ngap_adapters.h"
#include "../task_schedulers/cu_up_task_scheduler.h"
#include "cu_up_processor_config.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/ue_manager.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"
#include "srsran/support/executors/task_executor.h"
#include <string>

namespace srsran {
namespace srs_cu_cp {

class cu_up_processor_impl : public cu_up_processor_impl_interface
{
public:
  cu_up_processor_impl(const cu_up_processor_config_t cu_up_processor_config_,
                       e1ap_message_notifier&         e1ap_notifier_,
                       e1ap_cu_cp_notifier&           cu_cp_notifier_,
                       common_ue_manager&             ue_mng_,
                       cu_up_task_scheduler&          task_sched_,
                       task_executor&                 ctrl_exec_);

  void stop(ue_index_t ue_index) override;

  // message handlers
  void handle_cu_up_e1_setup_request(const cu_up_e1_setup_request& msg) override;

  // getter functions
  cu_up_index_t                        get_cu_up_index() override { return context.cu_up_index; };
  cu_up_processor_context&             get_context() override { return context; };
  e1ap_message_handler&                get_e1ap_message_handler() override { return *e1ap; };
  e1ap_bearer_context_manager&         get_e1ap_bearer_context_manager() override { return *e1ap; }
  e1ap_bearer_context_removal_handler& get_e1ap_bearer_context_removal_handler() override { return *e1ap; }
  e1ap_statistics_handler&             get_e1ap_statistics_handler() override { return *e1ap; }

  void update_ue_index(ue_index_t ue_index, ue_index_t old_ue_index) override;

private:
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
  common_ue_manager&     ue_mng;
  cu_up_task_scheduler&  task_sched;
  task_executor&         ctrl_exec;

  cu_up_processor_context context;

  // Components
  std::unique_ptr<e1ap_interface> e1ap;

  // E1AP to CU-UP processor adapter
  e1ap_cu_up_processor_adapter e1ap_ev_notifier;
};

} // namespace srs_cu_cp
} // namespace srsran
