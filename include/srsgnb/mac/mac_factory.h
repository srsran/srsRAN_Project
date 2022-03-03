
#ifndef SRSGNB_MAC_FACTORY_H
#define SRSGNB_MAC_FACTORY_H

#include "mac.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/support/task_executor.h"
#include <memory>

namespace srsgnb {

std::unique_ptr<mac_interface> create_mac(mac_ul_sdu_notifier& ul_ccch_notifier,
                                          task_executor&       ul_exec,
                                          span<task_executor*> dl_exec,
                                          task_executor&       ctrl_exec);

}

#endif // SRSGNB_MAC_FACTORY_H
