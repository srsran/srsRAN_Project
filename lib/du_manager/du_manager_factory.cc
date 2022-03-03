
#include "srsgnb/du_manager/du_manager_factory.h"
#include "du_manager_impl.h"

namespace srsgnb {

std::unique_ptr<du_manager_interface> create_du_manager(mac_configurer&       mac,
                                                        du_manager_config_notifier& f1ap_notifier,
                                                        rlc_ul_sdu_notifier&        rlc_ul_notifier,
                                                        task_executor&              du_mng_exec)
{
  du_manager_config_t cfg{};
  cfg.mac               = &mac;
  cfg.f1ap_cfg_notifier = &f1ap_notifier;
  cfg.rlc_ul_notifier   = &rlc_ul_notifier;
  cfg.du_mng_exec       = &du_mng_exec;
  auto du_manager       = std::make_unique<du_manager_impl>(cfg);
  return du_manager;
}

} // namespace srsgnb