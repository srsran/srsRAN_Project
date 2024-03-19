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
  virtual ~ngu_tnl_pdu_session() = default;

  /// Get bind port currently being used by the NG-U TNL session for the reception of PDUs.
  /// \return If a UDP link is being used, returns the respective bind port. If the connection is local, it returns
  /// nullopt.
  virtual optional<uint16_t> get_bind_port() = 0;
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

/// \brief Instance of a stub UPF using a local N3 connection (no sockets).
class local_upf_stub
{
public:
  virtual ~local_upf_stub() = default;

  /// Called when the UPF receives a new packed PDU from the CU-UP.
  virtual void handle_pdu(byte_buffer pdu, const sockaddr_storage& dest_addr) = 0;

  /// Register the handler of the PDUs transmitted by the UPF to the CU-UP.
  virtual void register_tx_pdu_handler(network_gateway_data_notifier_with_src_addr& data_notifier) = 0;
};

/// \brief Creates a NG-U gateway that establish direct local links to a stub UPF.
///
/// This type of NG-U gateway is useful when testing the CU-UP without a real UPF.
/// \param[in] Stub of an UPF that will be co-located with the CU-UP.
/// \return Returns the instantiated NG-U gateway.
std::unique_ptr<ngu_gateway> create_no_core_ngu_gateway(local_upf_stub& upf_stub);

} // namespace srs_cu_up
} // namespace srsran