/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/gateways/sctp_network_server_factory.h"
#include "sctp_network_server_impl.h"

using namespace srsran;

std::unique_ptr<sctp_network_server> srsran::create_sctp_network_server(const sctp_network_server_config& config)
{
  return sctp_network_server_impl::create(
      config.sctp, config.broker, config.io_rx_executor, config.association_handler_factory);
}
