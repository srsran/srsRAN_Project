
#ifndef SRSGNB_MAC_FACTORY_H
#define SRSGNB_MAC_FACTORY_H

#include "mac.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/ran/du_l2_ul_executor_mapper.h"
#include <memory>

namespace srsgnb {

std::unique_ptr<mac_interface> create_mac(mac_sdu_rx_notifier&   ul_ccch_notifier,
                                          du_l2_ul_executor_mapper& ul_exec_mapper,
                                          span<task_executor*>   dl_exec,
                                          task_executor&         ctrl_exec);

}

#endif // SRSGNB_MAC_FACTORY_H
