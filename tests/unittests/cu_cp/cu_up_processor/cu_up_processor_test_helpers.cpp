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

#include "cu_up_processor_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_up_processor_test::cu_up_processor_test()
{
  test_logger.set_level(srslog::basic_levels::debug);
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  // create and start CU-UP processor
  cu_up_processor_config_t cu_up_cfg = {};
  cu_up_cfg.cu_up_index              = uint_to_cu_up_index(0);

  cu_up_processor_obj =
      create_cu_up_processor(std::move(cu_up_cfg), e1ap_notifier, e1ap_cu_cp_notif, task_sched, ctrl_worker);
}

cu_up_processor_test::~cu_up_processor_test()
{
  // flush logger after each test
  srslog::flush();
}
