
#ifndef SRSGNB_MAC_CONFIGURATION_HELPERS_H
#define SRSGNB_MAC_CONFIGURATION_HELPERS_H

#include "srsgnb/mac/cell_configuration.h"
#include "srsgnb/scheduler/scheduler_configurator.h"

namespace srsgnb {

/// Convert a cell configuration request to the MAC into a cell configuration request of the scheduler.
sched_cell_configuration_request_message
make_scheduler_cell_configuration_request(const mac_cell_creation_request& request);

} // namespace srsgnb

#endif // SRSGNB_MAC_CONFIGURATION_HELPERS_H
