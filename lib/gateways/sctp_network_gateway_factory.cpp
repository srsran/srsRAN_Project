/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/gateways/sctp_network_gateway_factory.h"
#include "sctp_network_gateway_impl.h"
#include "sctp_network_server_impl.h"

using namespace srsran;

std::unique_ptr<sctp_network_gateway> srsran::create_sctp_network_gateway(sctp_network_gateway_creation_message msg)
{
  return std::make_unique<sctp_network_gateway_impl>(msg.config, msg.ctrl_notifier, msg.data_notifier);
}
