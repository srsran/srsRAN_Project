
#include "srsgnb/mac/mac_factory.h"
#include "mac_impl.h"

namespace srsgnb {

std::unique_ptr<mac_interface> create_mac(mac_config_notifier& listener, mac_northbound_notifier& ul_sdu_notifier)
{
  std::unique_ptr<mac_interface> macobj = std::make_unique<mac_impl>(listener, ul_sdu_notifier);
  return macobj;
}

} // namespace srsgnb