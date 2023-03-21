/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/e1ap/common/e1ap_common.h"
#include "srsran/pcap/pcap.h"

namespace srsran {

/// \brief E1AP bridge between CU-CP and CU-UP using fast-path message passing.
class e1ap_local_adapter : public e1ap_message_notifier
{
public:
  explicit e1ap_local_adapter(const std::string& log_name_, dlt_pcap* e1ap_pcap_) :
    log_name(log_name_), logger(srslog::fetch_basic_logger(log_name_)), e1ap_pcap(e1ap_pcap_)
  {
  }

  void attach_handler(e1ap_message_handler* handler_) { handler = handler_; }
  void on_new_message(const e1ap_message& msg) override
  {
    report_fatal_error_if_not(handler, "E1AP message handler not set.");
    logger.debug("Received a PDU of type {}", msg.pdu.type().to_string());

    if (e1ap_pcap != nullptr && e1ap_pcap->is_write_enabled()) {
      byte_buffer   buf;
      asn1::bit_ref bref(buf);
      if (msg.pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
        logger.error("Failed to pack PDU");
        return;
      }
      e1ap_pcap->push_pdu(std::move(buf));
    }

    handler->handle_message(msg);
  }

private:
  std::string           log_name;
  srslog::basic_logger& logger;
  dlt_pcap*             e1ap_pcap = nullptr;
  e1ap_message_handler* handler   = nullptr;
};

}; // namespace srsran
