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

#include "srsran/ngap/gateways/n2_connection_client_factory.h"
#include "srsran/asn1/ngap/common.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/gateways/sctp_network_client_factory.h"
#include "srsran/gateways/sctp_network_gateway_factory.h"
#include "srsran/ngap/ngap_message.h"
#include "srsran/pcap/dlt_pcap.h"

using namespace srsran;
using namespace srs_cu_cp;

namespace {

/// Notifier for converting packed NGAP PDUs coming from the N2 GW into unpacked NGAP PDUs and forward them to the
/// CU-CP.
class sctp_to_n2_pdu_notifier final : public sctp_association_sdu_notifier
{
public:
  sctp_to_n2_pdu_notifier(std::unique_ptr<ngap_message_notifier> du_rx_pdu_notifier_,
                          dlt_pcap&                              pcap_writer_,
                          srslog::basic_logger&                  logger_) :
    du_rx_pdu_notifier(std::move(du_rx_pdu_notifier_)), pcap_writer(pcap_writer_), logger(logger_)
  {
  }

  bool on_new_sdu(byte_buffer sdu) override
  {
    // Unpack NGAP PDU.
    asn1::cbit_ref bref(sdu);
    ngap_message   msg;
    if (msg.pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
      logger.error("Couldn't unpack NGAP PDU");
      return false;
    }

    // Forward Rx PDU to pcap, if enabled.
    if (pcap_writer.is_write_enabled()) {
      pcap_writer.push_pdu(sdu.copy());
    }

    // Forward unpacked Rx PDU to the DU.
    du_rx_pdu_notifier->on_new_message(msg);

    return true;
  }

private:
  std::unique_ptr<ngap_message_notifier> du_rx_pdu_notifier;
  dlt_pcap&                              pcap_writer;
  srslog::basic_logger&                  logger;
};

/// \brief Notifier for converting unpacked NGAP PDUs coming from the CU-CP into packed NGAP PDUs and forward them to
/// the N2 GW.
class n2_to_sctp_pdu_notifier final : public ngap_message_notifier
{
public:
  n2_to_sctp_pdu_notifier(std::unique_ptr<sctp_association_sdu_notifier> sctp_rx_pdu_notifier_,
                          dlt_pcap&                                      pcap_writer_,
                          srslog::basic_logger&                          logger_) :
    sctp_rx_pdu_notifier(std::move(sctp_rx_pdu_notifier_)), pcap_writer(pcap_writer_), logger(logger_)
  {
  }

  void on_new_message(const ngap_message& msg) override
  {
    // pack NGAP PDU into SCTP SDU.
    byte_buffer   tx_sdu{byte_buffer::fallback_allocation_tag{}};
    asn1::bit_ref bref(tx_sdu);
    if (msg.pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
      logger.error("Failed to pack NGAP PDU");
      return;
    }

    // Push Tx PDU to pcap.
    if (pcap_writer.is_write_enabled()) {
      pcap_writer.push_pdu(tx_sdu.copy());
    }

    // Forward packed Tx PDU to SCTP gateway.
    sctp_rx_pdu_notifier->on_new_sdu(std::move(tx_sdu));
  }

private:
  std::unique_ptr<sctp_association_sdu_notifier> sctp_rx_pdu_notifier;
  dlt_pcap&                                      pcap_writer;
  srslog::basic_logger&                          logger;
};

/// Stub for the operation of the CU-CP without a core.
class ngap_gateway_local_stub final : public n2_connection_client
{
public:
  ngap_gateway_local_stub(dlt_pcap& pcap_) : pcap_writer(pcap_) {}

  std::unique_ptr<ngap_message_notifier>
  handle_cu_cp_connection_request(std::unique_ptr<ngap_message_notifier> cu_cp_rx_pdu_notifier) override
  {
    class cu_cp_tx_pdu_notifier final : public ngap_message_notifier
    {
    public:
      cu_cp_tx_pdu_notifier(ngap_gateway_local_stub& parent_) : parent(parent_) {}
      ~cu_cp_tx_pdu_notifier() { parent.disconnect(); }

      void on_new_message(const ngap_message& msg) override { parent.handle_tx_message(msg); }

    private:
      ngap_gateway_local_stub& parent;
    };

    // Store Rx PDU notifier
    cu_cp_rx_notifier = std::move(cu_cp_rx_pdu_notifier);

    return std::make_unique<cu_cp_tx_pdu_notifier>(*this);
  }

private:
  void disconnect() { cu_cp_rx_notifier.reset(); }

  // Handle message sent by CU-CP.
  void handle_tx_message(const ngap_message& msg)
  {
    using namespace asn1::ngap;

    // Save message to pcap.
    if (pcap_writer.is_write_enabled()) {
      byte_buffer   packed_pdu;
      asn1::bit_ref bref{packed_pdu};
      if (msg.pdu.pack(bref) == asn1::SRSASN_SUCCESS) {
        pcap_writer.push_pdu(std::move(packed_pdu));
      } else {
        logger.warning("Failed to encode NGAP Tx PDU.");
      }
    }

    if (msg.pdu.type().value == ngap_pdu_c::types_opts::init_msg and
        msg.pdu.init_msg().value.type().value == ngap_elem_procs_o::init_msg_c::types_opts::ng_setup_request) {
      // CU-CP is requesting an NG Setup. Automatically reply with NG Setup Response.

      const auto& req = msg.pdu.init_msg().value.ng_setup_request();
      srsran_assert(req->supported_ta_list.size() > 0, "NG Setup Request has no supported TA list");
      const auto& broadcast_plmns = req->supported_ta_list[0].broadcast_plmn_list;

      // Generate fake NG Setup Response.
      ngap_message resp;
      resp.pdu.set_successful_outcome().load_info_obj(ASN1_NGAP_ID_NG_SETUP);
      auto& ng_resp = resp.pdu.successful_outcome().value.ng_setup_resp();
      ng_resp->amf_name.from_string("localamf");
      ng_resp->served_guami_list.resize(broadcast_plmns.size());
      for (unsigned i = 0; i != ng_resp->served_guami_list.size(); ++i) {
        ng_resp->served_guami_list[i].guami.plmn_id = req->supported_ta_list[0].broadcast_plmn_list[i].plmn_id;
        ng_resp->served_guami_list[i].guami.amf_region_id.from_number(0x2);
        ng_resp->served_guami_list[i].guami.amf_set_id.from_number(0x40);
        ng_resp->served_guami_list[i].guami.amf_pointer.from_number(0x0);
      }
      ng_resp->relative_amf_capacity = 255;

      // Support for the same PLMNs and Slices as in NG Setup request.
      ng_resp->plmn_support_list.resize(broadcast_plmns.size());
      for (unsigned i = 0; i != broadcast_plmns.size(); ++i) {
        auto& out_plmn              = ng_resp->plmn_support_list[i];
        out_plmn.plmn_id            = broadcast_plmns[i].plmn_id;
        out_plmn.slice_support_list = broadcast_plmns[i].tai_slice_support_list;
      }

      // Send NG Setup Response back to CU-CP.
      send_rx_pdu_to_cu_cp(resp);
    }
  }

  // Forward NGAP message to CU-CP.
  void send_rx_pdu_to_cu_cp(const ngap_message& msg)
  {
    srsran_assert(cu_cp_rx_notifier != nullptr, "Adapter is disconnected");

    if (pcap_writer.is_write_enabled()) {
      // PCAP writer is enabled. Encode ASN.1 message and send to PCAP.
      byte_buffer       bytes;
      asn1::bit_ref     bref{bytes};
      asn1::SRSASN_CODE code = msg.pdu.pack(bref);
      if (code != asn1::SRSASN_SUCCESS) {
        logger.warning("Failed to encode NGAP Rx PDU. NGAP PCAP will miss some messages.");
      } else {
        pcap_writer.push_pdu(std::move(bytes));
      }
    }

    // Push message to CU-CP.
    cu_cp_rx_notifier->on_new_message(msg);
  }

  dlt_pcap&             pcap_writer;
  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-CP");

  std::unique_ptr<ngap_message_notifier> cu_cp_rx_notifier;
};

/// \brief NGAP bridge that uses the IO broker to handle the SCTP connection
class n2_sctp_gateway_client : public n2_connection_client
{
public:
  n2_sctp_gateway_client(io_broker& broker_, const sctp_network_connector_config& sctp_, dlt_pcap& pcap_) :
    broker(broker_), sctp_cfg(sctp_), pcap_writer(pcap_)
  {
    // Create SCTP network adapter.
    sctp_gateway = create_sctp_network_client(sctp_network_client_config{sctp_cfg, broker});
    report_error_if_not(sctp_gateway != nullptr, "Failed to create SCTP gateway client.\n");
  }

  std::unique_ptr<ngap_message_notifier>
  handle_cu_cp_connection_request(std::unique_ptr<ngap_message_notifier> cu_cp_rx_pdu_notifier) override
  {
    srsran_assert(cu_cp_rx_pdu_notifier != nullptr, "CU-CP Rx PDU notifier is null");

    // Establish SCTP connection and register SCTP Rx message handler.
    logger.debug("Establishing TNL connection to {} ({}:{})...",
                 sctp_cfg.dest_name,
                 sctp_cfg.connect_address,
                 sctp_cfg.connect_port);
    std::unique_ptr<sctp_association_sdu_notifier> sctp_sender = sctp_gateway->connect_to(
        sctp_cfg.dest_name,
        sctp_cfg.connect_address,
        sctp_cfg.connect_port,
        std::make_unique<sctp_to_n2_pdu_notifier>(std::move(cu_cp_rx_pdu_notifier), pcap_writer, logger));

    if (sctp_sender == nullptr) {
      logger.error(
          "Failed to establish N2 TNL connection to AMF on {}:{}.\n", sctp_cfg.connect_address, sctp_cfg.connect_port);
      return nullptr;
    }
    logger.info("{}: Connection to {} on {}:{} was established",
                sctp_cfg.if_name,
                sctp_cfg.dest_name,
                sctp_cfg.connect_address,
                sctp_cfg.connect_port);
    fmt::print("{}: Connection to {} on {}:{} completed\n",
               sctp_cfg.if_name,
               sctp_cfg.dest_name,
               sctp_cfg.connect_address,
               sctp_cfg.connect_port);

    // Return the Tx PDU notifier to the CU-CP.
    return std::make_unique<n2_to_sctp_pdu_notifier>(std::move(sctp_sender), pcap_writer, logger);
  }

private:
  io_broker&                          broker;
  const sctp_network_connector_config sctp_cfg;
  dlt_pcap&                           pcap_writer;
  srslog::basic_logger&               logger = srslog::fetch_basic_logger("CU-CP");

  // SCTP network adapter
  std::unique_ptr<sctp_network_client> sctp_gateway;
};

} // namespace

std::unique_ptr<n2_connection_client>
srsran::srs_cu_cp::create_n2_connection_client(const n2_connection_client_config& params)
{
  if (std::holds_alternative<n2_connection_client_config::no_core>(params.mode)) {
    // Connection to local AMF stub.
    return std::make_unique<ngap_gateway_local_stub>(params.pcap);
  }

  // Connection to AMF through SCTP.
  const auto&                           nw_mode = std::get<n2_connection_client_config::network>(params.mode);
  srsran::sctp_network_connector_config sctp_cfg;
  sctp_cfg.dest_name         = "AMF";
  sctp_cfg.if_name           = "N2";
  sctp_cfg.connect_address   = nw_mode.amf_address;
  sctp_cfg.connect_port      = nw_mode.amf_port;
  sctp_cfg.bind_address      = nw_mode.bind_address;
  sctp_cfg.bind_interface    = nw_mode.bind_interface;
  sctp_cfg.rto_initial       = nw_mode.rto_initial;
  sctp_cfg.rto_min           = nw_mode.rto_min;
  sctp_cfg.rto_max           = nw_mode.rto_max;
  sctp_cfg.init_max_attempts = nw_mode.init_max_attempts;
  sctp_cfg.max_init_timeo    = nw_mode.max_init_timeo;
  sctp_cfg.ppid              = NGAP_PPID;
  return std::make_unique<n2_sctp_gateway_client>(nw_mode.broker, sctp_cfg, params.pcap);
}
