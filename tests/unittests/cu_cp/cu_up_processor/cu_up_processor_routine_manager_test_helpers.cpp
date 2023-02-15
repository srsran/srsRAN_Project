/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_processor_routine_manager_test_helpers.h"
#include "srsgnb/support/async/async_test_utils.h"

using namespace srsgnb;
using namespace srs_cu_cp;

cu_up_processor_routine_manager_test::cu_up_processor_routine_manager_test()
{
  test_logger.set_level(srslog::basic_levels::debug);
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  context.cu_cp_name  = "srs_cu_cp";
  context.cu_up_index = cu_up_index_t::min;

  task_sched = std::make_unique<dummy_cu_up_processor_task_scheduler>(timers);

  // create routine manager
  routine_mng =
      std::make_unique<cu_up_processor_routine_manager>(context, e1ap_conn_notifier, cu_cp_notifier, *task_sched.get());
}

cu_up_processor_routine_manager_test::~cu_up_processor_routine_manager_test()
{
  // flush logger after each test
  srslog::flush();
}
