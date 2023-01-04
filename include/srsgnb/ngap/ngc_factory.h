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
#include "srsgnb/support/timers.h"
#include <memory>

namespace srsgnb {

namespace srs_cu_cp {

/// Creates an instance of an NGC interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<ngc_interface> create_ngc(ngc_configuration&     ngc_cfg_,
                                          ngc_ue_task_scheduler& task_sched_,
                                          ngc_ue_manager&        ue_manager_,
                                          ngc_message_notifier&  ngc_notifier_);

} // namespace srs_cu_cp

} // namespace srsgnb