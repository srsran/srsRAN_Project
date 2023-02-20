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

#include "e1ap_cu_up.h"
#include "srsgnb/support/executors/task_executor.h"
#include "srsgnb/support/timers.h"
#include <memory>

namespace srsran {
namespace srs_cu_up {

/// Creates an instance of an E1AP interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<e1ap_interface> create_e1ap(e1ap_message_notifier& e1ap_pdu_notifier_,
                                            e1ap_cu_up_notifier&   cu_up_notifier_,
                                            task_executor&         cu_up_exec_);

} // namespace srs_cu_up
} // namespace srsran
