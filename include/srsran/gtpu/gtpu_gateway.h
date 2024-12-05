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

/// \brief Handler of the transmission and reception of User-plane (UP) Transport Network Layer (TNL) PDUs
/// via a GTP-U interface.
///
/// In particular, this class handles:
/// - the reception of PDUs by the CU-UP via the NG-U interface, or the DU/CU-UP via the F1-U interface
/// - the transmission of PDUs to the UPF via the NG-U interface, or the DU/CU-UP via the F1-U interface and
/// - the creation and deallocation of the required network resources for the NG-U/F1-U interface.
class gtpu_tnl_pdu_session : public udp_network_gateway_data_handler, public network_gateway_data_notifier_with_src_addr
{
public:
  ~gtpu_tnl_pdu_session() override = default;

  /// \brief Get the address to which the socket is bound.
  ///
  /// In case the gateway was configured to use a hostname,
  /// this function can be used to get the actual IP address in string form.
  virtual bool get_bind_address(std::string& ip_address) const = 0;

  /// Get bind port currently being used by the GTP-U TNL session for the reception of PDUs.
  /// \return If a UDP link is being used, returns the respective bind port. If the connection is local, it returns
  /// std::nullopt.
  virtual std::optional<uint16_t> get_bind_port() const = 0;
};

/// \brief This class is called by the CU-UP to instantiate new GTP-U TNL PDU sessions.
class gtpu_gateway
{
public:
  virtual ~gtpu_gateway() = default;

  /// \brief Creates a new GTP-U TNL session and starts listening for incoming PDUs.
  /// \param[in] data_notifier The notifier to be used by the instantiated GTP-U TNL session to push received PDUs back
  /// to the CU-UP.
  /// \return Returns a new instance of a NG-U TNL PDU session.
  virtual std::unique_ptr<gtpu_tnl_pdu_session> create(network_gateway_data_notifier_with_src_addr& data_notifier) = 0;
};

/// Creates a factory of GTP-U TNL sessions that handle the allocation of network resources for a UDP-based link.
/// \param[in] config Configuration of the UDP network gateway.
/// \param[in] io_brk IO broker that will manage the reception of new PDUs from the UDP socket.
/// \param[in] io_tx_executor Executor that will be used to handle the transmission of PDUs to the UDP socket.
/// \return Returns the instantiated GTP-U gateway. Returns nullptr, if the GTP-U gateway could not be established.
std::unique_ptr<gtpu_gateway>
create_udp_gtpu_gateway(const udp_network_gateway_config& config, io_broker& io_brk, task_executor& io_tx_executor);

/// \brief Creates a GTP-U gateway that establishes a connection to a null/dummy UPF.
///
/// This type of GTP-U gateway is useful when testing the CU-UP without a real UPF.
/// \return Returns the instantiated GTP-U gateway.
std::unique_ptr<gtpu_gateway> create_no_core_gtpu_gateway();

} // namespace srsran
