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

#pragma once

#include "../ue_manager.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/async/async_task.h"

namespace srsran::srs_cu_up {

class cu_up_e1_connection_loss_routine
{
public:
  cu_up_e1_connection_loss_routine(gnb_cu_up_id_t           cu_up_id_,
                                   std::string              cu_up_name_,
                                   std::string              plmn_,
                                   std::atomic<bool>&       stop_command,
                                   e1ap_connection_manager& e1ap_conn_mng_,
                                   ue_manager&              ue_mng_,
                                   timer_manager&           timers,
                                   task_executor&           ctrl_exec);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "CU-UP E1 connection loss routine"; }

private:
  gnb_cu_up_id_t     cu_up_id;
  std::string        cu_up_name;
  std::string        plmn;
  std::atomic<bool>& stop_command;

  unique_timer             retry_timer;
  e1ap_connection_manager& e1ap_conn_mng;
  ue_manager&              ue_mng;
  srslog::basic_logger&    logger;

  bool reconnected;
};

} // namespace srsran::srs_cu_up
