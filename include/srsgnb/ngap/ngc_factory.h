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
#include "srsgnb/cu_cp/ue_manager.h"
#include "srsgnb/support/timers.h"
#include <memory>

namespace srsgnb {

namespace srs_cu_cp {

/// Creates an instance of an NGC interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<ngc_interface> create_ngc(ngc_ue_task_scheduler&    task_sched_,
                                          ngc_ue_manager&           ue_manager_,
                                          ngc_message_notifier&     ngc_notifier_,
                                          ngc_e1_control_notifier&  e1_ctrl_notifier_,
                                          ngc_f1c_control_notifier& f1c_ctrl_notifier_);

} // namespace srs_cu_cp

} // namespace srsgnb