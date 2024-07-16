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
#include "cu_up_processor_test_helpers.h"
#include "lib/cu_cp/cu_up_processor/cu_up_processor_factory.h"
#include "lib/cu_cp/cu_up_processor/cu_up_processor_impl_interface.h"
#include "lib/cu_cp/task_schedulers/cu_up_task_scheduler.h"
#include "lib/cu_cp/ue_manager/ue_manager_impl.h"
#include "tests/unittests/e1ap/cu_cp/e1ap_cu_cp_test_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

namespace srsran {
namespace srs_cu_cp {

/// Fixture class for DU processor creation
class cu_up_processor_test : public ::testing::Test
{
protected:
  cu_up_processor_test();
  ~cu_up_processor_test() override;

  srslog::basic_logger& test_logger  = srslog::fetch_basic_logger("TEST");
  srslog::basic_logger& cu_cp_logger = srslog::fetch_basic_logger("CU-CP");

  timer_manager               timers;
  dummy_e1ap_message_notifier e1ap_notifier;
  manual_task_worker          ctrl_worker{128};
  cu_cp_configuration         cu_cp_cfg;

  ue_manager                                      ue_mng{cu_cp_cfg};
  dummy_e1ap_cu_cp_notifier                       cu_cp_notifier{ue_mng};
  std::unique_ptr<cu_up_processor_impl_interface> cu_up_processor_obj;
  cu_up_task_scheduler task_sched{timers, ctrl_worker, cu_cp_cfg.admission.max_nof_cu_ups, test_logger};
};

} // namespace srs_cu_cp
} // namespace srsran
