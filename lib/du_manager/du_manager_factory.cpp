
#include "srsgnb/du_manager/du_manager_factory.h"
#include "du_manager_impl.h"

namespace srsgnb {

std::unique_ptr<du_manager_interface> create_du_manager(mac_configurer&       mac,
                                                        f1ap_du_configurer&   f1ap,
                                                        f1ap_du_ul_interface& f1ap_ul,
                                                        rlc_sdu_rx_notifier&  rlc_ul_notifier,
                                                        task_executor&        du_mng_exec)
{
  du_manager_config_t cfg{};
  cfg.mac             = &mac;
  cfg.f1ap            = &f1ap;
  cfg.f1ap_ul         = &f1ap_ul;
  cfg.rlc_ul_notifier = &rlc_ul_notifier;
  cfg.du_mng_exec     = &du_mng_exec;
  auto du_manager     = std::make_unique<du_manager_impl>(cfg);
  return du_manager;
}

} // namespace srsgnb
