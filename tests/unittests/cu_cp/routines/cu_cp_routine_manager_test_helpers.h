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

#include "../test_helpers.h"
#include "lib/cu_cp/routine_managers/cu_cp_routine_manager.h"
#include "lib/cu_cp/ue_manager/ue_manager_impl.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/ran/five_qi.h"
#include "srsran/support/executors/manual_task_worker.h"
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

  pdcp_config pdcp_cfg{pdcp_rb_type::drb,
                       pdcp_rlc_mode::am,
                       {},
                       {},
                       {pdcp_sn_size::size12bits},
                       {pdcp_sn_size::size12bits}};

  up_resource_manager_cfg up_config{{{uint_to_five_qi(9), {pdcp_cfg}}, {uint_to_five_qi(7), {pdcp_cfg}}}};

  timer_manager                                      timers;
  manual_task_worker                                 ctrl_worker{128};
  dummy_e1ap_bearer_context_manager                  e1ap_bearer_ctxt_mng;
  dummy_f1ap_ue_context_manager                      f1ap_ue_ctxt_mng;
  dummy_ngap_control_message_handler                 ngap_control_handler;
  dummy_ue_task_scheduler                            ue_task_sched{timers, ctrl_worker};
  ue_manager                                         ue_mng{ue_config, up_config, {}, timers, ctrl_worker};
  dummy_du_processor_rrc_ue_control_message_notifier rrc_ue_ctrl_notifier;
  dummy_du_processor_rrc_ue_srb_control_notifier     rrc_ue_srb_ctrl_notifier;
  dummy_ngap_ue_context_removal_handler              ngap_ue_removal_handler;
  dummy_cu_cp_ue_removal_handler                     ue_removal_handler{&ue_mng};
  std::unique_ptr<cu_cp_routine_manager>             routine_mng;
};

} // namespace srs_cu_cp
} // namespace srsran
