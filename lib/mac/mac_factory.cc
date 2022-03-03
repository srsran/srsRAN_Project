
#include "srsgnb/mac/mac_factory.h"
#include "mac_impl.h"

namespace srsgnb {

std::unique_ptr<mac_interface> create_mac(mac_ul_sdu_notifier& ul_ccch_notifier,
                                          task_executor&       ul_exec,
                                          span<task_executor*> dl_exec,
                                          task_executor&       ctrl_exec)
{
  std::unique_ptr<mac_interface> macobj = std::make_unique<mac_impl>(ul_ccch_notifier, ul_exec, dl_exec, ctrl_exec);
  return macobj;
}

} // namespace srsgnb