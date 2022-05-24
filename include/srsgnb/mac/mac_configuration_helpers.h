
#ifndef SRSGNB_MAC_CONFIGURATION_HELPERS_H
#define SRSGNB_MAC_CONFIGURATION_HELPERS_H

#include "srsgnb/mac/cell_configuration.h"
#include "srsgnb/mac/mac_ue_configurator.h"
#include "srsgnb/scheduler/scheduler_configurator.h"

namespace srsgnb {

/// Convert a cell configuration request to the MAC into a cell configuration request of the scheduler.
sched_cell_configuration_request_message
make_scheduler_cell_configuration_request(const mac_cell_creation_request& request);

/// Convert a UE creation request for the MAC into a UE creation request of the scheduler.
sched_ue_creation_request_message make_scheduler_ue_creation_request(const mac_ue_create_request_message& request);

} // namespace srsgnb

#endif // SRSGNB_MAC_CONFIGURATION_HELPERS_H
