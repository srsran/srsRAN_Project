
#include "srsgnb/mac/mac_factory.h"
#include "mac_impl.h"

namespace srsgnb {

std::unique_ptr<mac_interface> create_mac(mac_config_notifier&     listener,
                                          mac_northbound_notifier& ul_sdu_notifier,
                                          task_executor&           ul_exec,
                                          span<task_executor*>     dl_exec)
{
  std::unique_ptr<mac_interface> macobj = std::make_unique<mac_impl>(listener, ul_sdu_notifier, ul_exec, dl_exec);
  return macobj;
}

} // namespace srsgnb