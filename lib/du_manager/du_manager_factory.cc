
#include "srsgnb/du_manager/du_manager_factory.h"
#include "du_manager_impl.h"

namespace srsgnb {

std::unique_ptr<du_manager_interface> create_du_manager(mac_config_interface&       mac,
                                                        du_manager_config_notifier& f1ap_notifier,
                                                        rlc_ul_sdu_notifier&        rlc_ul_notifier,
                                                        task_executor&              du_mng_exec)
{
  auto du_manager = std::make_unique<du_manager_impl>(mac, f1ap_notifier, rlc_ul_notifier, du_mng_exec);
  return du_manager;
}

} // namespace srsgnb