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

#include "ngc.h"
#include "ngc_configuration.h"
#include "srsgnb/cu_cp/ue_manager.h"
#include "srsgnb/support/executors/task_executor.h"
#include "srsgnb/support/timers.h"
#include <memory>

namespace srsgnb {

namespace srs_cu_cp {

/// Creates an instance of an NGAP interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<ngap_interface> create_ngap(ngap_configuration&     ngap_cfg_,
                                            ngap_ue_task_scheduler& task_sched_,
                                            ngap_ue_manager&        ue_manager_,
                                            ngap_message_notifier&  ngap_notifier_,
                                            task_executor&          ctrl_exec_);

} // namespace srs_cu_cp

} // namespace srsgnb