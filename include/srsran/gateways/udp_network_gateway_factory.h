/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "udp_network_gateway.h"
#include <memory>
#include <utility>

namespace srsran {

struct udp_network_gateway_creation_message {
  udp_network_gateway_creation_message(udp_network_gateway_config     config_,
                                       network_gateway_data_notifier& data_notifier_) :
    config(std::move(config_)), data_notifier(data_notifier_)
  {
  }
  udp_network_gateway_config     config;
  network_gateway_data_notifier& data_notifier;
};

/// Creates an instance of an network gateway
std::unique_ptr<udp_network_gateway> create_udp_network_gateway(udp_network_gateway_creation_message msg);

} // namespace srsran
