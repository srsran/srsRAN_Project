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

#include "cu_up_processor_routine_manager_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"

using namespace srsran;
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
