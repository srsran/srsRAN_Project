/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_pcap.h"
#include "srsgnb/adt/byte_buffer.h"
#include <stdint.h>

namespace srsgnb {

#define F1AP_DLT 154

f1ap_pcap::~f1ap_pcap()
{
  close();
}

void f1ap_pcap::open(const char* filename_)
{
  dlt_pcap_open(F1AP_DLT, filename_);
}
void f1ap_pcap::close()
{
  dlt_pcap_close();
}

void f1ap_pcap::write_pdu(srsgnb::const_span<uint8_t> pdu)
{
  if (!is_write_enabled() || pdu.empty()) {
    // skip
    return;
  }

  // write packet header
  write_pcap_header(pdu.size_bytes());

  // write PDU payload
  write_pcap_pdu(pdu);
}

} // namespace srsgnb
