
#ifndef SRSGNB_DU_MANAGER_FACTORY_H
#define SRSGNB_DU_MANAGER_FACTORY_H

#include "du_manager.h"
#include "srsgnb/du/du_cell_config.h"
#include "srsgnb/f1_interface/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/support/executors/task_executor.h"
#include "srsgnb/support/timers.h"

namespace srsgnb {
namespace srs_du {

/// Creates an instance of a DU manager.
std::unique_ptr<du_manager_interface> create_du_manager(timer_manager&                     timers,
                                                        mac_ue_configurator&               mac_ue_mng,
                                                        mac_cell_manager&                  mac_cell_mng,
                                                        f1ap_connection_manager&           f1ap_conn_mng,
                                                        f1ap_ue_context_manager&           f1ap_ue_ctx_mng,
                                                        f1ap_ul_interface&                 f1ap_ul,
                                                        rlc_sdu_rx_notifier&               rlc_ul_notifier,
                                                        task_executor&                     du_mng_exec,
                                                        const std::vector<du_cell_config>& cells);

} // namespace srs_du
} // namespace srsgnb

#endif // SRSGNB_DU_MANAGER_FACTORY_H
