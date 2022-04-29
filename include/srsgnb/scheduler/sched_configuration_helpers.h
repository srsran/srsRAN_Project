
#ifndef SRSGNB_SCHED_CONFIGURATION_HELPERS_H
#define SRSGNB_SCHED_CONFIGURATION_HELPERS_H

#include "srsgnb/scheduler/sched_configurator.h"

namespace srsgnb {

/// From MAC cell configuration, generate a scheduler cell configuration request message.
cell_configuration_request_message
make_sched_cell_configuration_request_message(const mac_cell_configuration& cell_cfg);

} // namespace srsgnb

#endif // SRSGNB_SCHED_CONFIGURATION_HELPERS_H
