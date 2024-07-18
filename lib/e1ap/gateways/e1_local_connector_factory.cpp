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

#include "srsran/e1ap/gateways/e1_local_connector_factory.h"
#include "srsran/cu_cp/cu_cp_e1_handler.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/e1ap/gateways/e1_network_client_factory.h"
#include "srsran/e1ap/gateways/e1_network_server_factory.h"
#include "srsran/pcap/dlt_pcap.h"

using namespace srsran;

namespace {

/// Decorator for e1ap_message_notifier that writes the forwarded PDU to a pcap file.
class e1ap_pdu_pcap_notifier final : public e1ap_message_notifier
{
public:
  e1ap_pdu_pcap_notifier(std::unique_ptr<e1ap_message_notifier> decorated_,
                         dlt_pcap&                              pcap_writer_,
                         srslog::basic_logger&                  logger_) :
    logger(logger_), pcap_writer(pcap_writer_), decorated(std::move(decorated_))
  {
    srsran_sanity_check(pcap_writer.is_write_enabled(), "Pcap writing must be enabled.");
  }

  void on_new_message(const e1ap_message& msg) override
  {
    byte_buffer   buf;
    asn1::bit_ref bref(buf);
    if (msg.pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
      logger.error("Failed to pack PDU");
    } else {
      pcap_writer.push_pdu(std::move(buf));
    }

    // Forward message to decorated class.
    decorated->on_new_message(msg);
  }

private:
  srslog::basic_logger&                  logger;
  dlt_pcap&                              pcap_writer;
  std::unique_ptr<e1ap_message_notifier> decorated;
};

/// Implementation of a CU-UP and CU-CP E1 gateway for the case that the CU-UP and CU-CP are co-located.
class e1_local_connector_impl final : public e1_local_connector
{
public:
  e1_local_connector_impl(const e1_local_connector_config& cfg) : pcap_writer(cfg.pcap) {}

  void attach_cu_cp(srs_cu_cp::cu_cp_e1_handler& cu_cp_e1_mng_) override { cu_cp_e1_mng = &cu_cp_e1_mng_; }

  std::optional<uint16_t> get_listen_port() const override { return std::nullopt; }

  std::unique_ptr<e1ap_message_notifier>
  handle_cu_up_connection_request(std::unique_ptr<e1ap_message_notifier> cu_up_notifier) override
  {
    report_fatal_error_if_not(cu_cp_e1_mng != nullptr, "CU-CP has not been attached to E1 gateway.");

    // Decorate CU-UP RX notifier with pcap writing.
    if (pcap_writer.is_write_enabled()) {
      cu_up_notifier = std::make_unique<e1ap_pdu_pcap_notifier>(
          std::move(cu_up_notifier), pcap_writer, srslog::fetch_basic_logger("CU-UP-E1"));
    }

    // Create direct connection between CU-CP and CU-UP notifier.
    auto cu_notifier = cu_cp_e1_mng->handle_new_cu_up_connection(std::move(cu_up_notifier));

    // Decorate CU-CP RX notifier with pcap writing.
    if (pcap_writer.is_write_enabled()) {
      cu_notifier = std::make_unique<e1ap_pdu_pcap_notifier>(
          std::move(cu_notifier), pcap_writer, srslog::fetch_basic_logger("CU-CP-E1"));
    }

    return cu_notifier;
  }

private:
  dlt_pcap&                    pcap_writer;
  srs_cu_cp::cu_cp_e1_handler* cu_cp_e1_mng = nullptr;
};

/// Implementation of a CU-UP and CU-CP E1 SCTP-based gateway for the case that the CU-UP and CU-CP are co-located.
///
/// Note: This class should only be used for testing purposes.
class e1_sctp_connector_impl final : public e1_local_connector
{
public:
  e1_sctp_connector_impl(const e1_local_sctp_connector_config& cfg) : broker(cfg.broker), pcap_writer(cfg.pcap)
  {
    // Create SCTP server.
    sctp_network_gateway_config sctp;
    sctp.if_name      = "E1";
    sctp.ppid         = E1AP_PPID;
    sctp.bind_address = "127.0.0.1";
    // Use any bind port available.
    sctp.bind_port = cfg.bind_port;
    server         = create_e1_gateway_server(e1_cu_cp_sctp_gateway_config{sctp, broker, pcap_writer});
  }

  void attach_cu_cp(srs_cu_cp::cu_cp_e1_handler& cu_e1_handler_) override
  {
    server->attach_cu_cp(cu_e1_handler_);

    // Create SCTP client.
    sctp_network_connector_config sctp_client;
    sctp_client.if_name         = "E1";
    sctp_client.dest_name       = "CU-CP";
    sctp_client.connect_address = "127.0.0.1";
    sctp_client.connect_port    = server->get_listen_port().value();
    sctp_client.ppid            = E1AP_PPID;
    // Note: We only need to save the PCAPs in one side of the connection.
    client = create_e1_gateway_client(e1_cu_up_sctp_gateway_config{sctp_client, broker, *null_pcap_writer});
  }

  std::optional<uint16_t> get_listen_port() const override { return server->get_listen_port(); }

  std::unique_ptr<e1ap_message_notifier>
  handle_cu_up_connection_request(std::unique_ptr<e1ap_message_notifier> cu_up_rx_pdu_notifier) override
  {
    // Connect client to server automatically.
    return client->handle_cu_up_connection_request(std::move(cu_up_rx_pdu_notifier));
  }

private:
  io_broker&                                       broker;
  dlt_pcap&                                        pcap_writer;
  std::unique_ptr<dlt_pcap>                        null_pcap_writer = create_null_dlt_pcap();
  std::unique_ptr<srs_cu_cp::e1_connection_server> server;
  std::unique_ptr<srs_cu_up::e1_connection_client> client;
};

} // namespace

std::unique_ptr<e1_local_connector> srsran::create_e1_local_connector(const e1_local_connector_config& cfg)
{
  return std::make_unique<e1_local_connector_impl>(cfg);
}

std::unique_ptr<e1_local_connector> srsran::create_e1_local_connector(const e1_local_sctp_connector_config& cfg)
{
  return std::make_unique<e1_sctp_connector_impl>(cfg);
}
