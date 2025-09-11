/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "cu_up_processor_test_helpers.h"
#include "srsran/cu_cp/cu_cp_configuration_helpers.h"
#include "srsran/support/async/async_test_utils.h"

using namespace srsran;
using namespace srs_cu_cp;

class dummy_task_sched final : public common_task_scheduler
{
public:
  bool schedule_async_task(async_task<void> task) override { return task_sched.schedule(std::move(task)); }

private:
  fifo_async_task_scheduler task_sched{32};
};

cu_up_processor_test::cu_up_processor_test() :
  cu_cp_cfg([this]() {
    cu_cp_configuration cucfg          = config_helpers::make_default_cu_cp_config();
    cucfg.services.timers              = &timers;
    cucfg.services.cu_cp_executor      = &ctrl_worker;
    cu_cp_cfg.admission.max_nof_cu_ups = 4;
    return cucfg;
  }()),
  common_task_sched(std::make_unique<dummy_task_sched>())
{
  test_logger.set_level(srslog::basic_levels::debug);
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  // create and start CU-UP processor
  cu_up_processor_config_t cu_up_cfg = {"srs_cu_cp", uint_to_cu_up_index(0), cu_cp_cfg, cu_cp_logger};

  cu_up_processor_obj = create_cu_up_processor(std::move(cu_up_cfg), e1ap_notifier, cu_cp_notifier, *common_task_sched);
}

cu_up_processor_test::~cu_up_processor_test()
{
  // flush logger after each test
  srslog::flush();
}
