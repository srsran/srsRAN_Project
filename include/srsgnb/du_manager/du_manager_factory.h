
#ifndef SRSGNB_DU_MANAGER_FACTORY_H
#define SRSGNB_DU_MANAGER_FACTORY_H

#include "du_manager.h"
#include "srsgnb/f1ap/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"
#include <memory>

namespace srsgnb {

/// Creates an instance of a PDCP interface.
std::unique_ptr<du_manager_interface>
create_du_manager(mac_config_interface& mac, rlc_config_interface& rlc, du_manager_config_notifier& f1ap_notifier);

} // namespace srsgnb

#endif // SRSGNB_DU_MANAGER_FACTORY_H
