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

#include "srsgnb/scheduler/config/scheduler_config.h"
#include "srsgnb/scheduler/mac_scheduler.h"

namespace srsgnb {

/// Create L2 scheduler object.
std::unique_ptr<mac_scheduler> create_scheduler(const scheduler_config&       sched_cfg,
                                                sched_configuration_notifier& config_notifier);

} // namespace srsgnb
