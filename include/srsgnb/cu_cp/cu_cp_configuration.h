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

#include "srsgnb/e1/cu_cp/e1_cu_cp.h"
#include "srsgnb/f1ap/cu_cp/f1ap_cu.h"
#include "srsgnb/ngap/ngap.h"
#include "srsgnb/ngap/ngap_configuration.h"
#include "srsgnb/support/executors/task_executor.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Configuration passed to CU CP.
struct cu_cp_configuration {
  task_executor*         cu_cp_executor = nullptr;
  f1c_message_notifier*  f1c_notifier   = nullptr; /// Callback for incoming F1C messages.
  e1_message_notifier*   e1_notifier    = nullptr; /// Callback for incoming E1 messages.
  ngap_message_notifier* ngap_notifier  = nullptr; /// Callack for outgoing NGAP messages
  ngap_configuration     ngap_config;
};

} // namespace srs_cu_cp
} // namespace srsgnb
