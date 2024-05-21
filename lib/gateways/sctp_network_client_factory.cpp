/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/gateways/sctp_network_client_factory.h"
#include "sctp_network_client_impl.h"

using namespace srsran;

std::unique_ptr<sctp_network_client> srsran::create_sctp_network_client(const sctp_network_client_config& config)
{
  return sctp_network_client_impl::create(config.client_name, config.sctp, config.broker);
}