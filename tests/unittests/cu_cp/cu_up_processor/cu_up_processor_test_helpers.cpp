/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_processor_test_helpers.h"
#include "srsran/cu_cp/cu_cp_configuration_helpers.h"
#include "srsran/support/async/async_test_utils.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_up_processor_test::cu_up_processor_test() :
  cu_cp_cfg([this]() {
    cu_cp_configuration cucfg          = config_helpers::make_default_cu_cp_config();
    cucfg.services.timers              = &timers;
    cucfg.services.cu_cp_executor      = &ctrl_worker;
    cu_cp_cfg.admission.max_nof_cu_ups = 4;
    return cucfg;
  }())
{
  test_logger.set_level(srslog::basic_levels::debug);
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  // create and start CU-UP processor
  cu_up_processor_config_t cu_up_cfg = {};
  cu_up_cfg.cu_up_index              = uint_to_cu_up_index(0);

  cu_up_processor_obj =
      create_cu_up_processor(std::move(cu_up_cfg), e1ap_notifier, cu_cp_notifier, task_sched, ctrl_worker);
}

cu_up_processor_test::~cu_up_processor_test()
{
  // flush logger after each test
  srslog::flush();
}
