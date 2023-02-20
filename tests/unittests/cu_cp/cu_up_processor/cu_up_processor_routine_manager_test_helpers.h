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

#include "../common/test_helpers.h"
#include "lib/cu_cp/routine_managers/cu_up_processor_routine_manager.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

namespace srsran {
namespace srs_cu_cp {

/// Fixture class for DU processor creation
class cu_up_processor_routine_manager_test : public ::testing::Test
{
protected:
  cu_up_processor_routine_manager_test();
  ~cu_up_processor_routine_manager_test() override;

  srslog::basic_logger& test_logger  = srslog::fetch_basic_logger("TEST");
  srslog::basic_logger& cu_cp_logger = srslog::fetch_basic_logger("CU-CP");

  timer_manager                                         timers;
  cu_up_processor_context                               context;
  dummy_cu_up_processor_e1ap_control_notifier           e1ap_conn_notifier;
  dummy_cu_up_processor_cu_up_management_notifier       cu_cp_notifier;
  std::unique_ptr<dummy_cu_up_processor_task_scheduler> task_sched;
  std::unique_ptr<cu_up_processor_routine_manager>      routine_mng;
};

} // namespace srs_cu_cp
} // namespace srsran
