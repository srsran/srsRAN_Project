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

    // Decorate DU RX notifier with pcap writing.
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

} // namespace

std::unique_ptr<e1_local_connector> srsran::create_e1_local_connector(const e1_local_connector_config& cfg)
{
  return std::make_unique<e1_local_connector_impl>(cfg);
}
