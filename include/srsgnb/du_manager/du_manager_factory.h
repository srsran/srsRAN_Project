
#ifndef SRSGNB_DU_MANAGER_FACTORY_H
#define SRSGNB_DU_MANAGER_FACTORY_H

#include "du_manager.h"
#include "srsgnb/f1_interface/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/support/task_executor.h"
#include <memory>

namespace srsgnb {

/// Creates an instance of a PDCP interface.
std::unique_ptr<du_manager_interface> create_du_manager(mac_configurer&       mac,
                                                        du_manager_config_notifier& f1ap_notifier,
                                                        rlc_sdu_rx_notifier&        rlc_ul_notifier,
                                                        task_executor&              du_mng_exec);

} // namespace srsgnb

#endif // SRSGNB_DU_MANAGER_FACTORY_H
