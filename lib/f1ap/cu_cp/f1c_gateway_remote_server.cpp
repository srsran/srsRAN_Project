/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/f1ap/cu_cp/f1c_gateway_remote_server.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/gateways/sctp_network_server_factory.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

namespace {

/// Interface used by F1AP PDU notifiers to shutdown the SCTP connection.
class cu_cp_sctp_connection_controller
{
public:
  virtual ~cu_cp_sctp_connection_controller() = default;

  /// \brief Deregister the SCTP connection from the io_broker to disconnect listening to new SCTP events.
  virtual void disconnect() = 0;
};

/// Notifier passed to the CU-CP, which the CU-CP will use to push F1AP Tx PDUs.
///
/// On destruction, the notifier will disconnect the SCTP gateway.
class f1c_tx_sctp_notifier final : public f1ap_message_notifier
{
public:
  f1c_tx_sctp_notifier(sctp_network_gateway_data_handler& gateway,
                       cu_cp_sctp_connection_controller&  conn_controller_,
                       dlt_pcap&                          pcap_writer_,
                       srslog::basic_logger&              logger_) :
    gw(gateway), conn_controller(conn_controller_), pcap_writer(pcap_writer_), logger(logger_)
  {
  }

  // Called by CU-CP to shutdown the SCTP connection.
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
  cu_cp_sctp_connection_controller&  conn_controller;
  dlt_pcap&                          pcap_writer;
  srslog::basic_logger&              logger;
};

/// \brief Notifier passed to the SCTP gateway, which the SCTP gateway will use to forward F1AP Rx PDUs to the CU-CP.
class f1c_rx_pdu_sctp_notifier final : public sctp_network_gateway_control_notifier,
                                       public network_gateway_data_notifier
{
public:
  f1c_rx_pdu_sctp_notifier(cu_cp_sctp_connection_controller& connection_controller_,
                           dlt_pcap&                         pcap_writer_,
                           srslog::basic_logger&             logger_) :
    connection_controller(connection_controller_), pcap_writer(pcap_writer_), logger(logger_)
  {
  }
  ~f1c_rx_pdu_sctp_notifier() override
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
  cu_cp_sctp_connection_controller& connection_controller;
  f1ap_message_notifier*            du_rx_pdu_notifier = nullptr;
  dlt_pcap&                         pcap_writer;
  srslog::basic_logger&             logger;
};

class f1c_sctp_server final : public srs_cu_cp::f1c_connection_server
{
public:
  f1c_sctp_server(const f1c_cu_sctp_gateway_config& cfg_) : cfg(cfg_) {}

  void attach_cu_cp(srs_cu_cp::cu_cp_f1c_handler& cu_f1c_handler_) override { cu_f1c_handler = &cu_f1c_handler_; }

  optional<uint16_t> get_listen_port() const override { return nullopt; }

private:
  const f1c_cu_sctp_gateway_config cfg;
  srslog::basic_logger&            logger         = srslog::fetch_basic_logger("CU-CP-F1");
  srs_cu_cp::cu_cp_f1c_handler*    cu_f1c_handler = nullptr;
};

} // namespace

std::unique_ptr<srs_cu_cp::f1c_connection_server>
srsran::create_f1c_gateway_server(const f1c_cu_sctp_gateway_config& cfg)
{
  return std::make_unique<f1c_sctp_server>(cfg);
}
