/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/gateways/udp_network_gateway_factory.h"
#include "srsran/support/error_handling.h"
#include "udp_network_gateway_impl.h"

using namespace srsran;

std::unique_ptr<udp_network_gateway> srsran::create_udp_network_gateway(udp_network_gateway_creation_message msg)
{
  return std::make_unique<udp_network_gateway_impl>(msg.config, msg.data_notifier);
}
