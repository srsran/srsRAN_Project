
#include "srsgnb/du_manager/du_manager_factory.h"
#include "du_manager_impl.h"

namespace srsgnb {

std::unique_ptr<du_manager_interface>
create_du_manager(mac_config_interface& mac, rlc_config_interface& rlc, du_manager_config_notifier& f1ap_notifier)
{
  auto du_manager = std::make_unique<du_manager_impl>(rlc, mac, f1ap_notifier);
  return du_manager;
}

} // namespace srsgnb