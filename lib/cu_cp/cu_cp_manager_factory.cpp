/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/cu_cp/cu_cp_manager_factory.h"
#include "cu_cp_manager_impl.h"
#include "srsgnb/support/timers.h"

using namespace srsgnb;
using namespace srs_cu_cp;

std::unique_ptr<cu_cp_manager_interface> srsgnb::srs_cu_cp::create_cu_cp_manager(timer_manager&           timers,
                                                                                 f1ap_connection_manager& f1ap_conn_mng,
                                                                                 task_executor& cu_cp_mng_exec)
{
  cu_cp_manager_config_t cfg{};
  cfg.timers         = &timers;
  cfg.cu_cp_mng_exec = &cu_cp_mng_exec;
  cfg.f1ap_conn_mng  = &f1ap_conn_mng;
  auto cu_cp_manager = std::make_unique<cu_cp_manager_impl>(std::move(cfg));
  return cu_cp_manager;
}
