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
#include "cu_up_processor_test_helpers.h"
#include "tests/unittests/e1ap/common/test_helpers.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/cu_cp/cu_up_processor.h"
#include "srsgnb/cu_cp/cu_up_processor_factory.h"
#include "srsgnb/support/executors/manual_task_worker.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

namespace srsgnb {
namespace srs_cu_cp {

/// Fixture class for DU processor creation
class cu_up_processor_test : public ::testing::Test
{
protected:
  cu_up_processor_test();
  ~cu_up_processor_test() override;

  srslog::basic_logger& test_logger  = srslog::fetch_basic_logger("TEST");
  srslog::basic_logger& cu_cp_logger = srslog::fetch_basic_logger("CU-CP");

  timer_manager                                         timers;
  dummy_cu_up_processor_cu_up_management_notifier       cu_cp_notifier;
  dummy_e1ap_message_notifier                           e1ap_notifier;
  std::unique_ptr<dummy_cu_up_processor_task_scheduler> task_sched;
  manual_task_worker                                    ctrl_worker{128};
  std::unique_ptr<cu_up_processor_interface>            cu_up_processor_obj;
};

} // namespace srs_cu_cp
} // namespace srsgnb
