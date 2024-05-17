/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "sctp_network_server_impl.h"

using namespace srsran;

sctp_network_server_impl::sctp_network_server_impl(const srsran::sctp_network_node_config& sctp_cfg_) :
  sctp_network_gateway_common_impl(sctp_cfg_)
{
}

sctp_network_server_impl::~sctp_network_server_impl() {}

bool sctp_network_server_impl::create_and_bind()
{
  return this->common_create_and_bind();
}

void sctp_network_server_impl::receive()
{
  // TODO
}

bool sctp_network_server_impl::listen()
{
  return socket.listen();
}

optional<uint16_t> sctp_network_server_impl::get_listen_port()
{
  return socket.get_listen_port();
}

bool sctp_network_server_impl::subscribe_to(io_broker& broker)
{
  // TODO
  return true;
}

void sctp_network_server_impl::attach_association_handler(sctp_network_association_factory& factory)
{
  // TODO
}
