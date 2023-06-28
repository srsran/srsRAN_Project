/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_adapter.h"
#include "srsran/f1ap/common/f1ap_message.h"

using namespace srsran;

f1ap_local_adapter::f1ap_local_adapter(const std::string& log_name, dlt_pcap& f1ap_pcap_) :
  logger(srslog::fetch_basic_logger(log_name)), f1ap_pcap(f1ap_pcap_)
{
}

void f1ap_local_adapter::on_new_message(const f1ap_message& msg)
{
  report_fatal_error_if_not(handler, "F1AP message handler not set");
  logger.debug("Received a PDU of type {}", msg.pdu.type().to_string());

  if (f1ap_pcap.is_write_enabled()) {
    byte_buffer   buf;
    asn1::bit_ref bref(buf);
    if (msg.pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
      logger.error("Failed to pack PDU");
    } else {
      f1ap_pcap.push_pdu(std::move(buf));
    }
  }
  handler->handle_message(msg);
}
