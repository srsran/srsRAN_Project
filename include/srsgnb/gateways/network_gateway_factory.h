/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "network_gateway.h"
#include <memory>

namespace srsgnb {

struct network_gateway_creation_message {
  network_gateway_creation_message(network_gateway_config            config_,
                                   network_gateway_control_notifier& ctrl_notifier_,
                                   network_gateway_data_notifier&    data_notifier_) :
    config(config_), ctrl_notifier(ctrl_notifier_), data_notifier(data_notifier_)
  {
  }
  network_gateway_config            config;
  network_gateway_control_notifier& ctrl_notifier;
  network_gateway_data_notifier&    data_notifier;
};

/// Creates an instance of an network gateway
std::unique_ptr<network_gateway> create_network_gateway(network_gateway_creation_message msg);

} // namespace srsgnb
