/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ngap/ngap.h"
#include "srsran/ngap/ngap_configuration.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {
namespace srs_cu_cp {

/// Configuration passed to CU CP.
struct cu_cp_configuration {
  task_executor*         cu_cp_executor = nullptr;
  f1ap_message_notifier* f1ap_notifier  = nullptr; /// Callback for incoming F1AP messages.
  e1ap_message_notifier* e1ap_notifier  = nullptr; /// Callback for incoming E1AP messages.
  ngap_message_notifier* ngap_notifier  = nullptr; /// Callack for outgoing NGAP messages.
  ngap_configuration     ngap_config;
};

} // namespace srs_cu_cp
} // namespace srsran
