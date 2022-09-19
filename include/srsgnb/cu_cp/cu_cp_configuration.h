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

#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/support/executors/task_executor.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Configuration passed to CU CP.
struct cu_cp_configuration {
  task_executor*        cu_executor  = nullptr;
  f1c_message_notifier* f1c_notifier = nullptr; /// Callback for incoming F1AP messages.
  ngc_message_notifier* ngc_notifier = nullptr; /// Callack for outgoing NGC messages
};

} // namespace srs_cu_cp
} // namespace srsgnb
