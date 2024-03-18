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

#include "ngap_adapter.h"
#include "srsran/asn1/ngap/common.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/gateways/sctp_network_gateway_factory.h"
#include "srsran/ngap/ngap_message.h"

using namespace srsran;
using namespace srs_cu_cp;

namespace {

/// Stub for the operation of the CU-CP without a core.
class ngap_gateway_local_stub final : public ngap_gateway_connector
{
public:
  ngap_gateway_local_stub(dlt_pcap& pcap_) : pcap_writer(pcap_) {}

  void connect_cu_cp(ngap_message_handler& msg_handler_, ngap_event_handler& ev_handler_) override
  {
    msg_handler = &msg_handler_;
    ev_handler  = &ev_handler_;

    logger.info("Bypassing AMF connection");
  }

  void disconnect() override {}

  void on_new_message(const ngap_message& msg) override
  {
    srsran_assert(msg_handler != nullptr, "Adapter is disconnected");

    if (pcap_writer.is_write_enabled()) {
      byte_buffer   packed_pdu;
      asn1::bit_ref bref{packed_pdu};
      if (msg.pdu.pack(bref) == asn1::SRSASN_SUCCESS) {
        pcap_writer.push_pdu(std::move(packed_pdu));
      } else {
        logger.warning("Failed to encode NGAP Tx PDU.");
      }
    }

    if (msg.pdu.type().value == asn1::ngap::ngap_pdu_c::types_opts::init_msg and
        msg.pdu.init_msg().value.type().value ==
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ng_setup_request) {
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

private:
  void send_rx_pdu_to_cu_cp(const ngap_message& msg)
  {
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
    msg_handler->handle_message(msg);
  }

  dlt_pcap&             pcap_writer;
  ngap_message_handler* msg_handler = nullptr;
  ngap_event_handler*   ev_handler  = nullptr;
  srslog::basic_logger& logger      = srslog::fetch_basic_logger("GNB");
};

/// \brief NGAP bridge that uses the IO broker to handle the SCTP connection
class ngap_sctp_gateway_adapter : public ngap_gateway_connector,
                                  public sctp_network_gateway_control_notifier,
                                  public network_gateway_data_notifier
{
public:
  ngap_sctp_gateway_adapter(io_broker& broker_, const sctp_network_gateway_config& sctp_, dlt_pcap& pcap_) :
    broker(broker_), sctp_cfg(sctp_), pcap_writer(pcap_)
  {
    // Create SCTP network adapter.
    sctp_gateway = create_sctp_network_gateway(sctp_network_gateway_creation_message{sctp_cfg, *this, *this});
    if (sctp_gateway == nullptr) {
      report_error("Failed to create SCTP gateway.\n");
    }
  }

  ~ngap_sctp_gateway_adapter() override { disconnect(); }

  void disconnect() override
  {
    if (sctp_gateway != nullptr) {
      if (not broker.unregister_fd(sctp_gateway->get_socket_fd())) {
        logger.error("NGAP Gateway failed to stop SCTP socket");
      }
      packer.reset();
      sctp_gateway.reset();
    }
  }

  void connect_cu_cp(ngap_message_handler& msg_handler_, ngap_event_handler& ev_handler_) override
  {
    msg_handler = &msg_handler_;
    ev_handler  = &ev_handler_;

    // Create NGAP ASN1 packer.
    packer = std::make_unique<ngap_asn1_packer>(*sctp_gateway, *this, *msg_handler, pcap_writer);

    // Establish SCTP connection and register SCTP Rx message handler.
    logger.debug("Establishing TNL connection to AMF ({}:{})...", sctp_cfg.connect_address, sctp_cfg.connect_port);
    if (not sctp_gateway->create_and_connect()) {
      report_error("Failed to create SCTP gateway.\n");
      return;
    }
    logger.info("TNL connection to AMF ({}:{}) established", sctp_cfg.connect_address, sctp_cfg.connect_port);
    bool success = broker.register_fd(sctp_gateway->get_socket_fd(), [this](int fd) { sctp_gateway->receive(); });
    if (!success) {
      report_fatal_error("Failed to register N2 (SCTP) network gateway at IO broker. socket_fd={}",
                         sctp_gateway->get_socket_fd());
    }
  }

  // Called by CU-CP for each new Tx PDU.
  void on_new_message(const ngap_message& msg) override
  {
    srsran_assert(packer != nullptr, "Adapter is disconnected");
    packer->handle_message(msg);
  }

  // Called by io-broker for each Rx PDU.
  void on_new_pdu(byte_buffer pdu) override
  {
    srsran_assert(packer != nullptr, "Adapter is disconnected");
    packer->handle_packed_pdu(pdu);
  }

  void on_connection_established() override
  {
    srsran_assert(ev_handler != nullptr, "Adapter is disconnected");
    // TODO: Extend interface for connection reestablishments.
  }

  void on_connection_loss() override
  {
    srsran_assert(ev_handler != nullptr, "Adapter is disconnected");
    ev_handler->handle_connection_loss();
  }

private:
  io_broker&                        broker;
  const sctp_network_gateway_config sctp_cfg;
  dlt_pcap&                         pcap_writer;
  ngap_message_handler*             msg_handler = nullptr;
  ngap_event_handler*               ev_handler  = nullptr;
  srslog::basic_logger&             logger      = srslog::fetch_basic_logger("GNB");

  // SCTP network adapter
  std::unique_ptr<sctp_network_gateway> sctp_gateway;

  std::unique_ptr<ngap_asn1_packer> packer;
};

} // namespace

std::unique_ptr<ngap_gateway_connector> srsran::srs_cu_cp::create_ngap_gateway(const ngap_gateway_params& params)
{
  if (variant_holds_alternative<ngap_gateway_params::no_core>(params.mode)) {
    // Connection to local AMF stub.
    return std::make_unique<ngap_gateway_local_stub>(params.pcap);
  }

  // Connection to AMF through SCTP.
  const auto& nw_mode = variant_get<ngap_gateway_params::network>(params.mode);
  return std::make_unique<ngap_sctp_gateway_adapter>(nw_mode.broker, nw_mode.sctp, params.pcap);
}
