
#ifndef SRSGNB_SCHEDULER_FACTORY_H
#define SRSGNB_SCHEDULER_FACTORY_H

#include "srsgnb/scheduler/mac_scheduler.h"

namespace srsgnb {

/// Create L2 scheduler object.
std::unique_ptr<mac_scheduler> create_scheduler(sched_configuration_notifier& config_notifier);

} // namespace srsgnb

#endif // SRSGNB_SCHEDULER_FACTORY_H
