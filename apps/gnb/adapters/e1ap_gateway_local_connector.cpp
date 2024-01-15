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

#include "e1ap_gateway_local_connector.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/pcap/dlt_pcap.h"

using namespace srsran;
using namespace srs_cu_cp;

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
  }

  void on_new_message(const e1ap_message& msg) override
  {
    logger.debug("Received a PDU of type {}", msg.pdu.type().to_string());

    if (pcap_writer.is_write_enabled()) {
      byte_buffer   buf;
      asn1::bit_ref bref(buf);
      if (msg.pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
        logger.error("Failed to pack PDU");
      } else {
        pcap_writer.push_pdu(std::move(buf));
      }
    }

    // Forward message to decorated class.
    decorated->on_new_message(msg);
  }

private:
  srslog::basic_logger&                  logger;
  dlt_pcap&                              pcap_writer;
  std::unique_ptr<e1ap_message_notifier> decorated;
};

} // namespace

e1ap_gateway_local_connector::e1ap_gateway_local_connector(dlt_pcap& e1ap_pcap_writer_) :
  e1ap_pcap_writer(e1ap_pcap_writer_)
{
}

void e1ap_gateway_local_connector::attach_cu_cp(srs_cu_cp::cu_up_repository& cu_cp_cu_up_mng_)
{
  cu_cp_cu_up_mng = &cu_cp_cu_up_mng_;
}

std::unique_ptr<e1ap_message_notifier> e1ap_gateway_local_connector::handle_cu_up_connection_request(
    std::unique_ptr<e1ap_message_notifier> cu_up_rx_pdu_notifier)
{
  report_fatal_error_if_not(cu_cp_cu_up_mng != nullptr, "CU-CP has not been attached to E1AP gateway.");

  // Decorate CU-UP RX notifier with pcap and logging.
  if (e1ap_pcap_writer.is_write_enabled()) {
    cu_up_rx_pdu_notifier = std::make_unique<e1ap_pdu_pcap_notifier>(
        std::move(cu_up_rx_pdu_notifier), e1ap_pcap_writer, srslog::fetch_basic_logger("CU-UP-E1"));
  }

  // Create direct connection between CU-CP and CU-UP notifier.
  auto cu_cp_rx_pdu_notifier = cu_cp_cu_up_mng->handle_new_cu_up_connection(std::move(cu_up_rx_pdu_notifier));

  // Decorate CU-CP RX notifier with pcap and logging.
  if (e1ap_pcap_writer.is_write_enabled()) {
    cu_cp_rx_pdu_notifier = std::make_unique<e1ap_pdu_pcap_notifier>(
        std::move(cu_cp_rx_pdu_notifier), e1ap_pcap_writer, srslog::fetch_basic_logger("CU-CP-E1"));
  }

  return cu_cp_rx_pdu_notifier;
}
