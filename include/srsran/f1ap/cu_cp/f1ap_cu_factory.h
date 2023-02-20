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

#include "f1ap_cu.h"
#include "srsran/support/executors/task_executor.h"
#include <memory>

namespace srsran {
namespace srs_cu_cp {

/// Creates an instance of an F1AP interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<f1ap_cu> create_f1ap(f1ap_message_notifier&       f1ap_pdu_notifier_,
                                     f1ap_du_processor_notifier&  f1ap_du_processor_notifier_,
                                     f1ap_du_management_notifier& f1ap_du_management_notifier_,
                                     task_executor&               ctrl_exec_);

} // namespace srs_cu_cp
} // namespace srsran
