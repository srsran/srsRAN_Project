/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SCHEDULER_SCHEDULER_FACTORY_H
#define SRSGNB_SCHEDULER_SCHEDULER_FACTORY_H

#include "srsgnb/scheduler/mac_scheduler.h"

namespace srsgnb {

/// Create L2 scheduler object.
std::unique_ptr<mac_scheduler> create_scheduler(sched_configuration_notifier& config_notifier);

} // namespace srsgnb

#endif // SRSGNB_SCHEDULER_SCHEDULER_FACTORY_H
