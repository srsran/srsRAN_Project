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

#include "e1ap_cu_cp.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"
#include <memory>

namespace srsran {
namespace srs_cu_cp {

/// Creates an instance of an E1AP interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<e1ap_interface> create_e1ap(srsran::timer_manager&         timers_,
                                            e1ap_message_notifier&         e1ap_pdu_notifier_,
                                            e1ap_cu_up_processor_notifier& e1ap_cu_up_processor_notifier_,
                                            task_executor&                 ctrl_exec_);

} // namespace srs_cu_cp
} // namespace srsran
