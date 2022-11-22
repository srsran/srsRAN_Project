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

#include "srsgnb/e1_interface/cu_cp/e1_cu_cp.h"
#include "srsgnb/f1c/cu_cp/f1c_cu.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/support/executors/task_executor.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Configuration passed to CU CP.
struct cu_cp_configuration {
  task_executor*        cu_executor  = nullptr;
  f1c_message_notifier* f1c_notifier = nullptr; /// Callback for incoming F1C messages.
  e1_message_notifier*  e1_notifier  = nullptr; /// Callback for incoming E1 messages.
  ngc_message_notifier* ngc_notifier = nullptr; /// Callack for outgoing NGC messages
  ngc_configuration     ngc_config;
};

} // namespace srs_cu_cp
} // namespace srsgnb
