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

#include "f1c_gateway_local_connector.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/pcap/dlt_pcap.h"

using namespace srsran;
using namespace srs_cu_cp;

namespace {

/// Decorator for f1ap_message_notifier that writes the forwarded PDU to a pcap file.
class f1ap_pdu_pcap_notifier final : public f1ap_message_notifier
{
public:
  f1ap_pdu_pcap_notifier(std::unique_ptr<f1ap_message_notifier> decorated_,
                         dlt_pcap&                              pcap_writer_,
                         srslog::basic_logger&                  logger_) :
    logger(logger_), pcap_writer(pcap_writer_), decorated(std::move(decorated_))
  {
  }

  void on_new_message(const f1ap_message& msg) override
  {
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
  std::unique_ptr<f1ap_message_notifier> decorated;
};

} // namespace

f1c_gateway_local_connector::f1c_gateway_local_connector(dlt_pcap& f1ap_pcap_writer_) :
  f1ap_pcap_writer(f1ap_pcap_writer_)
{
}

void f1c_gateway_local_connector::attach_cu_cp(srs_cu_cp::cu_cp_f1c_handler& cu_cp_du_mng_)
{
  cu_cp_du_mng = &cu_cp_du_mng_;
}

std::unique_ptr<f1ap_message_notifier>
f1c_gateway_local_connector::handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier)
{
  report_fatal_error_if_not(cu_cp_du_mng != nullptr, "CU-CP has not been attached to F1-C gateway.");

  // Decorate DU RX notifier with pcap and logging.
  if (f1ap_pcap_writer.is_write_enabled()) {
    du_rx_pdu_notifier = std::make_unique<f1ap_pdu_pcap_notifier>(
        std::move(du_rx_pdu_notifier), f1ap_pcap_writer, srslog::fetch_basic_logger("DU-F1"));
  }

  // Create direct connection between CU-CP and DU notifier.
  auto cu_cp_rx_pdu_notifier = cu_cp_du_mng->handle_new_du_connection(std::move(du_rx_pdu_notifier));

  // Decorate CU-CP RX notifier with pcap and logging.
  if (f1ap_pcap_writer.is_write_enabled()) {
    cu_cp_rx_pdu_notifier = std::make_unique<f1ap_pdu_pcap_notifier>(
        std::move(cu_cp_rx_pdu_notifier), f1ap_pcap_writer, srslog::fetch_basic_logger("CU-CP-F1"));
  }

  return cu_cp_rx_pdu_notifier;
}
