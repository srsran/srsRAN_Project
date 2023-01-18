/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/gateways/network_gateway.h"
#include "srsgnb/srslog/srslog.h"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace srsgnb {

struct sctp_network_gateway_config : common_network_gateway_config {
  std::string connect_address;
  int         connect_port = 0;
  // TODO add SCTP specific options
};

/// Interface to inject PDUs into gateway entity.
class sctp_network_gateway_data_handler
{
public:
  virtual ~sctp_network_gateway_data_handler() = default;

  /// \brief Handle the incoming PDU.
  /// \param[in]  put Byte-buffer with new PDU.
  virtual void handle_pdu(const byte_buffer& pdu) = 0;
};

class sctp_network_gateway : public network_gateway, public sctp_network_gateway_data_handler
{
public:
  explicit sctp_network_gateway(sctp_network_gateway_config       config_,
                                network_gateway_control_notifier& ctrl_notfier_,
                                network_gateway_data_notifier&    data_notifier_);
  virtual ~sctp_network_gateway() { close_socket(); }

  /// \brief Create and connect socket to given address.
  bool create_and_connect();

  /// \brief Return socket file descriptor.
  int get_socket_fd() override;

  /// \brief Trigger receive call on socket.
  void receive() override;

  // network_gateway_data_handler interface
  void handle_pdu(const byte_buffer& pdu) override;

  // network_gateway_controller interface
  /// \brief Create and bind socket to given address.
  bool create_and_bind() override;

  /// \brief Start listening on socket.
  bool listen();

private:
  bool is_initialized();

  bool set_sockopts();

  /// \brief Recreate and reconnect socket to given address.
  bool recreate_and_reconnect() override;

  /// \brief Return socket bind port.
  int get_bind_port() override;

  // internal helpers
  using socket_buffer_type = uint8_t;
  void handle_data(span<socket_buffer_type> payload);
  void handle_notification(span<socket_buffer_type> payload);

  // socket helpers
  bool set_non_blocking();
  bool set_receive_timeout(unsigned rx_timeout_sec);
  bool set_reuse_addr();
  bool subscripe_to_events();
  bool close_socket();

  sctp_network_gateway_config       config; /// configuration
  network_gateway_control_notifier& ctrl_notifier;
  network_gateway_data_notifier&    data_notifier;
  srslog::basic_logger&             logger;

  int sock_fd = -1;

  struct sockaddr_storage client_addr        = {}; // the local address
  socklen_t               client_addrlen     = 0;
  int                     client_ai_family   = 0;
  int                     client_ai_socktype = 0;
  int                     client_ai_protocol = 0;

  struct sockaddr_storage server_addr        = {}; // the server address when operating as client
  socklen_t               server_addrlen     = 0;
  int                     server_ai_family   = 0;
  int                     server_ai_socktype = 0;
  int                     server_ai_protocol = 0;

  unsigned ppi       = 0; /// the Payload Protocol Identifier (e.g. 18 for S1AP)
  unsigned stream_no = 0; /// the stream number to use for sending
};

} // namespace srsgnb
