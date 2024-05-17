/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1c_gateway_remote_connector.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/gateways/sctp_network_gateway_factory.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/support/io/io_broker.h"

using namespace srsran;

namespace {

/// Interface used by F1AP PDU notifiers to shutdown the SCTP connection.
class du_sctp_connection_controller
{
public:
  virtual ~du_sctp_connection_controller() = default;

  /// \brief Deregister the SCTP connection from the io_broker to disconnect listening to new SCTP events.
  virtual void disconnect() = 0;
};

/// Notifier passed to the DU, which the DU will use to push F1AP Tx PDUs.
///
/// On destruction, the notifier will disconnect the SCTP gateway.
class f1c_tx_sctp_notifier final : public f1ap_message_notifier
{
public:
  f1c_tx_sctp_notifier(sctp_network_gateway_data_handler& gateway,
                       du_sctp_connection_controller&     conn_controller_,
                       dlt_pcap&                          pcap_writer_,
                       srslog::basic_logger&              logger_) :
    gw(gateway), conn_controller(conn_controller_), pcap_writer(pcap_writer_), logger(logger_)
  {
  }

  // Called by DU to shutdown the SCTP connection.
  ~f1c_tx_sctp_notifier() override { conn_controller.disconnect(); }

  /// Handle unpacked Tx F1AP PDU by packing and forwarding it into the SCTP GW.
  void on_new_message(const f1ap_message& msg) override
  {
    // pack PDU into temporary buffer
    byte_buffer   tx_pdu{byte_buffer::fallback_allocation_tag{}};
    asn1::bit_ref bref(tx_pdu);
    if (msg.pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
      logger.error("Failed to pack F1AP PDU");
      return;
    }

    // Push Tx PDU to pcap.
    if (pcap_writer.is_write_enabled()) {
      pcap_writer.push_pdu(tx_pdu.copy());
    }

    // Forward packed Tx PDU to SCTP gateway.
    gw.handle_pdu(tx_pdu);
  }

private:
  sctp_network_gateway_data_handler& gw;
  du_sctp_connection_controller&     conn_controller;
  dlt_pcap&                          pcap_writer;
  srslog::basic_logger&              logger;
};

/// \brief Notifier passed to the SCTP gateway, which the SCTP gateway will use to forward F1AP Rx PDUs to the DU.
class f1c_rx_pdu_sctp_notifier final : public sctp_network_gateway_control_notifier,
                                       public network_gateway_data_notifier
{
public:
  f1c_rx_pdu_sctp_notifier(du_sctp_connection_controller& connection_controller_,
                           dlt_pcap&                      pcap_writer_,
                           srslog::basic_logger&          logger_) :
    connection_controller(connection_controller_), pcap_writer(pcap_writer_), logger(logger_)
  {
  }
  ~f1c_rx_pdu_sctp_notifier()
  {
    du_rx_pdu_notifier = nullptr;
    connection_controller.disconnect();
  }

  /// Connect the notifier that will be used to forward F1AP Rx PDUs to the DU.
  void connect_rx_pdu_notifier(f1ap_message_notifier& du_rx_pdu_notifier_)
  {
    du_rx_pdu_notifier = &du_rx_pdu_notifier_;
  }

  void on_connection_established() override
  {
    // TODO
  }

  // Called by SCTP GW on SCTP connection loss.
  void on_connection_loss() override
  {
    du_rx_pdu_notifier = nullptr;
    connection_controller.disconnect();
  }

  // Called by SCTP GW to forward an F1AP Rx PDU to the DU.
  void on_new_pdu(byte_buffer pdu) override
  {
    if (du_rx_pdu_notifier == nullptr) {
      logger.warning("Dropping F1AP Rx PDU. Cause: PDU received after connection loss");
      return;
    }

    // Unpack F1AP PDU.
    asn1::cbit_ref bref(pdu);
    f1ap_message   msg;
    if (msg.pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
      logger.error("Couldn't unpack F1AP PDU");
      return;
    }

    // Forward Rx PDU to pcap, if enabled.
    if (pcap_writer.is_write_enabled()) {
      pcap_writer.push_pdu(pdu.copy());
    }

    // Forward unpacked Rx PDU to the DU.
    du_rx_pdu_notifier->on_new_message(msg);
  }

private:
  du_sctp_connection_controller& connection_controller;
  f1ap_message_notifier*         du_rx_pdu_notifier = nullptr;
  dlt_pcap&                      pcap_writer;
  srslog::basic_logger&          logger;
};

class f1c_sctp_gateway_connector final : public srs_du::f1c_connection_client, public du_sctp_connection_controller
{
public:
  f1c_sctp_gateway_connector(const f1c_gateway_params& params) :
    pcap_writer(params.pcap),
    broker(params.broker),
    sctp_params(params.sctp),
    sctp_f1ap_adapter(*this, pcap_writer, logger)
  {
    // Create SCTP network adapter.
    sctp_gateway = create_sctp_network_gateway(
        sctp_network_gateway_creation_message{params.sctp, sctp_f1ap_adapter, sctp_f1ap_adapter});
    report_error_if_not(sctp_gateway != nullptr, "Failed to create SCTP gateway.\n");
  }

  void disconnect() override
  {
    // Stop receiving SCTP messages and close socket.
    sctp_gateway.reset();

    // Signal back to the DU that the SCTP Rx connection has been stopped.
    // Note: It is now safe to destroy the RX PDU notifier, as no more Rx PDUs will be pushed to it from within
    // the io_broker execution context.
    rx_pdu_notifier.reset();
  }

  std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier) override
  {
    srsran_assert(du_rx_pdu_notifier != nullptr, "DU Rx PDU notifier is null");

    // Connect the Rx PDU notifier to the SCTP GW.
    rx_pdu_notifier = std::move(du_rx_pdu_notifier);
    sctp_f1ap_adapter.connect_rx_pdu_notifier(*rx_pdu_notifier);

    // Establish SCTP connection and register SCTP Rx message handler.
    logger.debug(
        "Establishing TNL connection to CU-CP ({}:{})...", sctp_params.connect_address, sctp_params.connect_port);
    if (not sctp_gateway->create_and_connect()) {
      logger.error("Failed to create SCTP gateway.\n");
      return nullptr;
    }
    logger.info("TNL connection to CU-CP ({}:{}) established", sctp_params.connect_address, sctp_params.connect_port);

    // Subscribe SCTP gateway to IO broker to start listening to Rx events.
    if (not sctp_gateway->subscribe_to(broker)) {
      logger.error("Failed to register N2 (SCTP) network gateway at IO broker. socket_fd={}",
                   sctp_gateway->get_socket_fd());
      return nullptr;
    }

    // Return the Tx PDU notifier to the DU.
    return std::make_unique<f1c_tx_sctp_notifier>(*sctp_gateway, *this, pcap_writer, logger);
  }

private:
  dlt_pcap&                             pcap_writer;
  io_broker&                            broker;
  srsran::sctp_network_connector_config sctp_params;
  srslog::basic_logger&                 logger = srslog::fetch_basic_logger("DU-F1");

  // Adapter between SCTP client and F1AP ASN.1 packer.
  f1c_rx_pdu_sctp_notifier sctp_f1ap_adapter;

  // SCTP network gateway
  std::unique_ptr<sctp_network_gateway> sctp_gateway;

  // DU F1AP Rx PDU notifier.
  std::unique_ptr<f1ap_message_notifier> rx_pdu_notifier;
};

} // namespace

std::unique_ptr<srs_du::f1c_connection_client> srsran::create_f1c_gateway_connector(const f1c_gateway_params& params)
{
  return std::make_unique<f1c_sctp_gateway_connector>(params);
}
