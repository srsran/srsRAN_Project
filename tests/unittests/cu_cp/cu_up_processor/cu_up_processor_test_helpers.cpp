/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_processor_test_helpers.h"
#include "srsgnb/support/async/async_test_utils.h"

using namespace srsgnb;
using namespace srs_cu_cp;

cu_up_processor_test::cu_up_processor_test()
{
  test_logger.set_level(srslog::basic_levels::debug);
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  task_sched = std::make_unique<dummy_cu_up_processor_task_scheduler>(timers);

  // create and start CU-UP processor
  cu_up_processor_config_t cu_up_cfg = {};
  cu_up_cfg.index                    = uint_to_cu_up_index(0);

  cu_up_processor_obj =
      create_cu_up_processor(std::move(cu_up_cfg), cu_cp_notifier, e1_notifier, *task_sched, ctrl_worker);
}

cu_up_processor_test::~cu_up_processor_test()
{
  // flush logger after each test
  srslog::flush();
}
