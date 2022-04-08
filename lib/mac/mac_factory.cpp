
#include "srsgnb/mac/mac_factory.h"
#include "mac_impl.h"
#include "srsgnb/ran/du_l2_ul_executor_mapper.h"

using namespace srsgnb;

std::unique_ptr<mac_interface> srsgnb::create_mac(mac_ul_ccch_notifier&     event_notifier,
                                                  du_l2_ul_executor_mapper& ul_exec_mapper,
                                                  span<task_executor*>      dl_exec,
                                                  task_executor&            ctrl_exec,
                                                  mac_result_notifier&      phy_notifier)
{
  std::unique_ptr<mac_interface> macobj =
      std::make_unique<mac_impl>(event_notifier, ul_exec_mapper, dl_exec, ctrl_exec, phy_notifier);
  return macobj;
}
