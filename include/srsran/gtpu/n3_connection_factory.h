/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/gateways/udp_network_gateway.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

class io_broker;

namespace srs_cu_up {

/// Handler of a N3 connection between CU-UP and UPF.
class n3_connection : public udp_network_gateway_data_handler, public network_gateway_data_notifier_with_src_addr
{
public:
  virtual ~n3_connection() = default;

  virtual optional<uint16_t> get_n3_bind_port() = 0;
};

/// \brief Factory that instantiates N3 connections.
class n3_connection_factory
{
public:
  virtual ~n3_connection_factory() = default;

  /// \brief Create a new instance of a UDP network gateway.
  /// \param[in] data_notifier The notifier to be used by the gateway to notify about received data.
  /// \return Returns a new instance of a N3 connection.
  virtual std::unique_ptr<n3_connection> create(network_gateway_data_notifier_with_src_addr& data_notifier) = 0;
};

/// Creates a factory of N3 connections that establish an UDP-based connection to a remote UPF.
/// \param[in] config Configuration of the UDP network gateway.
/// \param[in] io_brk IO broker that will manage the reception of new PDUs from the UDP socket.
/// \param[in] io_tx_executor Executor that will be used to handle the transmission of PDUs to the UDP socket.
/// \return Returns a factory of N3 connections.
std::unique_ptr<n3_connection_factory> create_udp_n3_connection_factory(const udp_network_gateway_config& config,
                                                                        io_broker&                        io_brk,
                                                                        task_executor& io_tx_executor);

/// \brief Instance of a stub UPF using a local N3 connection.
class local_upf_stub
{
public:
  virtual ~local_upf_stub() = default;

  virtual void handle_pdu(byte_buffer pdu, const sockaddr_storage& dest_addr) = 0;

  virtual void register_tx_pdu_handler(network_gateway_data_notifier_with_src_addr& data_notifier) = 0;
};

/// Creates a factory of UDP network gateways that establish a direct link to a stub UPF.
/// \param[in] Stub of an UPF that will be co-located with the CU-UP.
/// \return Returns a factory of N3 connections.
std::unique_ptr<n3_connection_factory> create_no_upf_n3_connection_factory(local_upf_stub& upf_stub);

} // namespace srs_cu_up
} // namespace srsran