/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/gateways/network_gateway_factory.h"
#include "sctp_network_gateway.h"
#include "udp_network_gateway.h"
#include "srsgnb/support/error_handling.h"

using namespace srsgnb;

std::unique_ptr<network_gateway> srsgnb::create_network_gateway(network_gateway_creation_message msg)
{
  switch (msg.config.type) {
    case network_gateway_type::sctp:
      return std::make_unique<sctp_network_gateway>(msg.config, msg.ctrl_notifier, msg.data_notifier);
    case network_gateway_type::udp:
      return std::make_unique<udp_network_gateway>(msg.config, msg.data_notifier);
    default:
      srsgnb_terminate("Network gateway type not supported");
  }
  return nullptr;
}
