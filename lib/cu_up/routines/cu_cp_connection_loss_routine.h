/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../ue_manager.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_up {

class du_cell_manager;
struct du_manager_params;

class cu_cp_connection_loss_routine
{
public:
  cu_cp_connection_loss_routine(gnb_cu_up_id_t           cu_up_id_,
                                std::string              cu_up_name_,
                                std::string              plmn_,
                                std::atomic<bool>&       stop_command,
                                e1ap_connection_manager& e1ap_conn_mng_,
                                ue_manager&              ue_mng_,
                                timer_manager&           timers,
                                task_executor&           ctrl_exec);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "CU-CP connection loss routine"; }

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

} // namespace srs_cu_up
} // namespace srsran
