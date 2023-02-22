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

#include "../common/test_helpers.h"
#include "lib/cu_cp/routine_managers/cu_up_processor_routine_manager.h"
#include "srsran/support/test_utils.h"
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
