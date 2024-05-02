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

#include "../test_helpers.h"
#include "lib/cu_cp/routine_managers/cu_cp_routine_manager.h"
#include "lib/cu_cp/ue_manager/ue_manager_impl.h"
#include "lib/cu_cp/up_resource_manager/up_resource_manager_impl.h"
#include <gtest/gtest.h>

namespace srsran {
namespace srs_cu_cp {

/// Fixture class for DU processor creation
class cu_cp_routine_manager_test : public ::testing::Test
{
protected:
  cu_cp_routine_manager_test();
  ~cu_cp_routine_manager_test() override;

  void init_security_config();

  srslog::basic_logger& test_logger  = srslog::fetch_basic_logger("TEST");
  srslog::basic_logger& cu_cp_logger = srslog::fetch_basic_logger("CU-CP");

  ue_configuration                ue_config{std::chrono::seconds{10}};
  srsran::security::sec_as_config security_cfg;
  security_indication_t           default_security_indication = {};
  up_resource_manager_cfg         drb_cfg;

  timer_manager                                         timers;
  manual_task_worker                                    ctrl_worker{128};
  dummy_e1ap_bearer_context_manager                     e1ap_bearer_ctxt_mng;
  dummy_f1ap_ue_context_manager                         f1ap_ue_ctxt_mng;
  dummy_du_processor_ngap_control_notifier              ngap_control_notifier;
  std::unique_ptr<dummy_du_processor_ue_task_scheduler> ue_task_sched;
  ue_manager                                            ue_mng{ue_config, drb_cfg, timers, ctrl_worker};
  dummy_du_processor_rrc_ue_control_message_notifier    rrc_ue_ctrl_notifier;
  dummy_du_processor_rrc_ue_srb_control_notifier        rrc_ue_srb_ctrl_notifier;
  dummy_ngap_ue_context_removal_handler                 ngap_ue_removal_handler;
  dummy_cu_cp_ue_removal_handler                        ue_removal_handler{&ue_mng};
  std::unique_ptr<up_resource_manager_impl>             rrc_ue_up_resource_manager;
  std::unique_ptr<cu_cp_routine_manager>                routine_mng;
};

} // namespace srs_cu_cp
} // namespace srsran
