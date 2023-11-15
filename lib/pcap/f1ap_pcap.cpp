/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_pcap.h"
#include "srsran/adt/byte_buffer.h"
#include <stdint.h>

namespace srsran {

#define F1AP_DLT 154

f1ap_pcap::~f1ap_pcap()
{
  close();
}

void f1ap_pcap::open(const char* filename_)
{
  pcap_file_writer::open(F1AP_DLT, filename_);
}

void f1ap_pcap::close()
{
  pcap_file_writer::close();
}

void f1ap_pcap::write_pdu(srsran::const_span<uint8_t> pdu)
{
  if (!pcap_file_writer::is_write_enabled() || pdu.empty()) {
    // skip
    return;
  }

  // write packet header
  pcap_file_writer::write_pdu_header(pdu.size_bytes());

  // write PDU payload
  pcap_file_writer::write_pdu(pdu);
}

} // namespace srsran
