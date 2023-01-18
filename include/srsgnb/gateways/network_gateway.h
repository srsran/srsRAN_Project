#pragma once

#include "srsgnb/adt/byte_buffer.h"

namespace srsgnb {

/// \brief Generic network gateway interfaces to connect components to the outside world.

constexpr uint32_t network_gateway_sctp_max_len = 9100;
constexpr uint32_t network_gateway_udp_max_len  = 9100;

/// \brief Common parameters to all network gateways. Specific gateway
/// implementations will further specify parameters according to their
/// needs.
struct common_network_gateway_config {
  std::string bind_address;
  int         bind_port         = 0;
  bool        non_blocking_mode = false;
  unsigned    rx_timeout_sec    = 1; /// Socket received timeout in seconds
  bool        reuse_addr        = false;
};

/// Interface to inform upper layers about reception of new PDUs.
class network_gateway_data_notifier
{
public:
  virtual ~network_gateway_data_notifier() = default;

  /// \brief This callback is invoked on each received PDU.
  /// \param[in]  put Byte-buffer with new PDU.
  virtual void on_new_pdu(byte_buffer pdu) = 0;
};

/// Interface to inform upper layers about connection establishment, drops, etc.
class network_gateway_control_notifier
{
public:
  virtual ~network_gateway_control_notifier() = default;

  /// \brief This callback is invoked when connect() succeeds (client) or a new client connected to a listening socket
  /// (server).
  virtual void on_connection_established() = 0;

  /// \brief This callback is invoked when the connection is dropped or cannot be established in the first place.
  virtual void on_connection_loss() = 0;
};

/// Interface to trigger bind/listen/connect operations on gateway socket.
class network_gateway_controller
{
public:
  virtual ~network_gateway_controller() = default;

  /// \brief Create and bind the configured address and port.
  virtual bool create_and_bind() = 0;

  /// \brief Recreate a formerly closed socket and reconnect to a known address and port.
  virtual bool recreate_and_reconnect() = 0;

  /// \brief Trigger receive call on socket.
  virtual void receive() = 0;

  /// \brief Return socket file descriptor.
  virtual int get_socket_fd() = 0;

  /// \brief Return the port to which the socket is bound.
  ///
  /// In case the gateway was configured to bind to port 0, i.e. the operating system shall pick a random free port,
  /// this function can be used to get the actual port number.
  virtual int get_bind_port() = 0;
};

class network_gateway : public network_gateway_controller
{
public:
  virtual ~network_gateway() = default;
};

} // namespace srsgnb
