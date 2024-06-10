/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/gateways/udp_network_gateway.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

class io_broker;

namespace srs_cu_up {

/// \brief Handler of the transmission and reception of User-plane (UP) Transport Network Layer (TNL) PDUs by the CU-UP
/// via the NG-U interface.
///
/// In particular, this class handles:
/// - the reception of PDUs by the CU-UP via the NG-U interface,
/// - the transmission of PDUs to the UPF via the NG-U interface, and
/// - the creation and deallocation of the required network resources for the link with the UPF via the NG-U interface.
class ngu_tnl_pdu_session : public udp_network_gateway_data_handler, public network_gateway_data_notifier_with_src_addr
{
public:
  ~ngu_tnl_pdu_session() override = default;

  /// \brief Get the address to which the socket is bound.
  ///
  /// In case the gateway was configured to use a hostname,
  /// this function can be used to get the actual IP address in string form.
  virtual bool get_bind_address(std::string& ip_address) = 0;

  /// Get bind port currently being used by the NG-U TNL session for the reception of PDUs.
  /// \return If a UDP link is being used, returns the respective bind port. If the connection is local, it returns
  /// std::nullopt.
  virtual std::optional<uint16_t> get_bind_port() = 0;
};

/// \brief This class is called by the CU-UP to instantiate new NG-U TNL PDU sessions.
class ngu_gateway
{
public:
  virtual ~ngu_gateway() = default;

  /// \brief Creates a new NG-U TNL session and starts listening for incoming PDUs.
  /// \param[in] data_notifier The notifier to be used by the instantiated NG-U TNL session to push received PDUs back
  /// to the CU-UP.
  /// \return Returns a new instance of a NG-U TNL PDU session.
  virtual std::unique_ptr<ngu_tnl_pdu_session> create(network_gateway_data_notifier_with_src_addr& data_notifier) = 0;
};

/// Creates a factory of NG-U TNL sessions that handle the allocation of network resources for a UDP-based link.
/// \param[in] config Configuration of the UDP network gateway.
/// \param[in] io_brk IO broker that will manage the reception of new PDUs from the UDP socket.
/// \param[in] io_tx_executor Executor that will be used to handle the transmission of PDUs to the UDP socket.
/// \return Returns the instantiated NG-U gateway. Returns nullptr, if the NG-U gateway could not be established.
std::unique_ptr<ngu_gateway>
create_udp_ngu_gateway(const udp_network_gateway_config& config, io_broker& io_brk, task_executor& io_tx_executor);

/// \brief Creates a NG-U gateway that establishes a connection to a null/dummy UPF.
///
/// This type of NG-U gateway is useful when testing the CU-UP without a real UPF.
/// \return Returns the instantiated NG-U gateway.
std::unique_ptr<ngu_gateway> create_no_core_ngu_gateway();

} // namespace srs_cu_up
} // namespace srsran
