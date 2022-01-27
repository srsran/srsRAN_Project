
#ifndef SRSGNB_MAC_FACTORY_H
#define SRSGNB_MAC_FACTORY_H

#include "mac.h"
#include <memory>

namespace srsgnb {

std::unique_ptr<mac_interface> create_mac(mac_config_notifier& listener, mac_northbound_notifier& ul_sdu_notifier);

}

#endif // SRSGNB_MAC_FACTORY_H
