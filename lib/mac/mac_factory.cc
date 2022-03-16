
#include "srsgnb/mac/mac_factory.h"
#include "mac_impl.h"
#include "srsgnb/ran/du_l2_executor_mapper.h"

namespace srsgnb {

std::unique_ptr<mac_interface> create_mac(mac_rx_sdu_notifier&   ul_ccch_notifier,
                                          du_l2_executor_mapper& ul_exec_mapper,
                                          span<task_executor*>   dl_exec,
                                          task_executor&         ctrl_exec)
{
  std::unique_ptr<mac_interface> macobj =
      std::make_unique<mac_impl>(ul_ccch_notifier, ul_exec_mapper, dl_exec, ctrl_exec);
  return macobj;
}

} // namespace srsgnb