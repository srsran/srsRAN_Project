/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "cu_cp_manager.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"

#include "srsgnb/support/executors/task_executor.h"
#include "srsgnb/support/timers.h"

namespace srsgnb {

namespace srs_cu_cp {

/// Creates an instance of a CU-CP manager.
std::unique_ptr<cu_cp_manager_interface> create_cu_cp_manager(timer_manager&              timers,
                                                              f1c_du_management_notifier& f1c_du_management_notifier,
                                                              f1c_message_notifier&       f1c_notifier,
                                                              task_executor&              cu_cp_mng_exec);

} // namespace srs_cu_cp

} // namespace srsgnb
