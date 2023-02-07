/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/pcap/ngap_pcap.h"
#include "srsgnb/adt/byte_buffer.h"
#include <stdint.h>

namespace srsgnb {

constexpr uint16_t NGAP_DLT = 152;

ngap_pcap::~ngap_pcap()
{
  close();
}

void ngap_pcap::open(const char* filename_)
{
  dlt_pcap_open(NGAP_DLT, filename_);
}

void ngap_pcap::close()
{
  dlt_pcap_close();
}

void ngap_pcap::write_pdu(srsgnb::const_span<uint8_t> pdu)
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
