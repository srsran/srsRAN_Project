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

std::unique_ptr<cu_cp_manager_interface>
srsgnb::srs_cu_cp::create_cu_cp_manager(timer_manager&              timers,
                                        f1c_du_management_notifier& f1c_du_mgmt_notifier,
                                        f1c_message_notifier&       f1c_notifier,
                                        task_executor&              cu_cp_mng_exec)
{
  cu_cp_manager_config_t cfg{};
  cfg.timers               = &timers;
  cfg.f1c_du_mgmt_notifier = &f1c_du_mgmt_notifier;
  cfg.f1c_notifier         = &f1c_notifier;
  cfg.cu_cp_mng_exec       = &cu_cp_mng_exec;
  auto cu_cp_manager       = std::make_unique<cu_cp_manager_impl>(std::move(cfg));
  return cu_cp_manager;
}
