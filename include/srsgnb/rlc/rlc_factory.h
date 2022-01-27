
#ifndef SRSGNB_RLC_MANAGER_FACTORY_H
#define SRSGNB_RLC_MANAGER_FACTORY_H

#include "rlc.h"
#include <memory>

namespace srsgnb {

/// Creates an instance of a RLC interface.
std::unique_ptr<rlc_interface> create_rlc(rlc_config_notifier& config_notifier);

} // namespace srsgnb

#endif // SRSGNB_RLC_MANAGER_FACTORY_H
