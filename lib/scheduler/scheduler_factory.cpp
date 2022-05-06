
#include "srsgnb/scheduler/scheduler_factory.h"
#include "sched.h"

using namespace srsgnb;

std::unique_ptr<mac_scheduler> srsgnb::create_scheduler(sched_configuration_notifier& config_notifier)
{
  return std::make_unique<sched>(config_notifier);
}
