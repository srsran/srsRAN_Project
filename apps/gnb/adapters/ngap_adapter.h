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

#include "lib/ngap/ngc_asn1_packer.h"
#include "srsgnb/gateways/sctp_network_gateway_factory.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/support/io_broker/io_broker.h"

namespace srsgnb {

namespace srs_cu_cp {

/// \brief NGAP bridge that uses the IO broker to handle the SCTP connection
class ngap_network_adapter : public ngc_message_notifier,
                             public ngc_message_handler,
                             public sctp_network_gateway_control_notifier,
                             public network_gateway_data_notifier
{
public:
  ngap_network_adapter(io_broker& broker_, const sctp_network_gateway_config& nw_config_) :
    broker(broker_),
    nw_config(nw_config_),
    gw(create_sctp_network_gateway({nw_config, *this, *this})),
    packer(*gw, *this)
  {
    gw->create_and_connect();
    broker.register_fd(gw->get_socket_fd(), [this](int fd) { gw->receive(); });
  }

  ~ngap_network_adapter() { broker.unregister_fd(gw->get_socket_fd()); }

  void connect_ngc(ngc_message_handler* ngc_msg_handler_, ngc_event_handler* ngc_evt_handler_)
  {
    ngc_msg_handler = ngc_msg_handler_;
    ngc_evt_handler = ngc_evt_handler_;
  }

private:
  // NGAP calls interface to send (unpacked) NGAP PDUs
  void on_new_message(const ngc_message& msg) override { packer.handle_message(msg); }

  // SCTP network gateway calls interface to inject received PDUs (ASN1 packed)
  void on_new_pdu(byte_buffer pdu) override { packer.handle_packed_pdu(pdu); }

  // The packer calls this interface to inject unpacked NGAP PDUs
  void handle_message(const ngc_message& msg) override
  {
    report_fatal_error_if_not(ngc_msg_handler, "NGC handler not set.");
    ngc_msg_handler->handle_message(msg);
  }

  void on_connection_loss() override
  {
    report_fatal_error_if_not(ngc_evt_handler, "NGC handler not set.");
    ngc_evt_handler->handle_connection_loss();
  }

  void on_connection_established() override
  {
    // TODO: extend event interface to inform about connection establishment
    logger.info("on_connection_established");
  }

  io_broker&                            broker;
  const sctp_network_gateway_config&    nw_config;
  std::unique_ptr<sctp_network_gateway> gw;
  ngc_asn1_packer                       packer;
  ngc_message_handler*                  ngc_msg_handler = nullptr;
  ngc_event_handler*                    ngc_evt_handler = nullptr;

  srslog::basic_logger& logger = srslog::fetch_basic_logger("NGAP-ADAPTER");
};

}; // namespace srs_cu_cp

}; // namespace srsgnb
