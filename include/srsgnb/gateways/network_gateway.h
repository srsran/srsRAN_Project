#pragma once

#include "srsgnb/adt/byte_buffer.h"

namespace srsgnb {

/// \brief Generic network gateway interfaces to connect components to the outside world.

enum class network_gateway_type {
  sctp, ///< SCTP gateway
  udp   ///< UDP gateway
};

constexpr uint32_t network_gateway_sctp_max_len = 9100;
constexpr uint32_t network_gateway_udp_max_len  = 9100;

struct network_gateway_config {
  network_gateway_type type = network_gateway_type::sctp;
  std::string          connect_address;
  int                  connect_port = 0;
  std::string          bind_address;
  int                  bind_port         = 0;
  bool                 non_blocking_mode = false;
  unsigned             rx_timeout_sec    = 1; /// Socket received timeout in seconds
  bool                 reuse_addr        = false;
};

/// Interface to inject PDUs into gateway entity.
class network_gateway_data_handler
{
public:
  virtual ~network_gateway_data_handler() = default;

  /// \brief Handle the incoming PDU.
  /// \param[in]  put Byte-buffer with new PDU.
  virtual void handle_pdu(const byte_buffer& pdu) = 0;
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

  /// \brief Start listening on socket.
  virtual bool listen() = 0;

  /// \brief Create and connect to the configured address and port.
  virtual bool create_and_connect() = 0;

  /// \brief Recreate a formerly closed socket and reconnect to a known address and port.
  virtual bool recreate_and_reconnect() = 0;

  /// \brief Trigger receive call on socket.
  virtual void receive() = 0;

  /// \brief Return socket file descriptor.
  virtual int get_socket_fd() = 0;
};

class network_gateway : public network_gateway_data_handler, public network_gateway_controller
{};

} // namespace srsgnb
