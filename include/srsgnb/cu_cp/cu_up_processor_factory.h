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

#include "cu_up_processor.h"
#include "cu_up_processor_config.h"
#include "srsgnb/e1/common/e1_common.h"
#include "srsgnb/support/executors/task_executor.h"
#include <memory>

namespace srsgnb {
namespace srs_cu_cp {

/// Creates an instance of an CU-UP processor interface
std::unique_ptr<cu_up_processor_interface>
create_cu_up_processor(const cu_up_processor_config_t             cu_up_processor_config_,
                       cu_up_processor_cu_up_management_notifier& cu_cp_notifier_,
                       e1ap_message_notifier&                     e1ap_notifier_,
                       cu_up_processor_task_scheduler&            task_sched_,
                       task_executor&                             ctrl_exec_);

} // namespace srs_cu_cp
} // namespace srsgnb
