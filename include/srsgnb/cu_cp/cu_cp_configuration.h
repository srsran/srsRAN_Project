/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CU_CP_CONFIGURATION_H
#define SRSGNB_CU_CP_CONFIGURATION_H

#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/support/executors/task_executor.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Configuration passed to CU CP.
struct cu_cp_configuration {
  task_executor*       cu_executor = nullptr;
  f1c_message_notifier* f1c_notifier = nullptr; /// Callback for outgoing messeges.
};

} // namespace srs_cu_cp
} // namespace srsgnb

#endif // SRSGNB_CU_CP_CONFIGURATION_H
