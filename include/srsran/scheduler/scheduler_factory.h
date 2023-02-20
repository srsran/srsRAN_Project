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

#include "srsran/scheduler/config/scheduler_config.h"
#include "srsran/scheduler/mac_scheduler.h"

namespace srsran {

/// Create L2 scheduler object.
std::unique_ptr<mac_scheduler> create_scheduler(const scheduler_config& sched_cfg);

} // namespace srsran
